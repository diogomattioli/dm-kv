#include <sys/resource.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <net/route.h>
#include <errno.h>
#include <pthread.h>

#include "query.h"

struct thread_t
{
    int socket;
    struct sockaddr_in addr;
    pthread_t id;
};

#define LISTEN_QUEUE_MAX (1024)

void *thread_func(void *arg)
{
    struct thread_t *thread = arg;

    char buf[1000 + 1];
    size_t len = 0;
    while ((len = read(thread->socket, buf, sizeof(buf) - 1)) > 0)
    {
        if (len == 1 && buf[0] == 4)
            break;

        if (len > 1 && buf[len - 2] == '\r' && buf[len - 1] == '\n')
            buf[len - 2] = '\0';

        buf[len] = 0;

        query_execute(thread->socket, buf);
    }

    close(thread->socket);
    free(thread);

    return NULL;
}

int main(int argc, char **argv)
{
    printf("%s-%s\n", GIT_BRANCH, GIT_REVISION);

    uint16_t port = 4321;

    for (int i = 1; i < argc; i++)
    {
        if (strlen(argv[i]) < 3)
            continue;

        uint16_t value = (uint16_t) atoi(&argv[i][2]);
        if (value == 0)
            continue;

        switch (argv[i][1])
        {
            case 'p':
                port = value;
                break;
            case 'm':
            {
                struct rlimit rl;
                rl.rlim_cur = rl.rlim_max = value * 1024 * 1024;
                setrlimit(RLIMIT_AS, &rl);
                break;
            }
        }
    }

    query_init();

    int socket_server, socket_client;
    struct sockaddr_in addr_server, addr_client;
    size_t addr_len = sizeof(struct sockaddr_in);

    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(1);

    int optval = 1;
    setsockopt(socket_server, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    memset(&addr_server, 0, sizeof(struct sockaddr_in));
    addr_server.sin_addr.s_addr = INADDR_ANY;
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(port);
    if (bind(socket_server, (struct sockaddr *) &addr_server, addr_len) == -1)
        exit(1);

    if (listen(socket_server, LISTEN_QUEUE_MAX) == -1)
        exit(1);

    while ((socket_client = accept(socket_server, (struct sockaddr *) &addr_client, (socklen_t *) &addr_len)) != -1)
    {
        struct thread_t *thread = malloc(sizeof(struct thread_t));
        memset(thread, 0, sizeof(struct thread_t));
        thread->socket = socket_client;
        memcpy(&thread->addr, &addr_client, addr_len);
        pthread_create(&thread->id, NULL, thread_func, thread);
    }

    return 0;
}

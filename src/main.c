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

#define MAX_CONNECTIONS (10)

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

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    size_t addr_len = sizeof(struct sockaddr_in);

    query_init();

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        exit(1);

    int optval = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4321);
    if (bind(server_socket, (struct sockaddr *) &server_addr, addr_len) == -1)
        exit(1);

    if (listen(server_socket, MAX_CONNECTIONS) == -1)
        exit(1);

    while ((client_socket = accept(server_socket, (struct sockaddr *) &client_addr, (socklen_t *) &addr_len)) != -1)
    {
        struct thread_t *thread = malloc(sizeof(struct thread_t));
        memset(thread, 0, sizeof(struct thread_t));
        thread->socket = client_socket;
        memcpy(&thread->addr, &client_addr, addr_len);
        pthread_create(&thread->id, NULL, thread_func, thread);
    }

    return 0;
}

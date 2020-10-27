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

#if 0
uint64_t diffts(struct timespec ts, struct timespec ts2)
{
    uint64_t t = (ts2.tv_sec - ts.tv_sec) * 1000000000;
    t += (ts2.tv_nsec - ts.tv_nsec);
    return t;
}
#endif

struct _thread
{
    int socket;
    struct sockaddr_in addr;
    pthread_t id;
};

#define MAX_CONNECTIONS (10)

void *thread_func(void *arg)
{
    struct _thread *thread = arg;

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

#if 0
    char queries[][100] = {
            "+/22:doing it",
            "./22",
            "-/22",
            "./node/",
            "+/node/subnode:testing",
            "./node/subnode",
            "-/node/",
            "+/node",
            "+/node/subnode",
            "+/node/subnode/subsubnode:some text here",
            "./node/subnode/subsubnode",
    };

    for (int i = 0; i < sizeof(queries) / sizeof(queries[0]); i++)
    {
        printf("%d - %s\n", i, queries[i]);

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);

        query_execute(1, queries[i]);

        struct timespec ts2;
        clock_gettime(CLOCK_REALTIME, &ts2);

        uint64_t diff = diffts(ts, ts2);

        printf("%6luns\n", diff);
    }
#endif

#if 0
#define ENTRIES 1000000

    char qu[100] = {0};

    struct timespec ts;
    struct timespec ts2;
    uint64_t diff = 0;
    uint64_t difftotal = 0;

    difftotal = 0;
    for (int i = 0; i < ENTRIES; i++) {
        snprintf(qu, sizeof(qu), "+/%d\n%d", i, i);

        clock_gettime(CLOCK_REALTIME, &ts);
        query_execute(-1, qu);
        clock_gettime(CLOCK_REALTIME, &ts2);

        diff = diffts(ts, ts2);
        difftotal += diff;
    }

    printf("INSERT average %6luns\n", difftotal / ENTRIES);

    difftotal = 0;
    for (int i = 0; i < ENTRIES; i++) {
        snprintf(qu, sizeof(qu), "./%d\n%d", i, i);

        clock_gettime(CLOCK_REALTIME, &ts);
        query_execute(0, qu);
        clock_gettime(CLOCK_REALTIME, &ts2);

        diff = diffts(ts, ts2);
        difftotal += diff;
    }

    printf("FIND average %6luns\n", difftotal / ENTRIES);

    difftotal = 0;
    for (int i = 0; i < ENTRIES; i++) {
        snprintf(qu, sizeof(qu), "-/%d\n%d", i, i);

        clock_gettime(CLOCK_REALTIME, &ts);
        query_execute(-1, qu);
        clock_gettime(CLOCK_REALTIME, &ts2);

        diff = diffts(ts, ts2);
        difftotal += diff;
    }

    printf("DELETE average %6luns\n", difftotal / ENTRIES);

    return 0;
#endif

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
        struct _thread *thread = malloc(sizeof(struct _thread));
        memset(thread, 0, sizeof(struct _thread));
        thread->socket = client_socket;
        memcpy(&thread->addr, &client_addr, addr_len);
        pthread_create(&thread->id, NULL, thread_func, thread);
    }

    return 0;
}

#ifndef RPC_H
#define RPC_H

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif
void rpc_start(const char *addr, u_int16_t port);
void rpc_start_detached(const char *addr, u_int16_t port);
void rpc_stop();
#ifdef __cplusplus
}
#endif

#endif // RPC_H

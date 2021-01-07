#include <thread>

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "query_service.h"
#include "rpc.h"

static std::unique_ptr<grpc::Server> server;

void rpc_start(const char *addr, u_int16_t port)
{
    if (server || addr == nullptr || port == 0)
        return;

    std::string address(std::string(addr) + ":" + std::to_string(port));
    QueryService service;

    grpc::EnableDefaultHealthCheckService(true);
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    server = builder.BuildAndStart();
    server->Wait();
    server.reset();
}

void rpc_start_detached(const char *addr, u_int16_t port)
{
    std::thread t(rpc_start, addr, port);
    t.detach();
}

void rpc_stop()
{
    if (!server)
        return;

    server->Shutdown();
}
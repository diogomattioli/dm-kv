#ifndef QUERY_SERVICE_H
#define QUERY_SERVICE_H

#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

#include "query.grpc.pb.h"
#include "query.pb.h"

class QueryService final : public dm_kv::Query::Service {
public:
    grpc::Status Insert(grpc::ServerContext *context, const dm_kv::QueryRequest *request, dm_kv::QueryResponse *response) override;
    grpc::Status Update(grpc::ServerContext *context, const dm_kv::QueryRequest *request, dm_kv::QueryResponse *response) override;
    grpc::Status Delete(grpc::ServerContext *context, const dm_kv::QueryRequest *request, dm_kv::QueryResponse *response) override;
    grpc::Status Retrieve(grpc::ServerContext *context, const dm_kv::QueryRequest *request, dm_kv::QueryResponse *response) override;
};

#endif // QUERY_SERVICE_H

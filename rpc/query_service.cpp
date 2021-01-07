#include "query_service.h"

grpc::Status QueryService::Insert(grpc::ServerContext *context, const dm_kv::QueryRequest *request, dm_kv::QueryResponse *response)
{
    (void)context;
    (void)request;

    response->set_message(__PRETTY_FUNCTION__);
    return grpc::Status::OK;
}

grpc::Status QueryService::Update(grpc::ServerContext *context, const dm_kv::QueryRequest *request, dm_kv::QueryResponse *response)
{
    (void)context;
    (void)request;

    response->set_message(__PRETTY_FUNCTION__);
    return grpc::Status::OK;
}

grpc::Status QueryService::Delete(grpc::ServerContext *context, const dm_kv::QueryRequest *request, dm_kv::QueryResponse *response)
{
    (void)context;
    (void)request;

    response->set_message(__PRETTY_FUNCTION__);
    return grpc::Status::OK;
}

grpc::Status QueryService::Retrieve(grpc::ServerContext *context, const dm_kv::QueryRequest *request, dm_kv::QueryResponse *response)
{
    (void)context;
    (void)request;

    response->set_message(__PRETTY_FUNCTION__);
    return grpc::Status::OK;
}

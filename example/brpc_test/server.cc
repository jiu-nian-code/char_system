#include<brpc/server.h>
#include<butil/logging.h>
#include"main.pb.h"

class EchoServiceImpl : public example::EchoService
{
public:
    EchoServiceImpl(){}
    ~EchoServiceImpl(){}
    virtual void Echo(google::protobuf::RpcController* controller,
        const ::example::EchoRequest* request,
        ::example::EchoResponse* response,
        ::google::protobuf::Closure* done)
    {
        brpc::ClosureGuard cg(done);
        std::string req =  request->message();
        std::cout << "收到消息"  << req << std::endl;
        response->set_message("回复：" + req);
        // done->Run();
    }
};

int main()
{
    logging::LoggingSettings settings;
    settings.logging_dest =  logging::LoggingDestination::LOG_TO_NONE;
    logging::InitLogging(settings);
    brpc::Server server;
    EchoServiceImpl echo_server;
    int ret = server.AddService(&echo_server, brpc::ServiceOwnership::SERVER_DOESNT_OWN_SERVICE);
    if (ret == -1) {
        std::cout << "添加Rpc服务失败！\n";
        return -1;
    }
    brpc::ServerOptions options;
    options.idle_timeout_sec = -1;
    options.num_threads = 1;
    server.Start(8080, &options);
    if (ret == -1) {
        std::cout << "启动服务器失败！\n";
        return -1;
    }
    server.RunUntilAskedToQuit();
    return 0;
}
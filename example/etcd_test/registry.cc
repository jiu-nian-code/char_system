#include<iostream>
#include<etcd/Client.hpp>
#include<etcd/Response.hpp>
#include<etcd/KeepAlive.hpp>
#include<thread>
#include"etcd.hpp"
#include<brpc/server.h>
#include<butil/logging.h>
#include"../brpc_test/main.pb.h"
#include<gflags/gflags.h>
DEFINE_bool(mode, false, "mode - 运行模式： true-发布模式，日志通过文件输出； false-调试模式，日志通过屏幕输出。");
DEFINE_string(file, "log/default_log.txt", "若为发布模式，则指定日志文件，默认为：log/default_log.txt。");
DEFINE_int32(level, 1, "是否开启地址重用");//1是debug等级，查阅spdlog::level::level_enum可知

// int main()
// {
//     std::string registry_host = "http://127.0.0.1:2379";
//     std::string service_key = "/service/user/instance";
//     std::string service_host = "127.0.0.1:9090";
//     etcd::Client etcd(registry_host);
//     auto keepalive = etcd.leasekeepalive(3).get();
//     auto lease_id = keepalive->Lease();
//     auto resp_task = etcd.put(service_key, service_host, lease_id);
//     auto resp = resp_task.get();
//     if(resp.is_ok() == false)
//     {
//         std::cout << resp.error_message() << std::endl;
//         return -1;
//     }
//     std::cout << "添加数据成功！" << std::endl;
//     getchar();
//     etcd.leaserevoke(lease_id);
//     return 0;
// }
// int main(int argc, char* argv[])
// {
//     google::ParseCommandLineFlags(&argc, &argv, true);
//     init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
//     thx::Registry reg("http://127.0.0.1:2379");
//     reg.registry("/service/user/instance", "127.0.0.1:9090");
//     getchar();
//     return 0;
// }
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

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
    thx::Registry reg("http://127.0.0.1:2379");
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
    reg.registry("/service/echo/instance_1", "127.0.0.1:8080");
    server.RunUntilAskedToQuit();
}
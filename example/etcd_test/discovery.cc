#include<iostream>
#include<etcd/Client.hpp>
#include<etcd/Response.hpp>
#include<etcd/KeepAlive.hpp>
#include<etcd/Watcher.hpp>
#include<thread>
#include"etcd.hpp"
#include<gflags/gflags.h>
#include<brpc/channel.h>
#include<chrono>
#include"../brpc_test/main.pb.h"
#include"channel.hpp"
DEFINE_bool(mode, false, "mode - 运行模式： true-发布模式，日志通过文件输出； false-调试模式，日志通过屏幕输出。");
DEFINE_string(file, "log/default_log.txt", "若为发布模式，则指定日志文件，默认为：log/default_log.txt。");
DEFINE_int32(level, 1, "是否开启地址重用");//1是debug等级，查阅spdlog::level::level_enum可知

// void watcher_callback(etcd::Response const& resp)
// {
//     if(resp.error_code())
//     {
//         std::cout << "Watcher Error:" << resp.error_code();
//         std::cout << "-" << resp.error_message() << std::endl;
//     }
//     else
//     {
//         for(auto const& ev : resp.events())
//         {
//             if(ev.event_type() == etcd::Event::EventType::PUT)
//             {
//                 std::cout <<  "服务" << ev.kv().key() << "新增主机 ：" <<  ev.kv().as_string() << std::endl;
//             }
//             else if(ev.event_type() == etcd::Event::EventType::DELETE_)
//             {
//                 std::cout <<  "服务" << ev.kv().key() << "下线主机 ：" <<  ev.prev_kv().as_string() << std::endl;
//             }
//         }
//     }
// }

// int main()
// {
//     std::string registry_host = "http://127.0.0.1:2379";
//     std::string service_key = "/service/user/instance";
//     etcd::Client etcd(registry_host);
//     etcd::Response resp = etcd.ls(service_key).get();
//     if(resp.is_ok())
//     {
//         for(int i = 0; i < resp.keys().size(); ++i)
//         {
//             std::cout << resp.key(i) << "-" << resp.value(i).as_string() << std::endl;
//         }
//     }
//     else
//     {
//         std::cout << "Get Service Error:" << resp.error_code();
//         std::cout << "-" << resp.error_message() << std::endl;
//     }
//     etcd::Watcher watcher(registry_host, service_key, watcher_callback, true);
//     getchar();
//     watcher.Cancel();
//     return 0;
// }

// void func(std::string str1, std::string str2)
// {
//     std::cout << str1 << "@^@" << str2 << std::endl;
// }

// int main(int argc, char* argv[])
// {
//     google::ParseCommandLineFlags(&argc, &argv, true);
//     init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
//     thx::Discovery disc("http://127.0.0.1:2379", "/service/user/instance", func, func);
//     getchar();
// }
int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
    thx::ServiceManager sm;
    sm.declared("/service/echo");
    auto put_cb = std::bind(&thx::ServiceManager::onServiceOnline, &sm, std::placeholders::_1, std::placeholders::_2);
    auto del_cb = std::bind(&thx::ServiceManager::onServiceOffline, &sm, std::placeholders::_1, std::placeholders::_2);
    thx::Discovery disc("http://127.0.0.1:2379", "/service/echo", put_cb, del_cb);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    example::EchoService_Stub stub(sm.choose("/service/echo").get());
    example::EchoRequest req;
    req.set_message("hello world");
    brpc::Controller* controller_ptr = new brpc::Controller;
    example::EchoResponse *response_ptr =  new example::EchoResponse;
    stub.Echo(controller_ptr, &req, response_ptr, nullptr);
    if (controller_ptr->Failed() == true) {
        std::cout << "Rpc调用失败：" << controller_ptr->ErrorText() << std::endl;
        return -1;
    }
    std::cout << "收到消息：" << response_ptr->message() << std::endl;
    delete controller_ptr;
    delete response_ptr;
    getchar();
}
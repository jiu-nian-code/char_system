#include<iostream>
#include<etcd/Client.hpp>
#include<etcd/Response.hpp>
#include<etcd/KeepAlive.hpp>
#include<etcd/Watcher.hpp>
#include<thread>
#include"etcd.hpp"
#include<gflags/gflags.h>
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

void func(std::string str1, std::string str2)
{
    std::cout << str1 << "@^@" << str2 << std::endl;
}

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
    thx::Discovery disc("http://127.0.0.1:2379", "/service/user/instance", func, func);
    getchar();
}
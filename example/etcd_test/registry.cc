#include<iostream>
#include<etcd/Client.hpp>
#include<etcd/Response.hpp>
#include<etcd/KeepAlive.hpp>
#include<thread>
#include"etcd.hpp"
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
int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
    thx::Registry reg("http://127.0.0.1:2379");
    reg.registry("/service/user/instance", "127.0.0.1:9090");
    getchar();
    return 0;
}
// #include <ev.h>
// #include <amqpcpp.h>
// #include <amqpcpp/libev.h>
// #include <openssl/ssl.h>
// #include <openssl/opensslv.h>

// int main()
// {
//     auto *loop = EV_DEFAULT;
//     AMQP::LibEvHandler handler(loop);
//     AMQP::Address address("amqp://root:123456@127.0.0.1:5672/");
//     AMQP::TcpConnection connection(&handler, address);
//     AMQP::TcpChannel channel(&connection);
//     channel.declareExchange("test-exchange", AMQP::ExchangeType::direct)
//         .onError([](const char* message){
//             std::cout << "声明交换机失败：" << message << std::endl;
//             exit(0);
//         })
//         .onSuccess([](){
//             std::cout << "test-exchange 交换机创建成功！" << std::endl;
//         });
//     channel.declareQueue("test-queue")
//         .onError([](const char* message){
//             std::cout << "声明队列失败：" << message << std::endl;
//             exit(0);
//         })
//         .onSuccess([](){
//             std::cout << "test-queue 队列创建成功！" << std::endl;
//         });
//     channel.bindQueue("test-exchange", "test-queue", "test-queue-key")
//         .onError([](const char *message){
//             std::cout << "test-exchange - test-queue 绑定失败：" << message << std::endl;
//             exit(0);
//         })
//         .onSuccess([](){
//             std::cout << "test-exchange - test-queue 绑定成功！" << std::endl;
//         });
//     for(int i = 0; i < 10; ++i)
//     {
//         std::string msg = "hello world" + std::to_string(i);
//         bool ret = channel.publish("test-exchange", "test-queue-key", msg);
//         if (ret == false)
//             std::cout << "publish 失败！\n";
//     }
//     ev_run(loop, 0);
//     return 0;
// }

#include"rabbit_mq.hpp"
#include<gflags/gflags.h>
DEFINE_bool(mode, false, "mode - 运行模式： true-发布模式，日志通过文件输出； false-调试模式，日志通过屏幕输出。");
DEFINE_string(file, "log/default_log.txt", "若为发布模式，则指定日志文件，默认为：log/default_log.txt。");
DEFINE_int32(level, 1, "是否开启地址重用");//1是debug等级，查阅spdlog::level::level_enum可知

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
    thx::MQClient client("root", "123456", "127.0.0.1:5672");
    client.declare_combo("test-exchange", "test-queue");
    // std::this_thread::sleep_for(std::chrono::seconds(10));
    for(int i = 0; i < 10; ++i)
    {
        std::string msg = "hello world" + std::to_string(i);
        bool ret = client.publish("test-exchange", msg);
        if (ret == false) {
            std::cout << "publish 失败！\n";
        }
        else{
            std::cout << msg <<" publish 成功！\n";
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
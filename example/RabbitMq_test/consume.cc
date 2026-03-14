// #include <ev.h>
// #include <amqpcpp.h>
// #include <amqpcpp/libev.h>
// #include <openssl/ssl.h>
// #include <openssl/opensslv.h>

// void callback(AMQP::TcpChannel *channel, const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)
// {
//     std::string msg;
//     msg.assign(message.body(), message.bodySize());
//     std::cout << msg << std::endl;
//     channel->ack(deliveryTag);
// }

// int main()
// {
//     auto *loop = EV_DEFAULT;
//     AMQP::LibEvHandler handler(loop);
//     AMQP::Address address("amqp://root:123456@127.0.0.1:5672/");
//     AMQP::TcpConnection connection(&handler, address);
//     AMQP::TcpChannel channel(&connection); // 一个连接可以有多个信道，复用
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
//     auto cb = std::bind(callback, &channel, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
//     channel.consume("test-queue", "consume-tag")
//         .onReceived(cb)
//         .onError([](const char *message){
//             std::cout << "订阅 test-queue 队列消息失败:" << message << std::endl;
//             exit(0);
//         });
//     ev_run(loop, 0);
// }

#include"rabbit_mq.hpp"
#include<gflags/gflags.h>
DEFINE_bool(mode, false, "mode - 运行模式： true-发布模式，日志通过文件输出； false-调试模式，日志通过屏幕输出。");
DEFINE_string(file, "log/default_log.txt", "若为发布模式，则指定日志文件，默认为：log/default_log.txt。");
DEFINE_int32(level, 1, "是否开启地址重用");//1是debug等级，查阅spdlog::level::level_enum可知

void callback(const char* str, size_t sz)
{
    std::string msg;
    msg.assign(str, sz);
    std::cout << msg << std::endl;
}

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
    thx::MQClient client("root", "123456", "127.0.0.1:5672");
    client.declare_combo("test-exchange", "test-queue");
    client.consume("test-queue", callback);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

void callback(AMQP::TcpChannel *channel, const AMQP::Message &message, uint64_t deliveryTag, bool redelivered)
{
    std::string msg;
    msg.assign(message.body(), message.bodySize());
    std::cout << msg << std::endl;
    channel->ack(deliveryTag);
}

int main()
{
    auto *loop = EV_DEFAULT;
    AMQP::LibEvHandler handler(loop);
    AMQP::Address address("amqp://root:123456@127.0.0.1:5672/");
    AMQP::TcpConnection connection(&handler, address);
    AMQP::TcpChannel channel(&connection);
    channel.declareExchange("test-exchange", AMQP::ExchangeType::direct)
        .onError([](const char* message){
            std::cout << "声明交换机失败：" << message << std::endl;
            exit(0);
        })
        .onSuccess([](){
            std::cout << "test-exchange 交换机创建成功！" << std::endl;
        });
    channel.declareQueue("test-queue")
        .onError([](const char* message){
            std::cout << "声明队列失败：" << message << std::endl;
            exit(0);
        })
        .onSuccess([](){
            std::cout << "test-queue 队列创建成功！" << std::endl;
        });
    channel.bindQueue("test-exchange", "test-queue", "test-queue-key")
        .onError([](const char *message){
            std::cout << "test-exchange - test-queue 绑定失败：" << message << std::endl;
            exit(0);
        })
        .onSuccess([](){
            std::cout << "test-exchange - test-queue 绑定成功！" << std::endl;
        });
    auto cb = std::bind(callback, &channel, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    channel.consume("test-queue", "consume-tag")
        .onReceived(cb)
        .onError([](const char *message){
            std::cout << "订阅 test-queue 队列消息失败:" << message << std::endl;
            exit(0);
        });
    ev_run(loop, 0);
}
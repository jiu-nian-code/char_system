#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

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
    for(int i = 0; i < 10; ++i)
    {
        std::string msg = "hello world" + std::to_string(i);
        bool ret = channel.publish("test-exchange", "test-queue-key", msg);
        if (ret == false)
            std::cout << "publish 失败！\n";
    }
    ev_run(loop, 0);
    return 0;
}
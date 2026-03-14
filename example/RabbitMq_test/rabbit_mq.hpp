#pragma once
#include"logger.hpp"
#include <ev.h>
#include <amqpcpp.h>
#include <amqpcpp/libev.h>
#include <openssl/ssl.h>
#include <openssl/opensslv.h>

namespace thx
{
    class MQClient
    {
        struct ev_loop* _loop;
        std::unique_ptr<AMQP::LibEvHandler> _handler;
        std::unique_ptr<AMQP::Address> _address;
        std::unique_ptr<AMQP::TcpConnection> _connection;
        std::unique_ptr<AMQP::TcpChannel> _channel;
        std::thread _loop_thread;
        struct ev_async _async_watcher;
        static void watcher_callback(struct ev_loop *loop, ev_async *watcher, int32_t revents)
        {
            ev_break(loop, EVBREAK_ALL);
        }
    public:
        using MessageCallback = std::function<void(const char*, size_t)>;
        using ptr = std::shared_ptr<MQClient>;
        MQClient(const std::string& user, const std::string& passwd, const std::string& host)
        {
            _loop = EV_DEFAULT;
            _handler = std::make_unique<AMQP::LibEvHandler>(_loop);
            std::string url = "amqp://" + user + ":" + passwd + "@" + host + "/";
            _address = std::make_unique<AMQP::Address>(url);
            _connection = std::make_unique<AMQP::TcpConnection>(_handler.get(), *_address);
            _channel = std::make_unique<AMQP::TcpChannel>(_connection.get());
            _loop_thread = std::thread([this]() {
                ev_run(_loop, 0);
            });
        }
        ~MQClient()
        {
            ev_async_init(&_async_watcher, watcher_callback);
            ev_async_start(_loop, &_async_watcher);
            ev_async_send(_loop, &_async_watcher);
            _loop_thread.join();
            _loop = nullptr;
        }
        void declare_combo(const std::string &exchange,
            const std::string &queue,
            const std::string &routing_key = "routing_key",
            AMQP::ExchangeType echange_type = AMQP::ExchangeType::direct)
        {
            _channel->declareExchange(exchange, echange_type)
                .onError([](const char* message){
                    LOG_ERROR("声明交换机失败：{}", message);
                    exit(0);
                })
                .onSuccess([](){
                    LOG_DEBUG("test-exchange 交换机创建成功！");
                });
            _channel->declareQueue(queue)
                .onError([](const char* message){
                    LOG_ERROR("声明队列失败：{}", message);
                    exit(0);
                })
                .onSuccess([](){
                    LOG_DEBUG("test-queue 队列创建成功！");
                });
            _channel->bindQueue(exchange, queue, routing_key)
                .onError([](const char *message){
                    LOG_ERROR("test-exchange - test-queue 绑定失败：{}", message);
                    exit(0);
                })
                .onSuccess([](){
                    LOG_DEBUG("test-exchange - test-queue 绑定成功！");
                });
        }
        bool publish(const std::string &exchange, 
            const std::string &msg, 
            const std::string &routing_key = "routing_key")
        {
            bool ret = _channel->publish(exchange, routing_key, msg);
            if (ret == false)
            {
                LOG_ERROR("{} 发布消息失败：", exchange);
                return false;
            }
            return true;
        }
        void consume(const std::string& queue, const MessageCallback& cb)
        {
            LOG_DEBUG("开始订阅 {} 队列消息！", queue);
            _channel->consume(queue, "consume-tag")
            .onReceived([this, cb](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered){
                cb(message.body(), message.bodySize());
                _channel->ack(deliveryTag);
            })
            .onError([queue](const char *message){
                LOG_ERROR("订阅 {} 队列消息失败: {}", queue, message);
                exit(0);
            });
        }
    };
}
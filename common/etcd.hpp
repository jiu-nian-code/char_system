#pragma once
#include<etcd/Client.hpp>
#include<etcd/KeepAlive.hpp>
#include<etcd/Response.hpp>
#include<etcd/Watcher.hpp>
#include<etcd/Value.hpp>
#include<functional>
#include"logger.hpp"

namespace thx
{
    class Registry
    {
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::KeepAlive> _keep_alive;
        uint64_t _lease_id;
    public:
        Registry(const std::string& host):
            _client(std::make_shared<etcd::Client>(host)),
            _keep_alive(_client->leasekeepalive(3).get()),
            _lease_id(_keep_alive->Lease()) {}
        ~Registry() { _keep_alive->Cancel(); }
        bool registry(const std::string& key, const std::string& value)
        {
            auto resp = _client->put(key, value, _lease_id).get();
            if(resp.is_ok() == false)
            {
                LOG_ERROR("注册数据失败：{}", resp.error_message());
                return false;
            }
            return true;
        }
    };
    class Discovery
    {
    public:
        using ptr = std::shared_ptr<Discovery>;
        using NotifyCallback = std::function<void(std::string, std::string)>;
        Discovery(const std::string& host,
        const std::string& base_dir,
        const NotifyCallback& put_cb,
        const NotifyCallback& del_cb):
            _client(std::make_shared<etcd::Client>(host)) ,
            _put_cb(put_cb),
            _del_cb(del_cb)
        {
            auto resp = _client->ls(base_dir).get();
            if(resp.is_ok() == false)
            {
                LOG_ERROR("获取服务信息数据失败：{}", resp.error_message());
            }
            for(int i = 0; i < resp.keys().size(); ++i)
                if(_put_cb) _put_cb(resp.key(i), resp.value(i).as_string());
            _watcher = std::make_shared<etcd::Watcher>(*_client, base_dir, 
                std::bind(&Discovery::callback, this, std::placeholders::_1), true);
        }
    private:
        void callback(const etcd::Response &resp)
        {
            if(resp.error_code())
            {
                LOG_ERROR("收到一个错误的事件通知: {}", resp.error_message());
                return;
            }
            for(auto const& ev : resp.events())
            {
                if(ev.event_type() == etcd::Event::EventType::PUT)
                {
                    if (_put_cb) _put_cb(ev.kv().key(), ev.kv().as_string());
                    LOG_DEBUG("新增服务：{}-{}", ev.kv().key(), ev.kv().as_string());
                }
                else if(ev.event_type() == etcd::Event::EventType::DELETE_)
                {
                    if (_del_cb) _del_cb(ev.prev_kv().key(), ev.prev_kv().as_string());
                    LOG_DEBUG("下线服务：{}-{}", ev.prev_kv().key(), ev.prev_kv().as_string());
                }
            }
        }
        NotifyCallback _put_cb;
        NotifyCallback _del_cb;
        std::shared_ptr<etcd::Client> _client;
        std::shared_ptr<etcd::Watcher> _watcher;
    };
}
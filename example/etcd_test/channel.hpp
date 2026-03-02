#pragma once
#include"logger.hpp"
#include<brpc/channel.h>
#include<vector>
#include<mutex>
#include<unordered_map>
#include<unordered_set>

namespace thx
{
    class ServiceChannel
    {
    public:
        using ptr = std::shared_ptr<ServiceChannel>;
        using Channelptr = std::shared_ptr<brpc::Channel>;
        ServiceChannel(const std::string& service_name):
            _rotation_count(0),
            _service_name(service_name){}
        void append(const std::string& host)
        {
            brpc::ChannelOptions options;
            options.connect_timeout_ms = -1;
            options.timeout_ms = -1;
            options.max_retry = 3;
            options.protocol = "baidu_std";
            Channelptr new_ptr(new brpc::Channel);
            int ret = new_ptr->Init(host.c_str(), &options);
            if (ret == -1)
            {
                LOG_ERROR("{}-{} 初始化信道失败！", _service_name, host);
                return;
            }
            std::unique_lock<std::mutex> lock(_mt);
            _rotating_array.push_back(new_ptr);
            _hosts[host] = new_ptr;
        }
        void remove(const std::string& host)
        {
            std::cout << "remove " << host << std::endl;
            std::unique_lock<std::mutex> lock(_mt);
            auto it = _hosts.find(host);
            if(it == _hosts.end())
            {
                LOG_WARN("{} 中没有找到 {} 服务", _service_name, host);
                return;
            }
            bool is_success = false;
            for(auto vit = _rotating_array.begin(); vit != _rotating_array.end(); ++vit)
            {
                if (*vit == it->second)
                {
                    is_success = true;
                    _rotating_array.erase(vit);
                    break;
                }
            }
            // if(vit == _rotating_array.end()) // 迭代器失效
            // {
            //     LOG_WARN("程序逻辑错误，{} 中哈希表记录了但是轮转数组中没有 {} 服务！", _service_name, host);
            //     return;
            // }
            if(!is_success)
            {
                LOG_WARN("程序逻辑错误，{} 中哈希表记录了但是轮转数组中没有 {} 服务！", _service_name, host);
                return;
            }
            _hosts.erase(it);
        }
        Channelptr choose()
        {
            std::unique_lock<std::mutex> lock(_mt);
            if(_rotating_array.size() == 0)
            {
                LOG_ERROR("当前没有能提供 {} 服务的节点！", _service_name);
                return Channelptr();
            }
            int32_t cur_rotation_count = _rotation_count;
            _rotation_count = (_rotation_count + 1) % _rotating_array.size();
            return _rotating_array[cur_rotation_count];
        }
    private:
        std::mutex _mt;
        int32_t _rotation_count;
        std::string _service_name;
        std::vector<Channelptr> _rotating_array;
        std::unordered_map<std::string, Channelptr> _hosts;
    };

    class ServiceManager
    {
    public:
        using ptr = std::shared_ptr<ServiceManager>;
        ServiceManager() {}
        void declared(const std::string& service)
        {
            std::unique_lock<std::mutex> lock(_mt);
            _follow_services.insert(service);
        }
        ServiceChannel::Channelptr choose(const std::string& service_name)
        {
            std::unique_lock<std::mutex> lock(_mt);
            auto it = _services.find(service_name);
            if(it == _services.end())
            {
                LOG_WARN("但提前没有提供 {} 服务的节点！", service_name);
                return ServiceChannel::Channelptr();
            }
            return it->second->choose();
        }
        void onServiceOnline(const std::string &service_instance, const std::string &host)
        {
            std::string service_name = getServiceName(service_instance);
            ServiceChannel::ptr service;
            {
                std::unique_lock<std::mutex> lock(_mt);
                if(_follow_services.count(service_name) == 0)
                {
                    LOG_DEBUG("{}-{} 服务上线了，但是当前并不关心！", service_name, host);
                    return;
                }
                auto it = _services.find(service_name);
                if(it == _services.end())
                {
                    service = std::make_shared<ServiceChannel>(service_name);
                    _services[service_name] = service;
                }
                else service = it->second;
            }
            if (!service)
            {
                LOG_ERROR("新增 {} 服务管理节点失败！", service_name);
                return;
            }
            service->append(host);
            LOG_DEBUG("{}-{} 服务上线新节点，进行添加管理！", service_name, host);
        }
        void onServiceOffline(const std::string &service_instance, const std::string &host)
        {
            std::string service_name = getServiceName(service_instance);
            ServiceChannel::ptr service;
            {
                std::unique_lock<std::mutex> lock(_mt);
                if(_follow_services.count(service_name) == 0)
                {
                    LOG_DEBUG("{}-{} 服务下线了，但是当前并不关心！", service_name, host);
                    return;
                }
                auto it = _services.find(service_name);
                if(it == _services.end())
                {
                    LOG_WARN("删除 {} 服务节点时，没有找到管理对象", service_name);
                    return;
                }
                service = it->second;
            }
            service->remove(host);
            LOG_DEBUG("{}-{} 服务下线节点，进行删除管理！", service_name, host);
        }
    private:
        std::string getServiceName(const std::string &service_instance)
        {
            auto pos = service_instance.find_last_of('/');
            if (pos == std::string::npos) return service_instance;
            return service_instance.substr(0, pos);
        }
        std::mutex _mt;
        std::unordered_set<std::string> _follow_services;
        std::unordered_map<std::string, ServiceChannel::ptr> _services;
    };
}
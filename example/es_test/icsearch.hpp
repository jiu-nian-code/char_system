#pragma once
#include <elasticlient/client.h>
#include <cpr/cpr.h>
#include <json/json.h>
#include <iostream>
#include <memory>
#include<exception>
#include "../../common/logger.hpp"

namespace thx
{
    class Json_Util
    {
    public:
        static bool serialize(const Json::Value &root, std::string &str)
        {
            Json::StreamWriterBuilder swb;
            std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
            std::stringstream ss;
            int ret = sw->write(root, &ss);
            if(ret < 0) { LOG_WARN("Json::StreamWriter::write: {}", strerror(errno)); return false; }
            str = ss.str();
            return true;
        }
        static bool unserialize(const std::string &str, Json::Value &root)
        {
            Json::CharReaderBuilder crb;
            std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
            bool ret = cr->parse(str.c_str(), str.c_str() + str.size(), &root, nullptr);
            if(!ret) { LOG_WARN("Json::CharReader::parse: {}", strerror(errno)); return false; }
            return true;
        }
    };
    class ESIndex
    {
        std::shared_ptr<elasticlient::Client> _client;
        std::string _indexName;
        std::string _docType;
        Json::Value _properties;
        Json::Value _index;
    public:
        ESIndex(std::shared_ptr<elasticlient::Client>& client,
            const std::string &indexName, const std::string &docType = "_doc"):
            _client(client),
            _indexName(indexName),
            _docType(docType)
        {
            Json::Value settings;
            Json::Value analysis;
            Json::Value analyzer;
            Json::Value ik;
            ik["tokenizer"] = "ik_max_word";
            analyzer["ik"] = ik;
            analysis["analyzer"] = analyzer;
            settings["analysis"] = analysis;
            _index["settings"] = settings;
        }
        ESIndex& append(const std::string& key, 
            const std::string& type = "text", 
            const std::string& analyzer = "ik_max_word",
            bool enabled = true)
        {
            Json::Value tmp;
            tmp["type"] = type;
            tmp["analyzer"] = analyzer;
            if(!enabled) tmp["enabled"] = enabled;
            _properties[key] = tmp;
            return *this;
        }
        bool create(const std::string& id = "default_index_id", bool is_dynamic = true)
        {
            Json::Value mappings;
            mappings["dynamic"] = is_dynamic;
            mappings["properties"] = _properties;
            _index["mappings"] = mappings;
            std::string body;
            bool ret = Json_Util::serialize(_index, body);
            if(!ret)
            {
                LOG_ERROR("索引序列化失败！");
                return false;
            }
            // LOG_DEBUG("{}", body);
            try
            {
                std::cout << _indexName << " " << _docType << std::endl;
                auto rsp = _client->index(_indexName, _docType, id, body);
                if (rsp.status_code < 200 || rsp.status_code >= 300)
                {
                    LOG_ERROR("创建ES索引 {} 失败，响应状态码异常: {}", _indexName, rsp.status_code);
                    return false;
                }
            }
            catch(std::exception &e)
            {
                LOG_ERROR("创建ES索引 {} 失败: {}", _indexName, e.what());
                return false;
            }
            return true;
        }
    };
    class ESInsert
    {
        std::shared_ptr<elasticlient::Client> _client;
        std::string _indexName;
        std::string _docType;
        Json::Value _item;
    public:
        ESInsert(std::shared_ptr<elasticlient::Client> &client, 
            const std::string &indexName, const std::string &docType = "_doc"):
            _client(client),
            _indexName(indexName),
            _docType(docType) {}
        template<typename T>
        ESInsert& append(const std::string &key, const T &val)
        {
            _item[key] = val;
            return *this;
        }
        bool insert(const std::string id = "")
        {
            std::string body;
            bool ret = Json_Util::serialize(_item, body);
            if (ret == false)
            {
                LOG_ERROR("索引序列化失败！");
                return false;
            }
            LOG_DEBUG("{}", body);
            try
            {
                auto rsp = _client->index(_indexName, _docType, id, body);
                if (rsp.status_code < 200 || rsp.status_code >= 300)
                {
                    LOG_ERROR("新增数据 {} 失败，响应状态码异常: {}", body, rsp.status_code);
                    return false;
                }
            }
            catch(std::exception &e)
            {
                LOG_ERROR("新增数据 {} 失败: {}", body, e.what());
                return false;
            }
            return true;
        }
    };
    class ESRemove
    {
        std::shared_ptr<elasticlient::Client> _client;
        std::string _indexName;
        std::string _docType;
    public:
        ESRemove(std::shared_ptr<elasticlient::Client> &client, 
            const std::string &indexName, const std::string &docType = "_doc"):
            _client(client),
            _indexName(indexName),
            _docType(docType) {}
        bool remove(const std::string& id)
        {
            try
            {
                auto rsp = _client->remove(_indexName, _docType, id);
                if (rsp.status_code < 200 || rsp.status_code >= 300)
                {
                    LOG_ERROR("删除数据 {} 失败，响应状态码异常: {}", id, rsp.status_code);
                    return false;
                }
            }
            catch(std::exception &e)
            {
                LOG_ERROR("删除数据 {} 失败: {}", id, e.what());
                return false;
            }
            return true;
        }
    };
    class ESSearch
    {
        std::shared_ptr<elasticlient::Client> _client;
        std::string _indexName;
        std::string _docType;
        Json::Value _bool;
    public:
        ESSearch(std::shared_ptr<elasticlient::Client> &client, 
            const std::string &indexName, const std::string &docType = "_doc"):
            _client(client),
            _indexName(indexName),
            _docType(docType) {}
        ESSearch& append_must_not_terms(const std::string& key, const std::vector<std::string>& vals)
        {
            Json::Value terms;
            for(auto& val : vals)
                terms["key"].append(val);
            Json::Value tmp;
            tmp["terms"] = terms;
            _bool["must_not"].append(tmp);
            return *this;
        }
        ESSearch& append_should_match(const std::string& key, const std::string& val)
        {
            Json::Value match;
            match[key] = val;
            Json::Value tmp;
            tmp["match"] = match;
            _bool["should"].append(tmp);
            return *this;
        }
        ESSearch& append_must_terms(const std::string& key, const std::vector<std::string>& vals)
        {
            Json::Value terms;
            for(auto& val : vals)
                terms["key"].append(val);
            Json::Value tmp;
            tmp["terms"] = terms;
            _bool["must"].append(tmp);
            return *this;
        }
        ESSearch& append_must_match(const std::string& key, const std::string& val)
        {
            Json::Value match;
            match[key] = val;
            Json::Value tmp;
            tmp["match"] = match;
            _bool["must"].append(tmp);
            return *this;
        }
        Json::Value search()
        {
            Json::Value query;
            query["bool"] = _bool;
            Json::Value tmp;
            tmp["query"] = query;
            std::string body;
            bool ret = Json_Util::serialize(tmp, body);
            if(!ret)
            {
                LOG_ERROR("索引序列化失败！");
                return Json::Value();
            }
            LOG_DEBUG("{}", body);
            cpr::Response rsp;
            try
            {
                rsp = _client->search(_indexName, _docType, body);
                if (rsp.status_code < 200 || rsp.status_code >= 300)
                {
                    LOG_ERROR("检索数据 {} 失败，响应状态码异常: {}", body, rsp.status_code);
                    return Json::Value();
                }
            }
            catch(std::exception &e)
            {
                LOG_ERROR("检索数据 {} 失败: {}", body, e.what());
                return Json::Value();
            }
            std::cout << rsp.text << std::endl;
            Json::Value back;
            Json_Util::unserialize(rsp.text, back);
            return back;
        }
    };
}
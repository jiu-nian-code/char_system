#pragma once
#include <elasticlient/client.h>
#include <cpr/cpr.h>
#include <json/json.h>
#include <iostream>
#include <memory>
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
    
}
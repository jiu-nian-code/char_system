#include<elasticlient/client.h>
#include<cpr/cpr.h>
#include<iostream>
#include<exception>
#include"icsearch.hpp"
#include<gflags/gflags.h>
DEFINE_bool(mode, false, "mode - 运行模式： true-发布模式，日志通过文件输出； false-调试模式，日志通过屏幕输出。");
DEFINE_string(file, "log/default_log.txt", "若为发布模式，则指定日志文件，默认为：log/default_log.txt。");
DEFINE_int32(level, 1, "是否开启地址重用");//1是debug等级，查阅spdlog::level::level_enum可知

// int main()
// {
//     elasticlient::Client client({"http://127.0.0.1:9200/"});
//     try
//     {
//         auto rsp = client.search("user", "_doc", "{\"query\": { \"match_all\":{} }}");
//         std::cout << rsp.status_code << std::endl;
//         std::cout << rsp.text << std::endl;
//     }
//     catch(std::exception &e)
//     {
//         std::cout << "请求失败：" << e.what() << std::endl;
//         return -1;
//     }
//     return  0;
// }

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
    std::shared_ptr<elasticlient::Client> client(new elasticlient::Client({"http://127.0.0.1:9200/"}));
    thx::ESIndex eid(client, "prodect"); // 必须小写
    eid.append("价格");
    eid.append("商品名");
    eid.create();
    {
        thx::ESInsert eis(client, "prodect");
        eis.append("价格 ", 10);
        eis.append("商品名", "薯片");
        eis.insert("default_index_id3");
    }
    {
        thx::ESInsert eis(client, "prodect");
        eis.append("价格 ", 20);
        eis.append("商品名", "好丽友派");
        eis.insert("default_index_id4");
    }
    // thx::ESRemove erm(client, "prodect");
    // erm.remove("default_index_id");
    thx::ESSearch ess(client, "prodect");
    ess.append_should_match("商品名", "薯");
    return 0;
}
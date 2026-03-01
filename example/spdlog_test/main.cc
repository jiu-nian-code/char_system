#include<iostream>
#include"logger.hpp"
#include<gflags/gflags.h>
DEFINE_bool(mode, false, "mode - 运行模式： true-发布模式，日志通过文件输出； false-调试模式，日志通过屏幕输出。");
DEFINE_string(file, "log/default_log.txt", "若为发布模式，则指定日志文件，默认为：log/default_log.txt。");
DEFINE_int32(level, 1, "是否开启地址重用");//1是debug等级，查阅spdlog::level::level_enum可知

void sync()
{
    spdlog::flush_every(std::chrono::seconds(1));
    spdlog::flush_on(spdlog::level::level_enum::debug);
    spdlog::set_level(spdlog::level::level_enum::debug);
    auto logger = spdlog::stdout_color_mt("default-logger");
    logger->set_pattern("[%n][%H:%M:%S][%t][%-8l] %v");
    logger->trace("你好！{}", "小明");
    logger->debug("你好！{}", "小明");
    logger->info("你好！{}", "小明");
    logger->warn("你好！{}", "小明");
    logger->error("你好！{}", "小明");
    logger->critical("你好！{}", "小明");
}

void async()
{
    spdlog::flush_every(std::chrono::seconds(1));
    spdlog::flush_on(spdlog::level::level_enum::debug);
    spdlog::set_level(spdlog::level::level_enum::debug);
    spdlog::init_thread_pool(3072, 1);
    auto logger = spdlog::stdout_color_mt<spdlog::async_factory>("default-logger");
    logger->set_pattern("[%n][%H:%M:%S][%t][%-8l] %v");
    logger->trace("你好！{}", "小明");
    logger->debug("你好！{}", "小明");
    logger->info("你好！{}", "小明");
    logger->warn("你好！{}", "小明");
    logger->error("你好！{}", "小明");
    logger->critical("你好！{}", "小明");
}

int main(int argc, char* argv[])
{
    // sync();
    // async();
    google::ParseCommandLineFlags(&argc, &argv, true);
    init_logger(FLAGS_mode, FLAGS_file, FLAGS_level);
    LOG_TRACE("你好！{}", "小明");
    LOG_DEBUG("你好！{}", "小明");
    LOG_INFO("你好！{}", "小明");
    LOG_WARN("你好！{}", "小明");
    LOG_ERROR("你好！{}", "小明");
    LOG_FATAL("你好！{}", "小明");
    return 0;
}
#include<iostream>
#include<gflags/gflags.h>
DEFINE_string(ip, "127.0.0.1", "这是服务器的监听IP地址");
DEFINE_int32(port, 8080, "这是服务器的监听端口");
DEFINE_bool(address_reuse, true, "是否开启地址重用");

int main(int argc, char* argv[])
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    std::cout << FLAGS_ip << std::endl;
    std::cout << FLAGS_port << std::endl;
    std::cout << FLAGS_address_reuse << std::endl;
    return 0;
}
#include<brpc/channel.h>
#include<thread>
#include"main.pb.h"

void callback(brpc::Controller* controller_ptr, example::EchoResponse *response_ptr)
{
    std::unique_ptr<brpc::Controller> cntl_guard(controller_ptr);
    std::unique_ptr<example::EchoResponse> resp_guard(response_ptr);
    if (controller_ptr->Failed() == true) {
        std::cout << "Rpc调用失败：" << controller_ptr->ErrorText() << std::endl;
    }
    std::cout << "收到消息：" << response_ptr->message() << std::endl;
}

int main()
{
    brpc::ChannelOptions options;
    options.connect_timeout_ms = -1;
    options.timeout_ms = -1;
    options.max_retry = 3;
    options.protocol = "baidu_std";
    brpc::Channel channel;
    int ret = channel.Init("127.0.0.1:8080", &options);
    if (ret == -1) {
        std::cout << "初始化信道失败！\n";
        return -1;
    }
    example::EchoService_Stub stub(&channel);
    example::EchoRequest req;
    req.set_message("hello world");
    brpc::Controller* controller_ptr = new brpc::Controller;
    example::EchoResponse *response_ptr =  new example::EchoResponse;
    // stub.Echo(controller_ptr, &req, response_ptr, nullptr);
    // if (controller_ptr->Failed() == true) {
    //     std::cout << "Rpc调用失败：" << controller_ptr->ErrorText() << std::endl;
    //     return -1;
    // }
    // std::cout << "收到消息：" << response_ptr->message() << std::endl;
    // delete controller_ptr;
    // delete response_ptr;
    auto closure = google::protobuf::NewCallback(callback, controller_ptr, response_ptr);
    stub.Echo(controller_ptr, &req, response_ptr, closure);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}
#include <iostream>
#include "mprpcapplication.h"
#include "user.pb.h"

int main(int argc, char **argv)
{
    //整个程序启动以后，想使用mprpc框架来享受rpc服务调用，一定需要先调用框架的初始化函数（只初始化一次）
    Mprpcapplication::Init(argc, argv);

    //演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    // rpc方法的请求参数
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    // rpc方法的响应
    fixbug::LoginResponse response;
    //发起rpc方法的调用  同步的rpc调用过程  MprpcChannel::callMethod
    stub.Login(nullptr, &request, &response, nullptr);

    if (0 == response.result().errcode())
    {
        std::cout << "rpc login response success:" << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error:" << response.result().errcode() << std::endl;
    }
    
    fixbug::RegisterRequest req;
    req.set_id(100);
    req.set_name("li si");
    req.set_pwd("666666");

    fixbug::RegisterResponse res;
    stub.Register(0,&req,&res,0);
    if (0 == res.result().errcode())
    {
        std::cout << "rpc login response success:" << res.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error:" << res.result().errcode() << std::endl;
    }
    
    return 0;
}
#include "mprpcchannel.h"
#include <string>
#include <rpcheader.pb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include "mprpcapplication.h"
#include "mprpccontroller.h"
#include "zookeeperutil.h"

/*

*/
//所有通过stub代理对象调用的rpc方法，都走到这里了，统一做rpc方法调用的数据序列化和网络发送
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    //获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error!");
        return;
    }
    mprpc::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if (rpcheader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size();
    }
    else
    {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    //组织待发送的rpc请求字符串
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char *)&header_size, 4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    //使用tcp编程，完成rpc方法的远程调用
    int clientid = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientid)
    {
        char errtext[512] = {0};
        sprintf(errtext, "create socket error! errno:%d", errno);
        controller->SetFailed(errtext);
        return;
    }

    //读取配置文件rpcserver的信息
    // rpc调用方调用service_name的method_name服务，需要查询zk上该服务所在的host信息
    ZkClient zkCli;
    zkCli.Start();
    //  /UserServiceRpc/Login
    std::string method_path = '/' + service_name + '/' + method_name;
    // 127.0.0.1:8000
    std::string host_data = zkCli.GetData(method_path.c_str());
    if ("" == host_data)
    {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    int idx = host_data.find(':');
    if (-1 == idx)
    {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    //连接rpc服务节点
    if (-1 == connect(clientid, (struct sockaddr *)&server_addr, sizeof(server_addr)))
    {
        close(clientid);
        char errtext[512] = {0};
        sprintf(errtext, "connect error! errno:%d", errno);
        controller->SetFailed(errtext);
        return;
    }

    //发送rpc请求
    if (-1 == send(clientid, send_rpc_str.c_str(), send_rpc_str.size(), 0))
    {
        close(clientid);
        char errtext[512] = {0};
        sprintf(errtext, "send error! errno:%d", errno);
        controller->SetFailed(errtext);
        return;
    }

    //接收rpc请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientid, recv_buf, 1024, 0)))
    {
        close(clientid);
        char errtext[512] = {0};
        sprintf(errtext, "recv error! errno:%d", errno);
        controller->SetFailed(errtext);
        return;
    }

    //反序列化rpc调用的响应数据
    // std::string response_str(recv_buf, 0, recv_size);  //bug出现问题，recv_buf中遇到\0后面的数据就存不下来了
    // if (!response->ParseFromString(response_str))
    if (!response->ParseFromArray(recv_buf, recv_size))
    {
        close(clientid);
        char errtext[512] = {0};
        sprintf(errtext, "parse error! response_str:%s", recv_buf);
        controller->SetFailed(errtext);
        return;
    }

    close(clientid);
}
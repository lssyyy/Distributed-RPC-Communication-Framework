#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig Mprpcapplication::m_config;
void ShowArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}
void Mprpcapplication::Init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while ((c = getopt(argc, argv, ":i:")) != -1) /*getopt 命令行参数解析函数
                                                   optstring 1.选项后不带选项参数 i  ./provider -i 2.选项后带选项参数 i:  ./provider -i select
                                                   3.选项后带可选选项参数 i:: ./provider -i (select)
                                                   optstring 查询到命令行参数中出现未知参数时，返回?  没有参数时，返回-1 以":"开头 发现命令行缺少选项参数时，返回:*/
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //开始加载配置文件  rpcserver_ip=   rpcserver_port=   zookeeper_ip=  zookeeper_port=
    m_config.LoadConfigFile(config_file.c_str());
    
}

Mprpcapplication &Mprpcapplication::GetInstance()
{
    static Mprpcapplication app;
    return app;
}

MprpcConfig &Mprpcapplication::GetConfig()
{
    return m_config;
}
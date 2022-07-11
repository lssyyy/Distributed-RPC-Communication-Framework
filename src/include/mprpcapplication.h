#pragma once

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

// mprpc框架的基础类
class Mprpcapplication
{
public:
    static void Init(int argc, char **argv);
    static Mprpcapplication &GetInstance();
    static MprpcConfig &GetConfig();

private:
    static MprpcConfig m_config;
    Mprpcapplication() {}
    Mprpcapplication(const Mprpcapplication &) = delete;
    Mprpcapplication(Mprpcapplication &&) = delete;
};
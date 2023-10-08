#pragma once

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <memory>


class PipeIPC {
public:
	enum PIPEUSERTYPE {
		CLIENT, //客户端
		SERVER, //服务端
	};

public:
	explicit PipeIPC(PIPEUSERTYPE role, std::string name) :m_Role(role), m_strName(name), m_hPipeHandle(INVALID_HANDLE_VALUE) {};
    ~PipeIPC();

    bool WriteData(__in const std::string& datas); //写入数据

    bool ReadData(__out std::string& datas, __out bool& bIsPipeEnd); //读取数据

    void DisConnect();

    bool InitPipeIPC(); //初始化管道通信

    bool IsInit();

    BOOL Connect();

    void SetPipeName(__in const std::string& name);

private:
    void release();

    BOOL initSecurityAttributes();

private:
    std::string m_strName;
    PIPEUSERTYPE m_Role;
    HANDLE m_hPipeHandle = INVALID_HANDLE_VALUE;
    SECURITY_ATTRIBUTES m_sec_attr;  //security attributes
};

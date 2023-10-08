#pragma once

#include <stdio.h>
#include <tchar.h>

#include <iostream>
#include <Windows.h>
#include <string>

#include <memory>

enum PIPEUSERTYPE {
	USER_CLIENT, //客户端
	USER_SERVER, //服务端
};

class PipeIPC {
public:
	PipeIPC();
	~PipeIPC();

	bool InitPipeIPC(std::string pipe_name, PIPEUSERTYPE states); //初始化管道通信

	//读写数据
	bool WriteData(std::string datas); //写入数据
	bool ReadData(std::string& datas, bool& isPipeEnd); //读取数据

private:
	std::string m_pipe_name;
	HANDLE m_hPipeHandle;
	PIPEUSERTYPE m_state; //状态设置
};


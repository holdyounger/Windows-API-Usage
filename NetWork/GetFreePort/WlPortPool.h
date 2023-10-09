/**************************************************************************
 *
 *        Copyright (C) 2022  Beijing Winicssec Technologies Co.,Ltd
 *        All rights reserved
 *
 *        Filename    :	WLPortPool.h
 *        Description : 动态端口已占用完毕的情况下，获取动态端口的函数接口
 *
 *        Created by  mingming.shi on Jan. 24th, 2022
 *
 *************************************************************************/
#pragma once

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
#include <sstream>
#include <Windows.h>
#include <WinSock.h>
#include <tcpmib.h>
#include <IPHlpApi.h>
#include <set>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "IPHlpApi.lib")

#define MAX_PORT_NUM		10		// 最大只允许存在10个端口池
#define DEFAULT_START_PORT	1025	// 默认启动端口：1025
#define DEFAULT_STOP_PORT	65535	// 默认终止端口：65535
#define DEFAULT_PORT_RANGE	100		// 默认端口范围：100
#define DEFAULT_MIN_PORTRANGE 50	// 最小端口范围
#define START_STATIC_PORT_FLAG 100	// 动态端口个数：启动端口池的标识

/* 端口池结构体 */
typedef struct _PORT_POOL {
	UINT nStartPort;	// 启动端口
	UINT nRange;		// 端口范围
	bool bEnable;

	_PORT_POOL(UINT nSta = 0, UINT nRan = 0, bool bEn = false):nStartPort(nSta),nRange(nRan),bEnable(bEn){};
	bool isContain(UINT uPORT)
	{
		if ( uPORT >= nStartPort && uPORT < (nStartPort + nRange) )
		{
			return true;
		}
		return false;
	}
	
	bool isLegal()
	{
		if( (this->nStartPort + this->nRange) <= DEFAULT_STOP_PORT &&
			(this->nStartPort) >= DEFAULT_START_PORT
			)
		{
			return true;
		}
		return false;
	}

	bool operator<(const _PORT_POOL& b) const
	{
		return this->nStartPort == b.nStartPort ? this->nRange <= b.nRange : this->nStartPort < b.nRange;
	}
}STU_PORT_POOL, *PSTU_PORT_POOL;

/* 全局变量-端口数组 */
extern std::vector<STU_PORT_POOL> g_stuPortPool;

#ifdef __cplusplus
extern "C" {
#endif
	namespace WNTPORT {
	
		extern bool g_bFlagGetStaticPort;	// 为真表示启用端口池
		extern bool g_bStartThread;			// 为假表示停止线程
		extern int g_nCurPoint;				// 端口池轮询
		
		/* 设置当前端口状态 */
		void WINAPI SetPortIsAvailable(__in STU_PORT_POOL stuPort);

		/* 端口池线程 */
		void startScanPortsThread();

		/* 杀死线程 */
		void killScanPortsThread();

		/* 返回一个可用端口和其范围 */
		void WINAPI GetFreePort(__out STU_PORT_POOL& stuPort);

		/* 返回端口池状态 */
		bool WINAPI getPortPoolStatus();
	}
#ifdef __cplusplus
}
#endif
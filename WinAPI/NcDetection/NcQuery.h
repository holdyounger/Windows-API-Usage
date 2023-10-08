/********************************************************************************
 *
 *        Copyright (C) 2022 QIANXIN
 *        All rights reserved
 *
 *        filename:    clientapp\windows\nctunnel\NcDetection\NcQuery.h
 *        description: NC 异常检测与修复
 *						1. NcQuery：维护nc状态
 *						2. NcRecoverEvent: 状态执行类，根据错误码调用相应的函数，做中转
 *						3. NcMoitor: Nc 修复类，根据保存的状态和回调事件执行修复
 *		  wiki: https://wiki.qianxin-inc.cn/pages/viewpage.action?pageId=710796520
 *        created by  shi.mingming on 2023/04/07
 *
 ********************************************************************************/

#if SP_PLATFORM==SP_PLATFORM_WINDOWS
#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string>
#include <vector>
#include <functional>
#include <unordered_set>

#include "json11\json11.hpp"
#include "ModuleStatus.h"



using namespace json11;

typedef std::unordered_set<std::string> RouteTable;
typedef std::map<std::string, std::string> MAPDBSTR;
typedef std::function<void(std::string, int status, std::string)> RecoveryCallback;


extern bool HandleEventRouteAdd(RouteTable table);
/*
* @fn           IsExistNcCard
* @brief
* @return       return ture if Exist
*
* @detail
*/
static bool IsExistUsingNcCard();

/*
* @fn           HandleThreadStart
* @brief		启动 Nc 修复线程（网卡异常与路由异常）
*/
void HandleThreadStart();

/*
* @fn           HandleThreadStop
* @brief		停止 Nc 修复线程
*/
void HandleThreadStop();

// 返回的Json每一项的字段
#define MSG_TYPE_STATUS		"status"
#define MSG_TYPE_MSG		"message"
#define MSG_TYPE_TIME		"time"
#define MSG_TYPE_LEVEL		"level"
#define MSG_TYPE_CONTENT	"content"
#define MSG_TYPE_OPTIONS	"options"
#define MSG_TYPE_ROUTETABLE	"routetable"

// 返回的Json项
const std::vector<std::string> NcModuleList{ "netcard", "ncopt", "ncip", "route" };
#define IDX_NETCARD 0
#define IDX_NCOPT	1
#define IDX_NCIP	2
#define IDX_ROUTE	3


enum EN_NC_ERROR_MOUDLE
{
	NC_EM_NETCARD = 0,	// 虚拟网卡
	NC_EM_NCIP,			// ncip
	NC_EM_NCOPT,		// nc ipc&opt
	NC_EM_ROUTE,		// 路由
	NC_EM_EVENT,		// 修复事件
};

enum EN_NC_NETCARTD_ERROR_TYPE {
	NETCARD_STATUS_UNNSTALL,	// 未安装
	NETCARD_STATUS_DIABLE,		// 禁用
	NETECARD_STATUS_NOTRUN,		// 未启用
	NETECARD_STATUS_RUNNNIG		// 运行中
};

enum EN_NC_IP_ERROR_TYPE {

};
enum EN_NC_ROUTE_ERROR_TYPE {

};

enum EN_LEVEL_TYPE
{
	 LEVEL_TYPE_INFO,
	 LEVEL_TYPE_WARN,
	 LEVEL_TYPE_ERROR,
	 LEVEL_TYPE_ALARM,
	 LEVEL_TYPE_FATAL
};

class NcQuery
{
public:
	NcQuery();
	~NcQuery();

	void Init();

	void Clear();
public:

	static NcQuery* GetInstance();

	void SetNcIPStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/);

	void SetNcOptStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/);

	void SetNetCardStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/);

	void SetEventRelt(std::string strModule, std::string status, std::string message /*= ""*/);
	/*
	* @fn           SetNetCardOptStatus
	* @brief        设置网卡信息
	* @param[in]    
	*	iType		0: 更新网卡信息；1：更新网卡索引
	*	options		要更新的信息
	*               
	* @detail      
	*/
	void SetNetCardOptStatus(int iType, std::string options /*= ""*/);

	/*
	* @fn           SetRouteStatus
	* @brief        
	* @param[in]    
	*               
	* @detail      
	*/
	void SetRouteStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/, std::string route /*= ""*/);

	std::string GetNCStatus();

	void RegisterRecoverEvent(RecoveryCallback handle);

private:
	void updateStatus(std::string moudle, std::string msgType, std::string status);

	void updateMasterStatus(std::string module, int code, std::string state, std::string msg);

	std::string GetChdModuleName(EN_NC_ERROR_MOUDLE module);

private:

	std::map<std::string, MAPDBSTR> m_mapStatus;

	std::string m_state = "";
	std::string m_Msg = "";
	int m_nErrCode = 0;

	RecoveryCallback m_recoverHandle = nullptr;
};


namespace NCQ {
	NcQuery* Get();

	void SetNcIPStatus(ModuleThreadState threadState, std::string status, std::string message = "");
	void SetNcOptStatus(ModuleThreadState threadState, std::string status, std::string message = "");
	void SetNetCardStatus(ModuleThreadState threadState, std::string status, std::string message = "");
	void SetNetCardOptStatus(int iType, std::string options = "");
	void SetRouteStatus(ModuleThreadState threadState, std::string status, std::string message = "", std::string route = "");
	void SetEventRelt(std::string strModule, std::string status, std::string message /*= ""*/);
}

class NcRecoveryEvent
{

public:
	static NcRecoveryEvent* Get();

	/*
	* @fn           RecoverEvent
	* @brief        修复函数，根据传入的status决定修复的事件和操作
	* @param[in]    
	*		strModule	传入的模块
	*		status		传入的异常代码
	*		msg			更新的内容
	*               
	* @detail      
	*/
	void RecoverEvent(std::string strModule, int status, std::string msg = "");

private:
	static NcRecoveryEvent* instanceRecv;
};

struct CARD_INFO
{
	std::string ip;
	std::string mask;
	std::string vpn_host;
	std::string vpn_ip;
	std::string vpn_ip4;
	std::string vpn_port;

	std::vector<std::string> dns4;
	std::vector<std::string> wins;
};

enum EN_NC_STATUS
{
	NC_DEFAULT = 0,
	NC_RUNNING,
	NC_STOP
};


class NcMonitor
{
public:
	static NcMonitor* GetInstance();

	/*
	* @fn           Start
	* @brief        启动监控线程
	* @detail		nc 启动时启动
	*/
	void Start();

	/*
	* @fn           Stop
	* @brief        停止监控线程
	* @detail		nc 停止时停止
	*/
	void Stop();

	/*
	* @fn           UpdateRouteTable
	* @brief        更新路由
	* @param[in]
	*	strRouteTable	路由表的json串
	*
	* @detail
	*/
	void UpdateRouteTable(const std::string& strRouteTable);

	/*
	* @fn           UpdateNetCardOptions
	* @brief        更新网卡配置
	* @param[in]
	*	info	保存网卡信息的结构体
	*
	* @detail
	*/
	void UpdateNetCardOptions(const std::string& infoJson);

	/*
	* @fn           Clear
	* @brief        清空路由和配置信息，更新状态为default
	*
	* @detail
	*/
	void Clear();

	/*
	* @fn           UpdateNetCardIndex
	* @brief        更新网卡索引
	* @param[in]
	*	index		网卡索引
	*/
	void UpdateNetCardIndex(int index);
	int GetNetCardIndex() { return m_IfIndex; }

protected:

private:
	void ThreadCheck();

	/*
	* @fn           checkRoute
	* @brief        检查系统路由是否包含了所有已下发路由
	* @return		return true if sys Route Table contained All TrustAgent Route
	*
	* @detail
	*/
	bool checkRoute(RouteTable& route);

	/*
	* @fn           checkCardOptions
	* @brief        检查TrustAgent VNIC网卡的配置是否正确
	* @return       return true if netcard Config is Corrected
	*
	* @detail      TODO
	*/
	bool checkCardOptions();

private:
	bool m_StopFlag = false;

private:
	EN_NC_STATUS m_Status = NC_DEFAULT; // 保存当前nc的状态

	int m_IfIndex = -1;

	CARD_INFO m_cardinfo = {};	// 保存当前网卡的配置信息

	RouteTable m_Route = {};	// 路由表

	bool m_bRouteCheckSwitch = false;

	bool m_bNetCardCheckSwitch = false;
};

void HandleThreadStart();

void HandleThreadStop();


bool HandleNetCardError(int errorCode);

bool HandleUpdateNetCardOptions(int status, std::string strOptions);

bool HandleNcipError(int errorCode);

bool HandleNcoptError(int errorCode);

bool HandleRouteError(int errorCode);

bool HandleUpdateRouteTable(std::string errorCode);

#endif
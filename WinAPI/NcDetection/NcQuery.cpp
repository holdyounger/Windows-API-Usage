#if SP_PLATFORM==SP_PLATFORM_WINDOWS

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <time.h>
#include "inc/winlog.h"
#include "NcQuery.h"
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <netlistmgr.h>
#include <netcon.h>
#include <windows.h>
#include <Shlwapi.h>
#include <WinSock2.h>
#include <bitset>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#include "../../secureportal/spcore/trust/sp_trust_model.h"
#include "../../windows/utils/inc/WinReg.h"

#include "TrustServiceStatusManager.h"

static const char* _adapter_key =
"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002BE10318}";

static const char* _component_id = "vpnvnic";

extern std::unique_ptr<SPTrustModel> g_sp_trust_model;
typedef std::map<NETCON_STATUS, std::wstring> NcStatusList;
typedef std::unordered_set<std::string> TapReg;

std::mutex g_mutex_mointor_thread;

// 函数声明
static TapReg get_tap_reg();

std::string ipToString(DWORD dwIP)
{
	std::string strDestIp = "";
	std::string strMaskIp = "";

	struct in_addr network;
	network.S_un.S_addr = dwIP;    //为s_addr赋值--网络字节序
	strDestIp = inet_ntoa(network);

	return strDestIp;
}

uint32_t stringToIP(std::string strIP)
{
	int nMaskNum = 0;
	DWORD ip = inet_addr(strIP.c_str());

	if (ip == INADDR_NONE)
	{
		return -1;
	}

	return ip;
}

static std::string GUIDToString(const GUID& guid)
{
	OLECHAR guidString[40] = { 0 };
	::StringFromGUID2(guid, guidString, sizeof(guidString));

	std::wstring wsGuid = guidString;
	std::string str(wsGuid.begin(), wsGuid.end());

	return str;
}

/*
* @fn           获取 VNIC上配置的路由
* @brief        GetSysVnicRouteTable
* @param[out]
*		route	路由表
* @return       bool 获取成功返回 true, 否则返回 false
*
* @detail
*/
static bool GetSysRouteTable(RouteTable& route);

static int maskToDigit(std::string strIp);

static std::string GetSystemTime();

static std::string EnumLevelToString(EN_LEVEL_TYPE level);
static std::string EnumTdStateToString(ModuleThreadState threadState);

static NcQuery* instanceQuery = nullptr;

NcQuery::NcQuery()
{
	Init();
}

NcQuery::~NcQuery()
{

}


void NcQuery::Init()
{
	TrustServiceStatusManager::instance()->InitService("trustnc", 2, true);

	MAPDBSTR temp;
	temp[MSG_TYPE_STATUS] = "0000";
	temp[MSG_TYPE_MSG] = "Init";

	int strListsize = NcModuleList.size();
	for (int i = 0; i < strListsize; i++)
	{
		m_mapStatus[NcModuleList[i]] = temp;
		TrustServiceStatusManager::instance()->AddModuleStatus(i, NcModuleList[i], thread_wait);
	}

	updateMasterStatus("", 0, "", "wait");
}

void NcQuery::Clear()
{
	// 清空信息
	m_mapStatus.clear();

	m_Msg.clear();
	m_state.clear();

	Init();
}

NcQuery* NcQuery::GetInstance()
{
	if (instanceQuery == nullptr)
	{
		instanceQuery = new NcQuery;
	}

	return instanceQuery;
}

void NcQuery::updateStatus(std::string strModule, std::string msgType, std::string status)
{
	MAPDBSTR mapModuleStatus;

	mapModuleStatus = m_mapStatus[strModule];
	mapModuleStatus[msgType] = status;
	mapModuleStatus["time"] = GetSystemTime();
	m_mapStatus[strModule] = mapModuleStatus;

	LOGI("%s [MOD]:%s,[TYPE]:%s,[STATUS]:%s", __FUNCTION__, strModule.c_str(), msgType.c_str(), status.c_str());
}

std::string NcQuery::GetChdModuleName(EN_NC_ERROR_MOUDLE module)
{
	std::string strModule = "";
	switch (module)
	{
	case NC_EM_NETCARD:
		strModule = "netcard";
		break;
	case NC_EM_NCIP:
		strModule = "ncip";
		break;
	case NC_EM_NCOPT:
		strModule = "ncopt";
		break;
	case NC_EM_ROUTE:
		strModule = "route";
		break;
	case NC_EM_EVENT:
		strModule = "repair";
		break;
	default:
		break;
	}

	return strModule;
}

void NcQuery::updateMasterStatus(std::string module, int code, std::string state, std::string msg)
{
	m_nErrCode = code;
	m_state = code == 0 ? "R" : code == 1 ? "S" : "E";
	m_Msg = msg;

	// code说明 https://wiki.qianxin-inc.cn/pages/viewpage.action?pageId=710796520#id-%E3%80%90Windows%E3%80%91NC%E5%BC%82%E5%B8%B8%E6%A3%80%E6%B5%8B-3Code%E8%AF%B4%E6%98%8E
	
	// if (code != 0) // 非0表示有错误
	{
		SPJSONObject jerr{
			{"method", "update_nc_error"},
			{"code", code},
			{"message", msg.c_str()},
			{"module", module.c_str()}
		};


		std::string msg = SPJSON(jerr).dump();
		static TrustSpaceIPCUtil ncIpc("trustNcToCore");
		ncIpc.sendIPCEventToChannel(msg.c_str(), msg.size());
	}
}

void NcQuery::SetNcIPStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/)
{
	std::string strModule = GetChdModuleName(NC_EM_NCIP);
	updateStatus(strModule, MSG_TYPE_STATUS, status);
	updateStatus(strModule, MSG_TYPE_MSG, message);

	TrustServiceStatusManager::instance()->UpdateModuleStatus(strModule, threadState, status, message);

	updateMasterStatus(strModule, std::stoi(status), "", message);

#if 1
	if (m_recoverHandle)
	{
		m_recoverHandle("ncip", std::stoi(status), "");
	}
#endif
}

void NcQuery::SetNcOptStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/)
{
	std::string strModule = GetChdModuleName(NC_EM_NCOPT);
	// updateStatus(strModule, MSG_TYPE_LEVEL, EnumLevelToString(level));
	updateStatus(strModule, MSG_TYPE_STATUS, status);
	updateStatus(strModule, MSG_TYPE_MSG, message);

	updateMasterStatus(strModule, std::stoi(status), "", message);

	TrustServiceStatusManager::instance()->UpdateModuleStatus(strModule, threadState, status, message);

#if 1
	if (m_recoverHandle)
	{
		m_recoverHandle("ncip", std::stoi(status), "");
	}
#endif
}

void NcQuery::SetNetCardStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/)
{
	std::string strModule = GetChdModuleName(NC_EM_NCIP);

	updateStatus(strModule, MSG_TYPE_STATUS, status);
	updateStatus(strModule, MSG_TYPE_MSG, message);

	updateMasterStatus(strModule, std::stoi(status), "", message);

	TrustServiceStatusManager::instance()->UpdateModuleStatus(strModule, threadState, status, message);


#if 1
	if (m_recoverHandle)
	{
		m_recoverHandle("netcard", std::stoi(status), "");
	}
#endif
}


void NcQuery::SetEventRelt(std::string module, std::string status, std::string message /*= ""*/)
{
	if (std::stoi(status) != 0)
	{
		std::string strModule = GetChdModuleName(NC_EM_EVENT);
		updateStatus(strModule, "error_module", module);
		updateStatus(strModule, MSG_TYPE_STATUS, status);
		updateStatus(strModule, MSG_TYPE_MSG, message);
	
		updateMasterStatus(strModule, std::stoi(status), "", message);

		TrustServiceStatusManager::instance()->AddModuleStatus(5, strModule, thread_error);
		TrustServiceStatusManager::instance()->UpdateModuleStatus(strModule, thread_error, status, message);
	}
}

void NcQuery::SetNetCardOptStatus(int iType, std::string options /*= ""*/)
{
	std::string strModule = GetChdModuleName(NC_EM_NETCARD);
	updateStatus(strModule, MSG_TYPE_OPTIONS, options);
#if 1
	if (m_recoverHandle)
	{
		m_recoverHandle("netcardopt", iType, options);
	}
#endif
}

void NcQuery::SetRouteStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/, std::string route /*= ""*/)
{
	std::string strModule = GetChdModuleName(NC_EM_ROUTE);
	updateStatus(strModule, MSG_TYPE_STATUS, status);
	updateStatus(strModule, MSG_TYPE_MSG, message);
	updateStatus(strModule, MSG_TYPE_ROUTETABLE, route);

	updateMasterStatus(strModule, std::stoi(status), "", message);

#if 1
	if (m_recoverHandle)
	{
		if (route.empty())
		{
			m_recoverHandle("route", std::stoi(status), message);
		}
		else
		{
			m_recoverHandle("routetable", std::stoi(status), route);
		}
	}
#endif
}

std::string NcQuery::GetNCStatus()
{
#if 0
	if (NcRecoveryEvent::GetInstance()->HasRecyErrorEvent())
	{
		Json JsonRecv = NcRecoveryEvent::GetInstance()->to_Json();
		Json JsonStatus = Json(m_mapStatus);

		Json::object objStatus = Json::object{JsonStatus.object_items()};
		objStatus["recv"] = JsonRecv;


		return Json(objStatus).dump();
	}
	else
	{
		return Json(m_mapStatus).dump();
	}
#endif

	Json JsonModules = Json(m_mapStatus);

	Json::array arr = Json::array();
	for (auto& it : m_mapStatus)
	{
		Json modCont = Json(it.second);
		Json::object objmodCont = Json::object{ modCont.object_items() };
		objmodCont["name"] = it.first;

		arr.push_back(objmodCont);
	}

	Json JsonRet = Json::object({
		{"service", "trustnc"},
		{"os", "windows"},
		{"time", GetSystemTime()},
		{"state", m_state},
		{"message", m_Msg},
		{"error_code", m_nErrCode},
		{"modules", arr}
		});

	return JsonRet.dump();
}

void NcQuery::RegisterRecoverEvent(RecoveryCallback handle)
{
	m_recoverHandle = handle;
}

namespace NCQ {
	NcQuery* Get()
	{
		return NcQuery::GetInstance();
	}

	void SetNcIPStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/)
	{
		Get()->SetNcIPStatus(threadState, status, message);
	}

	void SetNcOptStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/)
	{
		Get()->SetNcOptStatus(threadState, status, message);
	}

	void SetNetCardStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/)
	{
		Get()->SetNetCardStatus(threadState, status, message);
	}

	void SetNetCardOptStatus(int iType, std::string options /*= ""*/)
	{
		Get()->SetNetCardOptStatus(iType, options);
	}

	void SetRouteStatus(ModuleThreadState threadState, std::string status, std::string message /*= ""*/, std::string route /*= ""*/)
	{
		Get()->SetRouteStatus(threadState, status, message, route);
	}

	void SetEventRelt(std::string strModule, std::string status, std::string message /*= ""*/)
	{
		Get()->SetEventRelt(strModule, status, message);
	}
}

static NcRecoveryEvent* intanceRcEvent;

NcRecoveryEvent* NcRecoveryEvent::Get()
{
	if (intanceRcEvent == nullptr)
	{
		intanceRcEvent = new NcRecoveryEvent;
	}
	

	return intanceRcEvent;
}

void NcRecoveryEvent::RecoverEvent(std::string strModule, int status, std::string msg)
{
	SP_LOGI("module:%s, status[%d], message:%s", strModule.c_str(), status, msg.c_str());

	bool bRepair = false;

	// 调用相应的修复模块
	/* 修复事件在 vnic 构造后由 vnic 接管， 主要逻辑不变，切换的原因是
	* 由于直接使用 vinc 操作网卡更方便，接管函数为 VNIC::RecoverEvent
	* 当前注册函数处理主要是更新网卡信息到 NcRepair
	*/
#if 1
	if (strModule.compare("netcard") == 0)
	{
		bRepair = HandleNetCardError(status);
	}
	else if (strModule.compare("netcardopt") == 0)
	{
		// 更新网卡配置
		bRepair = HandleUpdateNetCardOptions(status, msg);
	}
	else if (strModule.compare("ncip") == 0)
	{
		bRepair = HandleNcipError(status);
	}
	else if (strModule.compare("ncopt") == 0)
	{
		bRepair = HandleNcoptError(status);
	}
	else if (strModule.compare("route") == 0)
	{
		bRepair = HandleRouteError(status);
	}
	else if (strModule.compare("routetable") == 0)
	{
		// 更新路由表
		bRepair = HandleUpdateRouteTable(msg);
	}
#endif
}

static NcMonitor* instanceMon = nullptr;

NcMonitor* NcMonitor::GetInstance()
{
	if (instanceMon == nullptr)
	{
		instanceMon = new NcMonitor;
	}

	return instanceMon;
}

void NcMonitor::Start()
{
	SP_LOGI("[NCREPAIR] Thread Start");

	Clear();

	std::thread RepairThread(&NcMonitor::ThreadCheck, NcMonitor::GetInstance());
	RepairThread.detach();
}

void NcMonitor::Stop()
{
	SP_LOGI("[NCREPAIR] Thread Stop");

	m_StopFlag = true;

	Clear();
}

void NcMonitor::ThreadCheck()
{
	std::lock_guard<std::mutex> lock(g_mutex_mointor_thread);



	do
	{
		//#1 判断网卡是否安装
		// 注，查看网卡的逻辑已在 trustcore 中实现 查看 keepDeviceTAVnicAlive 函数

		//#2 检查网卡配置是否正确
		if (m_bNetCardCheckSwitch)
		{
			if (!checkCardOptions())
			{
				LOGE("[NCREPAIR] Check netcard Failed");

				return;
			}
		}

		//#3 判断路由是否正确
		if (m_bRouteCheckSwitch)
		{
			RouteTable table;
			if (!checkRoute(table))
			{
				LOGE("[NCREPAIR] Check Route Failed, Repairing");

				// 先判断是否有启用网卡，如果没有，则等待30s后再重新检测
				if (!IsExistUsingNcCard())
				{
					LOGE("[NCREPAIR] Check NcCard Failed, Sleep 30s");
					Sleep(30 * 1000);
					continue;
				}

				bool bRet = HandleEventRouteAdd(table);

				LOGE("[NCREPAIR] Repairing Result %d", bRet);

			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 10));

	} while (!m_StopFlag);
}


bool NcMonitor::checkRoute(RouteTable& route)
{
	// 路由表未更新情况下直接返回true
	if (m_Route.empty())
	{
		return true;
	}

	bool bRet = false;
	RouteTable sysRoute = {};

	//#1 获取系统路由
	bRet = GetSysRouteTable(sysRoute);

	if (!bRet)
		SP_LOGI("%s Found Route Result:%d, GetLastError()=%d", __FUNCTION__, bRet, GetLastError());

	//#2 检查且修复
	// 保证检查出的路由必须包含已下发的路由信息，否则重新走添加路由信息
	for (auto& it : m_Route)
	{
		if (sysRoute.end() == sysRoute.find(it))
		{
			// 系统路由中未找到下发路由
			bRet = false;
			route.insert(it);
		}
	}

#if 0
	if (route.size() == m_Route.size())
	{
		bRet = true;
	}
#endif

	return bRet;
}

bool NcMonitor::checkCardOptions()
{
	return true;
}

void NcMonitor::UpdateRouteTable(const std::string& strRouteTable)
{
	std::string err;
	Json jsonRoute = Json::parse(strRouteTable, err);
	Json::array arr = jsonRoute.array_items();
	for (auto& it : arr)
	{
		m_Route.insert(it.string_value());
	}

	m_bRouteCheckSwitch = true;
}

void NcMonitor::UpdateNetCardOptions(const std::string& infoJson)
{
	SP_LOGI("%s Update Card Options", __FUNCTION__);

	CARD_INFO info = {};

	std::string err;
	Json JsonOptions = Json::parse(infoJson, err);
	Json::object Obj = JsonOptions.object_items();
	Json::object dataObj = Obj["data"].object_items();

	info.ip = dataObj["ip"].string_value();
	info.mask = dataObj["mask"].string_value();
	info.vpn_host = dataObj["vpn_host"].string_value();
	info.vpn_ip = dataObj["vpn_ip"].string_value();
	info.vpn_ip4 = dataObj["vpn_ip4"].string_value();
	info.vpn_port = dataObj["vpn_port"].string_value();

	// 添加虚拟ip路由
	int iMaskIp = maskToDigit(info.mask);
	std::string strVnipRoute = info.ip.append("/").append(std::to_string(iMaskIp));
	SP_LOGI("%s Update Card Route %s", __FUNCTION__, strVnipRoute.c_str());
	m_Route.insert(strVnipRoute);


	Json::array dns_array = dataObj["dns"].array_items();
	int dns_num = dns_array.size();

	for (auto& dns : dns_array)
		info.dns4.push_back(dns.string_value());

	Json::array wins_array = dataObj["wins"].array_items();
	int wins_num = wins_array.size();

	for (auto& wins : wins_array)
		info.wins.push_back(wins.string_value());

	m_cardinfo = info;

	m_bNetCardCheckSwitch = true;
}

void NcMonitor::Clear()
{
	m_StopFlag = false;
	m_bNetCardCheckSwitch = false;
	m_bRouteCheckSwitch = false;

	m_Status = NC_DEFAULT;
	m_Route.clear();
	m_cardinfo = {};
}

void NcMonitor::UpdateNetCardIndex(int index)
{
	m_IfIndex = index;
}

void HandleThreadStart()
{
	NcMonitor::GetInstance()->Start();
}

void HandleThreadStop()
{
	NcMonitor::GetInstance()->Stop();
}

bool HandleNetCardError(int errorCode)
{
	SP_LOGI("%s [NCREPAIR] error Code:%d", __FUNCTION__, errorCode);

	return false;
}

bool HandleUpdateNetCardOptions(int status, std::string strOptions)
{
	SP_LOGI("%s [NCREPAIR] error Code:%d", __FUNCTION__, status);

	if (status == 0) // 更新网卡配置
	{
		NcMonitor::GetInstance()->UpdateNetCardOptions(strOptions);
	}
	else if (status == 1) // 更新网卡索引
	{
		// NcRepair::GetInstance()->UpdateNetCardIndex(std::stoi(strOptions));
	}

	return false;
}

bool HandleNcipError(int errorCode)
{
	SP_LOGI("%s [NCREPAIR] error Code:%d", __FUNCTION__, errorCode);
	return false;
}

bool HandleNcoptError(int errorCode)
{
	SP_LOGI("%s [NCREPAIR] error Code:%d", __FUNCTION__, errorCode);

	return false;
}

bool HandleRouteError(int errorCode)
{
	SP_LOGI("%s [NCREPAIR] error Code:%d", __FUNCTION__, errorCode);

	// 更新检测路由
	if (errorCode == 0)
	{
		return false;
	}
	return false;
}

bool HandleUpdateRouteTable(std::string strRouteTable)
{
	NcMonitor::GetInstance()->UpdateRouteTable(strRouteTable);
	return true;
}

bool GetSysRouteTable(RouteTable& route)
{
	bool bRet = true;
	PMIB_IPFORWARDTABLE pIpForwardTable;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;

	// char szDestIp[128];
	// char szMaskIp[128];
	// char szGatewayIp[128];

	std::string strDestIp = "";
	std::string strMaskIp = "";

	struct in_addr IpAddr;

	pIpForwardTable =
		(MIB_IPFORWARDTABLE*)malloc(sizeof(MIB_IPFORWARDTABLE));
	if (pIpForwardTable == NULL) {
		printf("Error allocating memory\n");
		return 1;
	}

	if (GetIpForwardTable(pIpForwardTable, &dwSize, 0) ==
		ERROR_INSUFFICIENT_BUFFER) {
		free(pIpForwardTable);
		pIpForwardTable = (MIB_IPFORWARDTABLE*)malloc(dwSize);
		if (pIpForwardTable == NULL) {
			printf("Error allocating memory\n");
			return 1;
		}
	}

	// 获取系统所有的路由
	if ((dwRetVal = GetIpForwardTable(pIpForwardTable, &dwSize, 0)) == NO_ERROR)
	{
		for (int idx = 0; idx < (int)pIpForwardTable->dwNumEntries; idx++)
		{
			// 获取系统所有的路由
			// if (pIpForwardTable->table[idx].dwForwardIfIndex == NcRepair::GetInstance()->GetNetCardIndex())
			{

				strDestIp = ipToString(pIpForwardTable->table[idx].dwForwardDest);
				strMaskIp = ipToString(pIpForwardTable->table[idx].dwForwardMask);

				int iMaskIp = maskToDigit(strMaskIp);

				// 按下发路由格式写入set
				std::string strRoute = "";
				strRoute = strRoute.append(strDestIp).append("/").append(std::to_string(iMaskIp));

				route.insert(strRoute);
			}
		}

		bRet = true;
	}

	if (pIpForwardTable)
	{
		free(pIpForwardTable);
	}

	return bRet;
}


int maskToDigit(std::string strIP)
{
	DWORD ip = stringToIP(strIP);

	int nMaskNum = 0;
	std::bitset<32> bitBinary = std::bitset<32>(ip);

	return bitBinary.count();

#if 0
	while (ip)
	{
		if (ip & 1)
		{
			nMaskNum++;
		}

		ip >>= 1;
	}

	return nMaskNum;
#endif
}

std::string EnumLevelToString(EN_LEVEL_TYPE level)
{
	switch (level)
	{
	case LEVEL_TYPE_INFO:
		return std::string("INFO");
		break;
	case LEVEL_TYPE_WARN:
		return std::string("WARN");
		break;
	case LEVEL_TYPE_ERROR:
		return std::string("ERROR");
		break;
	case LEVEL_TYPE_ALARM:
		return std::string("ALARM");
		break;
	case LEVEL_TYPE_FATAL:
		return std::string("FATAL");
		break;
	default:
		return "";
		break;
	}
}

std::string EnumTdStateToString(ModuleThreadState threadState)
{
	switch (threadState)
	{
	case thread_wait:
		return std::string("INFO");
		break;
	case thread_running:
		return std::string("WARN");
		break;
	case thread_stop:
		return std::string("ERROR");
		break;
	case thread_error:
		return std::string("ALARM");
		break;
	default:
		return "";
		break;
	}
}

std::string GetSystemTime()
{
	time_t time_seconds = time(0);
	struct tm ptm;
	localtime_s(&ptm, &time_seconds);
	char char_time[50] = {};
	sprintf_s(char_time, " %04d-%02d-%02dT%02d:%02d:%02dZ", ptm.tm_year + 1900,
		ptm.tm_mon + 1, ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
	std::string str_system_time = static_cast<std::string>(char_time);
	return str_system_time;
}


bool IsExistUsingNcCard()
{
	CoInitialize(NULL);
	INetConnectionManager* pNetManager;
	INetConnection* pNetConnection;
	IEnumNetConnection* pEnum;

	ULONG celtFetched;
	std::string strComGuid;
	NETCON_PROPERTIES* properties = nullptr;
	TapReg setRegGuid = get_tap_reg();
	NcStatusList ncStausList;

	if (setRegGuid.size() == 0)
	{
		LOGW("%s Reg Not Found, Re Install", __SP_FUNC__);
		return false;
	}

	if (S_OK != CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&pNetManager))
	{
		LOGW("%s Com Instance Failed, sleep(5s)", __SP_FUNC__);
		return false;
	}

	pNetManager->EnumConnections(NCME_DEFAULT, &pEnum);
	if (NULL == pEnum)
	{
		pNetManager->Release();
		return false;
	}

	while (pEnum->Next(1, &pNetConnection, &celtFetched) == S_OK)
	{
		pNetConnection->GetProperties(&properties);

		strComGuid = GUIDToString(properties->guidId);

		// 查看当前网卡与注册表的guid一致时的状态
		for (auto& it : setRegGuid)
		{
			// #1 判断是否安装了 虚拟网卡
			if (strComGuid.compare(it) == 0)
			{
				ncStausList[properties->Status] = properties->pszwDeviceName;
			}
		}
	}

	pEnum->Release();
	pNetManager->Release();
	CoUninitialize();

	return ncStausList.find(NCS_CONNECTED) != ncStausList.end();
}

TapReg get_tap_reg()
{
	using std::string;
	using std::vector;

	TapReg tap_reg;

	WinRegKey reg_key;
	if (!reg_key.open_exist(HKLM, _adapter_key))
	{
		SP_LOGW("open regedit %s failed: %s",
			_adapter_key, reg_key.get_error_msg());
	}

	vector<string> sub_keys = reg_key.enum_subkeys();

	//iterator subkeys
	for (auto& iter : sub_keys)
	{
		char connection_string[256];
		sprintf_s(connection_string, sizeof(connection_string),
			"%s\\%s", _adapter_key, iter.c_str());
		WinRegKey sub_reg_key;

		if (!sub_reg_key.open_exist(HKLM, connection_string))
		{
			SP_LOGW("%s Open_exist %s failed: %s", __SP_FUNC__,
				connection_string, sub_reg_key.get_error_msg());
			continue;
		}
		string component_id;

		if (!sub_reg_key.read_key_value("ComponentId", component_id))
		{
			continue;
		}
		string net_cfg_instance_id;

		if (!sub_reg_key.read_key_value("NetCfgInstanceId", net_cfg_instance_id))
		{
			continue;
		}

		if (strcmp(component_id.c_str(), _component_id) == 0)
		{
			tap_reg.insert(net_cfg_instance_id);
		}
	}

	return tap_reg;
}

#endif
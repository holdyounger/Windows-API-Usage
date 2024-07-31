#include <windows.h>
#include <iostream>
#include <vector>
#include <set>
#include "ThreadPools.h"
#include "apr_md5.h"

using namespace std;

typedef struct _RISK_USER_INFO
{
	DWORD		dwWeakType;
	CString     sUserName; // 用户明文密码
	CString     sPassword;
	CStringA    sSoftType;
	CStringA    sService;
	CStringA    sWeakType;
	CStringA    sThreatName;
	BOOL        bUplaod;
}RISK_USER_INFO, * PRISK_USER_INFO;
typedef std::vector<RISK_USER_INFO> vec_risk_user;

static BOOL IsWeakInDicts(RISK_USER_INFO& riskUser, const set<CStringA>& pwdDicts);

namespace bfptpool {

	CThreadPools::CThreadPools(int nMinThreads, int nMaxThreads)
	{
		m_nRollback = 0;
		m_pPool = NULL;
		m_pCleanupGroup = NULL;

		CreateThreadPools(nMinThreads, nMaxThreads);
	}

	CThreadPools::~CThreadPools(void)
	{
		CloseThreadPools();
	}

	void CThreadPools::CreateThreadPools(int nMinThreads, int nMaxThreads)
	{
		BOOL bRet = FALSE;

		InitializeThreadpoolEnvironment(&m_CallBackEnviron);

		// Create a custom, dedicated thread pool
		m_pPool = CreateThreadpool(NULL);

		if (NULL == m_pPool)
		{
			CloseThreadPools();
		}

		m_nRollback = 1; // pool creation succeeded

		// The thread pool is made persistent simply by setting
		if (nMaxThreads)
			SetThreadpoolThreadMaximum(m_pPool, nMaxThreads);
		if (nMinThreads)
			bRet = SetThreadpoolThreadMinimum(m_pPool, nMinThreads);

		SetThreadpoolThreadMinimum(m_pPool, 1);

		if (FALSE == bRet)
		{
			CloseThreadPools();
		}

		//Create a cleanup group for this thread pool
		m_pCleanupGroup = CreateThreadpoolCleanupGroup();

		if (NULL == m_pCleanupGroup)
		{
			CloseThreadPools();
		}

		m_nRollback = 2;  // Cleanup group creation succeeded

		// Associate the callback environment with our thread pool.
		SetThreadpoolCallbackPool(&m_CallBackEnviron, m_pPool);

		// Associate the cleanup group with our thread pool.
		// Objects created with the same callback environment
		// as the cleanup group become members of the cleanup group.
		SetThreadpoolCallbackCleanupGroup(&m_CallBackEnviron, m_pCleanupGroup, NULL);

		m_nRollback = 3;  // Creation of work succeeded

	}

	void CThreadPools::CloseThreadPools()
	{
		switch (m_nRollback)
		{
			case 4:
			case 3:
				// Clean up the cleanup group members.
				CloseThreadpoolCleanupGroupMembers(m_pCleanupGroup, FALSE, NULL);

			case 2:
				// Clean up the cleanup group.
				CloseThreadpoolCleanupGroup(m_pCleanupGroup);
				m_pCleanupGroup = NULL;

			case 1:
				// Clean up the pool.
				CloseThreadpool(m_pPool);
				m_pPool = NULL;

			default:
				break;
		}
	}

	/**
	 * @brief CCheckThread
	 * @param nMinThreads 
	 * @param nMaxThreads 
	 */
	CCheckThreadPools::CCheckThreadPools(int nMinThreads, int nMaxThreads): CThreadPools(nMinThreads, nMaxThreads)
	{
		m_pWork = NULL;
	}

	CCheckThreadPools::~CCheckThreadPools(void)
	{
		CloseWorkThread();
	}

	void CCheckThreadPools::SubmitTask(CWorkThreadData_Task* pParameter)
	{
		if (!TrySubmitThreadpoolCallback(TryCallback, pParameter, &m_CallBackEnviron))
		{
			printf("Submit Task Error");
			return;
		}
	}

	void CCheckThreadPools::RunWorkThread(PVOID pParameter)
	{
		CWorkThreadData_Task* pDataPtr = static_cast<CWorkThreadData_Task*>(pParameter);
		if (pDataPtr == NULL) 
			return;

		CCheckThreadPools* pThreadPtr = static_cast<CCheckThreadPools*>(pDataPtr->m_pCallerPtr);
		if (pThreadPtr)
		{
			pThreadPtr->WorkThreadProcess(pParameter);
		}
	}

	BOOL CCheckThreadPools::CreateWorkThread(CWorkThreadData_Task* pTask)
	{
		if (NULL == m_pPool || NULL == m_pCleanupGroup) return FALSE;

		pTask->m_pCallerPtr = this;

		m_pWork = CreateThreadpoolWork((PTP_WORK_CALLBACK)WorkCallback, pTask, &m_CallBackEnviron);
		if (NULL == m_pWork) return FALSE;
		
		SubmitThreadpoolWork(m_pWork);

		return TRUE;
	}

	void CCheckThreadPools::CloseWorkThread()
	{
		if (NULL == m_pWork) return;

		WaitForThreadpoolWorkCallbacks(m_pWork, TRUE);

		CloseThreadpoolWork(m_pWork);

		m_pWork = NULL;
	}

	VOID CCheckThreadPools::TryCallback(PTP_CALLBACK_INSTANCE, PVOID pParameter)
	{
		auto task = (bfptpool::CWorkThreadData_Task*)pParameter;

		task->setExecuteMode(bfptpool::CWorkThreadData_Task::ExecMode_Try);

		task->execute();
	}

	VOID CCheckThreadPools::WorkCallback(PTP_CALLBACK_INSTANCE pInstance, PVOID pParameter, PTP_WORK pWork)
	{
		// Instance, Parameter, and Work not used in this example.
		UNREFERENCED_PARAMETER(pInstance);
		UNREFERENCED_PARAMETER(pParameter);
		UNREFERENCED_PARAMETER(pWork);

		// Do something when the work callback is invoked.
		CWorkThreadData_Task* pDataPtr = static_cast<CWorkThreadData_Task*>(pParameter);
		if (pDataPtr)
		{
			CCheckThreadPools* pThreadPtr = static_cast<CCheckThreadPools*>(pDataPtr->m_pCallerPtr);
			if (pThreadPtr)
			{
				pThreadPtr->RunWorkThread(pParameter);
			}
		}

		return;
	}
}

namespace jenkinsCheck {
	/**
	 * @brief Task 
	 */
	CJenkinsThreadDT::CJenkinsThreadDT(JenkinsCheckData data, PVOID pFeedBack)
		:m_data(data), CWorkThreadData_Task(pFeedBack)
	{

	}

	CJenkinsThreadDT::~CJenkinsThreadDT()
	{

	}

	void CJenkinsThreadDT::execute()
	{
		do
		{
			if (execute(m_data.m_pwdInnerDicts))
				break;
			if (execute(m_data.m_pwdUserDicts))
				break;
		} while (FALSE);

		m_bDone = TRUE;
	}

	/**
	 * @brief 执行校验
	 */
	BOOL CJenkinsThreadDT::execute(const std::set<CString>& pwdDict)
	{
#ifdef _DEBUG
		std::cout << "[" << __FUNCTION__ << "] Task executed by thread ID: " << GetCurrentThreadId() << std::endl;
#endif

		BOOL bFind = FALSE;
		auto feedback = [&]() 
			{
				if (m_pFeedBack)
				{
					((void(*)())m_pFeedBack)();
				}
			};


		RISK_USER_INFO riskUser;
		riskUser.sPassword = m_data.m_strHash;

		if (!pwdDict.empty())
		{
			if (TRUE == IsWeakInDicts(riskUser, pwdDict))
			{
				m_strPwd = riskUser.sPassword;
				bFind = TRUE;
			}
		}

		return bFind;

	}

	CJenkinsCheckThreadPools::CJenkinsCheckThreadPools(int nMinThreads, int nMaxThreads) : CCheckThreadPools(nMinThreads, nMaxThreads)
	{

	}

	CJenkinsCheckThreadPools::~CJenkinsCheckThreadPools(void)
	{
	}

	void CJenkinsCheckThreadPools::taskCallback(PTP_CALLBACK_INSTANCE, PVOID pParameter)
	{
		auto task = (bfptpool::CWorkThreadData_Task*)pParameter;

		task->setExecuteMode(bfptpool::CWorkThreadData_Task::ExecMode_Try);

		task->execute();
	}

	void CJenkinsCheckThreadPools::SubmitTask(bfptpool::CWorkThreadData_Task* pParameter)
	{
		if (!TrySubmitThreadpoolCallback(taskCallback, pParameter, &m_CallBackEnviron))
		{
			printf("Submit Task Error");
			return;
		}
	}

	void CJenkinsCheckThreadPools::WorkThreadProcess(PVOID pParameter)
	{
		jenkinsCheck::CJenkinsThreadDT* pDataPtr = static_cast<jenkinsCheck::CJenkinsThreadDT*>(pParameter);
		if (pDataPtr == NULL) 
			return;

		CCheckThreadPools* pThreadPtr = static_cast<CCheckThreadPools*>(pDataPtr->m_pCallerPtr);

		pDataPtr->setExecuteMode(bfptpool::CWorkThreadData_Task::ExecMode_Create);

		do 
		{
			if (pDataPtr->execute(pDataPtr->m_data.m_pwdInnerDicts))
			{
				break;
			}
			if (pDataPtr->execute(pDataPtr->m_data.m_pwdUserDicts))
			{
				break;
			}
		} while (FALSE);

		pDataPtr->m_bDone = TRUE;
	}
}

#define STATUS_OK				(0)
#define STATUS_ERROR			(-1)
#define STATUS_EOF				(-2)

BOOL IsWeakInDicts(RISK_USER_INFO& riskUser, const set<CStringA>& pwdDicts)
{
	for (auto pwd : pwdDicts)
	{
		std::string sHash = riskUser.sPassword.GetBuffer();
		std::string sPwd = pwd.GetBuffer();
		if (true == apr_password_validate(sPwd.c_str(), sHash.c_str()))
		{
			riskUser.sPassword = pwd;
#if BFPWEAKPWD_ENABLE_RISKTYPE
#else
			if (sPwd.empty())
				return FALSE;
#endif
			return TRUE;
		}
	}
	return FALSE;
};


bfptpool::CCheckThreadPools* create_jenkins_check_thread_pool(int minThreads, int maxThreads)
{
	return new (std::nothrow) jenkinsCheck::CJenkinsCheckThreadPools(minThreads, maxThreads);
}
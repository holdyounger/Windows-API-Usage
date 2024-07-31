#pragma once

const int DEFAULT_MIN_POOL_THREADS = 1;
const int DEFAULT_MAX_POOL_THREADS = 500;

#include <vector>

namespace bfptpool {

	class CWorkThreadData_Task
	{
	public:
		enum ExecMode {
			ExecMode_None,
			ExecMode_Create,
			ExecMode_Try,
		};

	public:
		CWorkThreadData_Task(PVOID pFeedBack) : m_pFeedBack(pFeedBack)
		{
			m_mode = ExecMode_None;
		}

		~CWorkThreadData_Task() {};

		virtual void execute() = 0;
		virtual void setExecuteMode(ExecMode mode) { m_mode = mode; };

	public:
		PVOID	m_pCallerPtr;	// 调用方
		PVOID	m_pFeedBack;	// 自定义回调
		ExecMode m_mode;
	};

	class CThreadPools
	{
	public:
		CThreadPools(int nMinThreads, int nMaxThreads);
		virtual ~CThreadPools(void);

	private:
		void CreateThreadPools(int nMinThreads, int nMaxThreads);
		void CloseThreadPools();

	protected:
		int						m_nRollback;
		PTP_POOL				m_pPool;
		TP_CALLBACK_ENVIRON		m_CallBackEnviron;
		PTP_CLEANUP_GROUP		m_pCleanupGroup;
	};

	/**
	 * @brief 两种使用方式：
			1. 通过 SubmitTask 提交，不可以停止，跑完整个字典
			2. 通过 CreateWorkThread 提交，可以在检测出后停止
	 */
	class CCheckThreadPools : public CThreadPools
	{
	public:
		CCheckThreadPools(int nMinThreads, int nMaxThreads);
		virtual ~CCheckThreadPools(void);

		BOOL CreateWorkThread(CWorkThreadData_Task* pParameter);
		void CloseWorkThread();

	public:
		virtual void SubmitTask(CWorkThreadData_Task* pParameter);
		virtual void WorkThreadProcess(PVOID pParameter) = 0;

	protected:
		void RunWorkThread(PVOID pParameter);
		static VOID CALLBACK WorkCallback(PTP_CALLBACK_INSTANCE pInstance, PVOID pParameter, PTP_WORK pWork);
		static VOID CALLBACK TryCallback(PTP_CALLBACK_INSTANCE, PVOID pParameter);

	protected:
		PTP_WORK	m_pWork;
	};
}

#include <set>
#include <atlstr.h>

namespace jenkinsCheck 
{

	class JenkinsCheckData
	{
	public:
		JenkinsCheckData() {};
		~JenkinsCheckData() {};

	public:
		CString					m_strHash;			// 用户Hash
		CString					m_strPwd;			// 用户明文密码
		/**
		 * @brief 由于字典是所有用户共用的，放在线程池中比较合适
		 */
		std::set<CString>	m_pwdInnerDicts;		// 内置字典
		std::set<CString>	m_pwdUserDicts;			// 用户字典
	};

	/**
	 * @brief Task
	 */
	class CJenkinsThreadDT : public bfptpool::CWorkThreadData_Task
	{
	public:
		CJenkinsThreadDT(JenkinsCheckData data, PVOID pFeedBack = nullptr);
		~CJenkinsThreadDT();

		virtual void execute();
		virtual BOOL execute(const std::set<CString>& pwdDict);

		BOOL IsWorkDone() { return m_bDone; };

	public:
		JenkinsCheckData m_data;
		BOOL m_bDone = FALSE;
		CString m_strPwd;
		WORD m_nRiskType;
	};

	/**
	 * @brief CJenkinsCheckThreadPools
	 */
	class CJenkinsCheckThreadPools : public bfptpool::CCheckThreadPools
	{

	public:
		CJenkinsCheckThreadPools(int nMinThreads, int nMaxThreads);
		virtual ~CJenkinsCheckThreadPools(void);

	public:
		virtual void SubmitTask(bfptpool::CWorkThreadData_Task* pParameter);
		virtual void WorkThreadProcess(PVOID pParameter) override;

	protected:
		static void CALLBACK taskCallback(PTP_CALLBACK_INSTANCE, void* pParameter);

#if 0

	public:
		/**
		 * @brief 由于字典是所有用户共用的，放在线程池中比较合适
		 */
		std::set<CString>	m_pwdInnerDicts;		// 内置字典
		std::set<CString>	m_pwdUserDicts;			// 用户字典
#endif
	};
}


bfptpool::CCheckThreadPools* create_jenkins_check_thread_pool(int minThreads = DEFAULT_MIN_POOL_THREADS, int maxThreads = DEFAULT_MAX_POOL_THREADS);
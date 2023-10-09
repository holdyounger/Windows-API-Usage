#pragma once
/*在调用的进程里面，使用类似：CInstallCrashDump g_installDump; 
在stdafx.cpp 中，加入
#include "common/windows/CrashDump/CrashReport.cpp"
#include "common/windows/CrashDump/ExceptionReport.cpp"
*/
namespace CrashReport
{
    class CCrashReporter
    {
    public:
        static CCrashReporter& GetInstaller(void)
		{
			static CCrashReporter _crashReporter;
			return _crashReporter;
		}

    private:
        CCrashReporter();
        ~CCrashReporter();

    public:
        bool Init();
        void UnInit(void);
        bool IsExiting(void) const;

    private:
        bool m_bIsExiting;
    };
};


class CInstallCrashDump
{
public:
	CInstallCrashDump()
	{
		CrashReport::CCrashReporter::GetInstaller().Init();
	}

	~CInstallCrashDump()
	{
		CrashReport::CCrashReporter::GetInstaller().UnInit();
	}
};

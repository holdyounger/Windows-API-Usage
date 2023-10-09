// #include "stdafx.h"
#include "generate_mini_dump_win.h"
#include "exception_report.h"

CrashReport::CCrashReporter::CCrashReporter() :m_bIsExiting(false)
{
	
}

CrashReport::CCrashReporter::~CCrashReporter()
{

}

void CrashReport::CCrashReporter::UnInit(void)
{
    m_bIsExiting = true;
}

bool CrashReport::CCrashReporter::IsExiting(void) const
{
    return m_bIsExiting;
}

static LONG WINAPI CustomUnhandledExceptionFilter(PEXCEPTION_POINTERS pExInfo)
{
	CExceptionReport report(pExInfo);
	report.CatchDump();

	// 无论如何，最终进程总结性的自杀一下。
	::TerminateProcess(::GetCurrentProcess(), 0);

	return EXCEPTION_EXECUTE_HANDLER;
}

bool CrashReport::CCrashReporter::Init()
{
	::SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);

	return true;
}

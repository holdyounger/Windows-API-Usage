#pragma once
#include <atlstr.h>
#include <DbgHelp.h>
class CExceptionReport {

public:
	CExceptionReport(PEXCEPTION_POINTERS pExceptionInfo);

	void CatchDump(void);
	
	CString GenerateDumpFileName();
	void WriteDumpFile(LPCTSTR lpszFileName);

	static BOOL CALLBACK MiniDumpCallback(PVOID CallbackParam, CONST PMINIDUMP_CALLBACK_INPUT CallbackInput, PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);

private:
	bool	m_bMaxDmp;
	bool	m_bVistaLater;
	BOOL    m_bXpSp2OrLater;
	BOOL    m_b2003Sp1OrLater;

	CString m_strReportDir;
	CString m_strAppPath;
	PEXCEPTION_POINTERS m_pException;

};




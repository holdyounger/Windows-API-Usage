#pragma once

class SspiNTLMAuth
{
public:
	static SspiNTLMAuth *Instance();

	BOOL IsEmptyPWUser(__in const CString& Domain, __in const CString& UserName, __out BOOL& bEmptyPw);
};


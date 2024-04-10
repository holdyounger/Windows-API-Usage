#pragma once
#include "PJNNTLMAuth.h"

#ifndef PJNSMTP_EXT_CLASS
#define PJNNTLM_EXT_CLASS
#endif

#ifndef CPJNSMTP_NONTLM
    class PJNNTLM_EXT_CLASS CPJNNTLMConnetion : public CNTLMClientAuth
#else
    class PJNNTLM_EXT_CLASS CPJNNTLMConnetion
#endif
{
public:
    CPJNNTLMConnetion() {};
    ~CPJNNTLMConnetion() {};
#ifndef CPJNSMTP_NONTLM
        virtual SECURITY_STATUS NTLMAuthPhase1(PBYTE pBuf, DWORD cbBuf);
        virtual SECURITY_STATUS NTLMAuthPhase2(PBYTE pBuf, DWORD cbBuf, DWORD* pcbRead);
        virtual SECURITY_STATUS NTLMAuthPhase3(PBYTE pBuf, DWORD cbBuf);
#endif
};


#include "CPJNNTLMConnetion.h"

SECURITY_STATUS CPJNNTLMConnetion::NTLMAuthPhase1(PBYTE pBuf, DWORD cbBuf)
{
    return SEC_E_OK;
}

SECURITY_STATUS CPJNNTLMConnetion::NTLMAuthPhase2(PBYTE pBuf, DWORD cbBuf, DWORD* pcbRead)
{
    return SEC_E_OK;
}

SECURITY_STATUS CPJNNTLMConnetion::NTLMAuthPhase3(PBYTE pBuf, DWORD cbBuf)
{
    return SEC_E_OK;
}

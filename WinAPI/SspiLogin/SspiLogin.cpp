// SspiLogin.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#define SECURITY_WIN32

#include <windows.h>  
#include <sspi.h>  
#include <iostream>  
#include "CPJNNTLMConnetion.h" 
#include <stdio.h>  


#pragma comment(lib, "Secur32.lib")  

#ifndef SEC_SUCCESS
#define SEC_SUCCESS(Status) ((Status) >= 0)
#endif


int main() {

    SecHandle clientContext;
    CredHandle clientCredHandle;
    TimeStamp expiry;
    SecBufferDesc inBufDesc;
    BYTE InBuf[12000];
    BYTE OutBuf[12000];
    SecBuffer inBuffer;
    SecBufferDesc outBufDesc;
    SecBuffer outBuffer;
    SEC_WINNT_AUTH_IDENTITY clientIdentity;
    SECURITY_STATUS secStatus;

    LPCSTR username = "localtestempty";
    LPCSTR password = "localtestempty";

    CPJNNTLMConnetion* auth = new CPJNNTLMConnetion;
    SECURITY_STATUS status1 = auth->NTLMAuthenticate((LPCTSTR)L"", (LPCTSTR)L"");
    SECURITY_STATUS status2 = auth->NTLMAuthenticate((LPCTSTR)L"localtestempty", (LPCTSTR)L"");

    // 设置客户端身份  
    ZeroMemory(&clientIdentity, sizeof(clientIdentity));
#ifdef _UNICODE  
    clientIdentity.User = (unsigned short*)(username);
    clientIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
#else
    clientIdentity.User = reinterpret_cast<unsigned char*>(const_cast<LPTSTR>(username));
    clientIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
#endif
    clientIdentity.UserLength = strlen(reinterpret_cast<const char*>(clientIdentity.User));
    clientIdentity.Domain = NULL; // 对于本地账户，通常不需要指定域  
    clientIdentity.DomainLength = 0;
    clientIdentity.Password = (unsigned short*)(password); // 如果使用当前用户的凭据，则不需要密码  
    clientIdentity.PasswordLength = 0;
    clientIdentity.Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;

    wchar_t ntlm[5] = L"NTLM";
    wchar_t* pntlm = &ntlm[0];

    // 获取客户端凭据句柄  
    SEC_E_INTERNAL_ERROR;
    secStatus = AcquireCredentialsHandle(NULL, pntlm, SECPKG_CRED_OUTBOUND, NULL, &clientIdentity, NULL, NULL, &clientCredHandle, &expiry);
    if (secStatus != SEC_E_OK) {
        std::cerr << "AcquireCredentialsHandle failed: " << std::hex << secStatus << std::endl;
        return 1;
    }

    // 初始化安全上下文  
    ZeroMemory(&inBufDesc, sizeof(inBufDesc));
    ZeroMemory(&inBuffer, sizeof(inBuffer));
    ZeroMemory(&outBufDesc, sizeof(outBufDesc));
    ZeroMemory(&outBuffer, sizeof(outBuffer));

    inBufDesc.ulVersion = SECBUFFER_VERSION;
    inBufDesc.cBuffers = 0;
    inBufDesc.pBuffers = (PSecBuffer)InBuf;

    outBufDesc.ulVersion = SECBUFFER_VERSION;
    outBufDesc.cBuffers = 1;
    outBufDesc.pBuffers = &outBuffer;
    outBuffer.BufferType = SECBUFFER_TOKEN;
    outBuffer.cbBuffer = 12000;
    outBuffer.pvBuffer = &OutBuf;

    memset(&clientContext, 0, sizeof(clientContext));
    ULONG ContextAttributes;
    BOOL fDone = FALSE;
    wchar_t targetName[] = L"localhost"; // 替换为实际的服务器名称

    secStatus = InitializeSecurityContext(&clientCredHandle, NULL, targetName,
        0, 0, SECURITY_NATIVE_DREP, &inBufDesc, 0, &clientContext, &outBufDesc, &ContextAttributes, &expiry);

    //secStatus = InitializeSecurityContext(&clientCredHandle, &clientContext, NULL,
    //   0, 0, SECURITY_NATIVE_DREP, &inBufDesc, 0, &clientContext, &outBufDesc, &ContextAttributes, &expiry);

    if (!SEC_SUCCESS(secStatus))
        return secStatus;

    if (secStatus != SEC_I_CONTINUE_NEEDED) {
        std::cerr << "InitializeSecurityContext failed: " << secStatus << std::endl;
        DeleteSecurityContext(&clientContext);
        FreeCredentialsHandle(&clientCredHandle);
        delete[] outBuffer.pvBuffer;
        return 1;
    }

    // 在这里，你可以使用outBuffer.pvBuffer指向的令牌作为身份验证的一部分发送给服务器  
    // 注意：对于本地测试，你通常不需要实际发送这个令牌，因为服务器端和客户端都在同一台机器上。  







    // 清理资源  
    DeleteSecurityContext(&clientContext);
    FreeCredentialsHandle(&clientCredHandle);
    delete[] outBuffer.pvBuffer;

    return 0;
}
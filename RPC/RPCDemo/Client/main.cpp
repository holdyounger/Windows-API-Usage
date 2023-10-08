//client.cpp
#include <iostream>
#include <string>
using namespace std;


#include "hello_h.h"

#pragma comment(lib,"Rpcrt4.lib")


void doRpcCall();

int main(int argc, char** argv)
{
    int i = 0;
    RPC_STATUS status = 0;

    unsigned char* pszNetworkAddr = NULL;
    unsigned char* pszStringBinding = NULL;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ip") == 0) {
            pszNetworkAddr = (unsigned char*)argv[++i];
            break;
        }
    }

    status = RpcStringBindingCompose(NULL,
        (unsigned char*)"ncacn_np",
        pszNetworkAddr,
        (unsigned char*)"\\pipe\\hello",
        NULL,
        &pszStringBinding);
    if (status != 0) {
        cout << "RpcStringBindingCompose returns: " << status << "!" << endl;
        return -1;
    }

    cout << "pszStringBinding = " << pszStringBinding << endl;
    status = RpcBindingFromStringBinding(pszStringBinding, &hello_IfHandle);
    if (status != 0) {
        cout << "RpcBindingFromStringBinding returns: " << status << "!" << endl;
        return -1;
    }

    doRpcCall();

    status = RpcStringFree(&pszStringBinding);
    if (status != 0)
        cout << "RpcStringFree returns: " << status << "!" << endl;

    status = RpcBindingFree(&hello_IfHandle);
    if (status != 0)
        cout << "RpcBindingFree returns: " << status << "!" << endl;

    cin.get();
    return 0;
}

void doRpcCall(void)
{
    char buff[1024];
    RpcTryExcept{
     while (true) {
      cout << "Please input a string param for Rpc call:" << endl;
      cin.getline(buff, 1023);
      if (strcmp(buff, "exit") == 0 || strcmp(buff, "quit") == 0) {
       Shutdown();
      }
      else {
       HelloProc((unsigned char*)buff);
       cout << "call helloproc succeed!" << endl;
      }
     }
    }

        RpcExcept(1) {
        unsigned long ulCode = RpcExceptionCode();
        cout << "RPC exception occured! code: " << ulCode << endl;
    }
    RpcEndExcept
}

void* __RPC_USER MIDL_user_allocate(size_t len)
{
    return (malloc(len));
}

void __RPC_USER MIDL_user_free(void* ptr)
{
    free(ptr);
}
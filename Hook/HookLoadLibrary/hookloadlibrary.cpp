/*
    C/C++ HOOK API（原理深入剖析之-LoadLibraryA）_c++ loadlibrarya-CSDN博客: 
    https://blog.csdn.net/masefee/article/details/4566121
*/
#include <iostream>
#include <Windows.h>
using namespace std;

#pragma warning( disable: 4309 )
#pragma warning( disable: 4311 )

typedef HMODULE(WINAPI* HOOKAPI)(IN LPCSTR);
#define MYHOOKMETHOD(__fun) HMODULE WINAPI __fun
#define DECLARE_REGISTER( __0bj, __lawfunc, __newfunc ) Inline_Hook<HOOKAPI, 1> __Obj( __lawfunc, __newfunc )

struct __InlineHOOK_Base
{
    DWORD _argsBytes;
    void* _lawFunc;
    void* _newFunc;
    char  _lawByteCode[16];
    char  _newByteCode[16];

    bool unhook(void)
    {
        // It's hooked.
        if (memcmp(_newByteCode, _lawFunc, 16) == 0)
        {
            DWORD dwOldFlag;
            VirtualProtect(_lawFunc, 8, PAGE_EXECUTE_READWRITE, &dwOldFlag);
            memcpy(_lawFunc, _lawByteCode, 16);
            VirtualProtect(_lawFunc, 8, dwOldFlag, &dwOldFlag);
            return true;
        }

        return false;
    }

    bool hook(void)
    {
        // It's saved.
        if (memcmp(_lawByteCode, _lawFunc, 16) == 0)
        {
            DWORD dwOldFlag;
            VirtualProtect(_lawFunc, 8, PAGE_EXECUTE_READWRITE, &dwOldFlag);
            memcpy(_lawFunc, _newByteCode, 16);
            VirtualProtect(_lawFunc, 8, dwOldFlag, &dwOldFlag);
            return true;
        }

        return false;
    }

    __InlineHOOK_Base(void* lawfun, void* newfun, DWORD args);
};


void __declspec(naked) __Inline_Hook_Func(void)
{
    __asm
    {
        push ebp  // save maybe usefull register.
        push ebx
        push esi
        push ecx
        call __InlineHOOK_Base::unhook  // first, remove the hook in order to call the normal function.
        test eax, eax   // check the remove was successful
        jz __return


    __getargnum :
        mov eax, dword ptr[esp] // esp just is ecx, also is __InlineHOOK_Base's this pointer.
        mov ecx, dword ptr[eax] // get first 4 bytes, that is params total size.
        shr ecx, 2  // get params num, equal with __InlineHOOK_Base::_argsBytes / sizeof( DWORD )
        test ecx, ecx   // check whether there are params.
        jz __callfunc   // no param


    __pushargs :
        mov edx, esp    // __InlineHOOK_Base's this pointer.
        add edx, 14h    // navigate to first call ret addr.
        add edx, dword ptr[eax]; // add params size.
        push dword ptr[edx];    // push the dll file name pointer.
        loop __pushargs


    __callfunc :
        call[eax + 8]   // call my function .
        mov ecx, dword ptr[esp]  // get __InlineHOOK_Base's this pointer.

        push edx        // save my function return value.
        push eax

        call __InlineHOOK_Base::hook   // rehook.

        pop eax                  // get saved return value, provided to my superiors to use
        pop edx


    __return :
        pop ecx
        pop esi
        pop ebx
        pop ebp
        ret
    }
}

__InlineHOOK_Base::__InlineHOOK_Base(void* lawfun, void* newfun, DWORD args)
    : _lawFunc(lawfun), _newFunc(newfun), _argsBytes(args * 4)
{
    _newByteCode[0] = 0xB9;     // mov ecx, ...
    (DWORD&)_newByteCode[1] = (DWORD)this;
    _newByteCode[5] = 0xB8;     // mov eax, ...
    (DWORD&)_newByteCode[6] = (DWORD)__Inline_Hook_Func;
    (WORD&)_newByteCode[10] = 0xD0FF;       // call eax
    _newByteCode[12] = 0x000000C3;          // ret

    if (args > 0)
    {
        _newByteCode[12] = 0xC2;        // ret ...
        (WORD&)_newByteCode[13] = (WORD)_argsBytes;
        _newByteCode[15] = 0;
    }

    memcpy(_lawByteCode, _lawFunc, 16); // save
}


template< typename _function, DWORD args >
struct Inline_Hook : __InlineHOOK_Base
{
    Inline_Hook(_function lawfun, _function newfun)
        :__InlineHOOK_Base(lawfun, newfun, args) {
        hook();
    }
    ~Inline_Hook(void) { unhook(); }
};  

MYHOOKMETHOD(myLoadLibrary)(LPCSTR lpcStrFileName)
{
    ::MessageBoxA(NULL, lpcStrFileName, "LoadLibrary Name", MB_OK | MB_ICONINFORMATION);
    return LoadLibraryA(lpcStrFileName);
}


DECLARE_REGISTER(__inline_hook, LoadLibraryA, myLoadLibrary);


int main(void)
{
    HMODULE hIntstance = LoadLibraryA("d3d9.dll");
    return 0;
}
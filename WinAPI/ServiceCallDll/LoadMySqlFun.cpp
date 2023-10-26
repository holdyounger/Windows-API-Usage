#include "define.h"
#include "LoadMySqlFun.h"

int LoadMySQLFun()
{
    HINSTANCE hDLL; // Handle to DLL
    using Face = void (*)();

    hDLL = LoadLibrary(L"CallMysqlDll.dll");
    OutputDebugStringA("------------------LoadLibrary----------------");
    if (hDLL != NULL)
    {
        OutputDebugStringA("------------------LoadLibrary Success----------------");

        Face faceSum = (Face)GetProcAddress(hDLL,
            "ConnMySQL");

        if (!faceSum)
        {
            // handle the error
            OutputDebugStringA("------------------LoadLibrary Call Function Failed----------------");
            FreeLibrary(hDLL);
            return 0;
        }
        else
        {
            OutputDebugStringA("------------------LoadLibrary Call Function----------------");
            // call the function
            // faceSum();
        }
    }
    else
    {
        OutputDebugStringA("------------------LoadLibrary failed----------------");
        DWORD error = GetLastError();
        CString strError = "";
        strError.Format(L"----------------GetLastError=%d", error);
        OutputDebugString(strError.GetBuffer());

    }

    return 0;
}

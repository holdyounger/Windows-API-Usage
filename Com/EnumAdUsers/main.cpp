#include <Iads.h>
#include <adshlp.h>
#include <oaidl.h>
#include <activeds.h>
#include <windows.h>

#include <iostream>
using namespace std;

#pragma comment(lib, "Activeds.lib")
#pragma comment(lib, "ADSIid.lib")
#pragma comment(lib, "Psapi.lib")


int main(int argc, const char* argv[])
{
    IEnumVARIANT* pEnum = NULL;
    IADsContainer* pCont = NULL;
    LPUNKNOWN pUnk = NULL;
    VARIANT var;
    IDispatch* pDisp = NULL;
    ULONG lFetch;
    IADs* pADs = NULL;

    // In this sample, skip error checking.
   // ADsGetObject(L"LDAP://OU=Sales,DC=Fabrikam,DC=COM",
    ADsGetObject(L"LDAP://OU=Sales,DC=Fabrikam,DC=COM",
        IID_IADsContainer, (void**)&pCont);
    pCont->get__NewEnum(&pUnk);
    pCont->Release();

    pUnk->QueryInterface(IID_IEnumVARIANT, (void**)&pEnum);
    pUnk->Release();

    // Enumerate. 
    HRESULT hr = pEnum->Next(1, &var, &lFetch);
    while (SUCCEEDED(hr) && lFetch > 0)
    {
        if (lFetch == 1)
        {
            BSTR bstr;

            pDisp = V_DISPATCH(&var);
            pDisp->QueryInterface(IID_IADs, (void**)&pADs);
            pDisp->Release();
            hr = pADs->get_Name(&bstr);
            if (SUCCEEDED(hr))
            {
                SysFreeString(bstr);
            }

            pADs->Release();
        }

        VariantClear(&var);
        hr = pEnum->Next(1, &var, &lFetch);
    };


    pEnum->Release();
}
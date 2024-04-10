#include <stdio.h>
#include "ZipHelper.h"

#include "./lzma2107/CPP/Common/Common.h"
#include "./lzma2107/CPP/Common/Defs.h"
#include "./lzma2107/CPP/Common/IntToString.h"
#include "./lzma2107/CPP/Common/MyInitGuid.h"
#include "./lzma2107/CPP/Common/MyWindows.h"
#include "./lzma2107/CPP/Common/StringConvert.h"

#include "./lzma2107/CPP/Windows/DLL.h"
#include "./lzma2107/CPP/Windows/FileDir.h"
#include "./lzma2107/CPP/Windows/FileFind.h"
#include "./lzma2107/CPP/Windows/FileName.h"
#include "./lzma2107/CPP/Windows/NtCheck.h"
#include "./lzma2107/CPP/Windows/PropVariant.h"
#include "./lzma2107/CPP/Windows/PropVariantConv.h"

#include "./lzma2107/CPP/7zip/Archive/IArchive.h"
#include "./lzma2107/CPP/7zip/Common/FileStreams.h"
#include "./lzma2107/CPP/7zip/IPassword.h"

#include "./lzma2107/C/7zVersion.h"

#include "CompressUpdateCB.h"

DEFINE_GUID(CLSID_CFormatZip,
    0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00);

#define CLSID_Format CLSID_CFormatZip

using namespace NWindows;
using namespace NFile;
using namespace NDir;

#include <vector>
using namespace std;

STDAPI CreateObject(const GUID* clsid, const GUID* iid, void** outObject);

int QZipComprocess(CString strArchive, CString strPassword, vector<CString>& vetFileList)
{
    int iRet = 0;

    CObjectVector<CDirItem> dirItems;
    for (auto& file : vetFileList)
    {
        CString strFile = file;
        CDirItem di;
        FString name = strFile;
        NFind::CFileInfo fi;
        if (!fi.Find(name))
        {
            printf("Can't find file[%s]", strFile);
            //return 1;
            continue;
        }

        di.Attrib = fi.Attrib;
        di.Size = fi.Size;
        di.CTime = fi.CTime;
        di.ATime = fi.ATime;
        di.MTime = fi.MTime;
        //di.Name = fs2us(name);
        di.Name = fi.Name; /*fs2us(name)*/;
        di.FullPath = name;
        dirItems.Add(di);
    }

    COutFileStream* outFileStreamSpec = new COutFileStream;
    CMyComPtr<IOutStream> outFileStream = outFileStreamSpec;
    if (!outFileStreamSpec->Create(strArchive, true))
    {
        //PrintError("can't create archive file");
        printf("[QZipComprocess]无法创建压缩文件[%s],GetLastError=%d", strArchive, GetLastError());
        return 1;
    }

    CMyComPtr<IOutArchive> outArchive;
    if (CreateObject(&CLSID_Format, &IID_IOutArchive, (void**)&outArchive) != S_OK)
    {
        //PrintError("Can not get class object");
        printf("[QZipComprocess]CreateObject[CLSID_Format]失败[%s],GetLastError=%d", strArchive, GetLastError());
        return 2;
    }

    CArchiveUpdateCallback* updateCallbackSpec = new CArchiveUpdateCallback;
#if 1
    updateCallbackSpec->PasswordIsDefined = true;
    // updateCallbackSpec->AskPassword = false;
    updateCallbackSpec->Password = strPassword;
    // updateCallbackSpec->IndexInArc = 1;
#endif

    CMyComPtr<IArchiveUpdateCallback2> updateCallback(updateCallbackSpec);
    updateCallbackSpec->Init(&dirItems);

#if 1
    // 如果需要使用 AES 加密密码，则启动这段宏
    {
        const wchar_t* names[] =
        {
          L"em"
        };
        const unsigned kNumProps = ARRAY_SIZE(names);
        NCOM::CPropVariant values[kNumProps] =
        {
          L"aes128",
          // false,    // solid mode OFF
          // (UInt32)9 // compression level = 9 - ultra
        };
        CMyComPtr<ISetProperties> setProperties;
        outArchive->QueryInterface(IID_ISetProperties, (void**)&setProperties);
        if (!setProperties)
        {
            // PrintError("ISetProperties unsupported");
            return 1;
        }
        if (setProperties->SetProperties(names, values, kNumProps) != S_OK)
        {
            // PrintError("SetProperties() error");
            return 1;
        }
    }
#endif // 0

    HRESULT result = outArchive->UpdateItems(outFileStream, dirItems.Size(), updateCallback);

    updateCallbackSpec->Finilize();

    if (result != S_OK)
    {
        printf("[QZipComprocess]Update Error(%S),UpdateItems return %d,GetLastError=%d", strArchive.GetBuffer(), result, GetLastError());
        return 3;
    }

    FOR_VECTOR(i, updateCallbackSpec->FailedFiles)
    {
        //PrintNewLine();
        //PrintError("Error for file", updateCallbackSpec->FailedFiles[i]);
    }

    if (updateCallbackSpec->FailedFiles.Size() != 0) {
        printf("[[QZipComprocess]updateCallbackSpec->FailedFiles.Size() != 0[%s],GetLastError=%d", strArchive, GetLastError());
        return 4;
    }

    return iRet;
}

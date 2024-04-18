#include <stdio.h>
#include "7zHelper.h"

#include "./lzma2107/CPP/Common/Common.h"
#include "./lzma2107/CPP/Common/MyWindows.h"
#include "./lzma2107/CPP/Common/Defs.h"
#include "./lzma2107/CPP/Common/MyInitGuid.h"
#include "./lzma2107/CPP/Common/IntToString.h"
#include "./lzma2107/CPP/Common/StringConvert.h"

#include "./lzma2107/CPP/Windows/DLL.h"
#include "./lzma2107/CPP/Windows/FileDir.h"
#include "./lzma2107/CPP/Windows/FileFind.h"
#include "./lzma2107/CPP/Windows/FileName.h"
#include "./lzma2107/CPP/Windows/NtCheck.h"
#include "./lzma2107/CPP/Windows/PropVariant.h"
#include "./lzma2107/CPP/Windows/PropVariantConv.h"

#include "./lzma2107/CPP/7zip/Common/FileStreams.h"
#include "./lzma2107/CPP/7zip/Archive/IArchive.h"
#include "./lzma2107/CPP/7zip/IPassword.h"

#include "./lzma2107/C/7zVersion.h"

using namespace NWindows;
using namespace NFile;
using namespace NDir;

static const wchar_t* const kEmptyFileAlias = L"[Content]";

////////////////////////////////////////////////////////////////
// Archive Creating callback class

struct CDirItem
{
    UInt64 Size;
    FILETIME CTime;
    FILETIME ATime;
    FILETIME MTime;
    UString Name;
    FString FullPath;
    UInt32 Attrib;

    bool isDir() const { return (Attrib & FILE_ATTRIBUTE_DIRECTORY) != 0; }
};

class CArchiveUpdateCallback :
    public IArchiveUpdateCallback2,
    public ICryptoGetTextPassword2,
    public CMyUnknownImp
{
public:
    MY_UNKNOWN_IMP2(IArchiveUpdateCallback2, ICryptoGetTextPassword2)

        // IProgress
        STDMETHOD(SetTotal)(UInt64 size);
    STDMETHOD(SetCompleted)(const UInt64* completeValue);

    // IUpdateCallback2
    STDMETHOD(GetUpdateItemInfo)(UInt32 index,
        Int32* newData, Int32* newProperties, UInt32* indexInArchive);
    STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT* value);
    STDMETHOD(GetStream)(UInt32 index, ISequentialInStream** inStream);
    STDMETHOD(SetOperationResult)(Int32 operationResult);
    STDMETHOD(GetVolumeSize)(UInt32 index, UInt64* size);
    STDMETHOD(GetVolumeStream)(UInt32 index, ISequentialOutStream** volumeStream);

    STDMETHOD(CryptoGetTextPassword2)(Int32* passwordIsDefined, BSTR* password);

public:
    CRecordVector<UInt64> VolumesSizes;
    UString VolName;
    UString VolExt;

    FString DirPrefix;
    const CObjectVector<CDirItem>* DirItems;

    bool PasswordIsDefined;
    UString Password;
    bool AskPassword;

    bool m_NeedBeClosed;

    int IndexInArc = -1; // 使用的压缩方式 NFileHeader::NCompressionMethod::kDeflate

    FStringVector FailedFiles;
    CRecordVector<HRESULT> FailedCodes;

    CArchiveUpdateCallback() :
        DirItems(NULL),
        PasswordIsDefined(false),
        AskPassword(false)
    {}

    ~CArchiveUpdateCallback() { Finilize(); }
    HRESULT Finilize();

    void Init(const CObjectVector<CDirItem>* dirItems)
    {
        DirItems = dirItems;
        m_NeedBeClosed = false;
        FailedFiles.Clear();
        FailedCodes.Clear();
    }
};

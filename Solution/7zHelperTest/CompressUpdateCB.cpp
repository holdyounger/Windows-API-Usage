#include "CompressUpdateCB.h"

STDMETHODIMP CArchiveUpdateCallback::SetTotal(UInt64 /* size */)
{
    return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::SetCompleted(const UInt64* /* completeValue */)
{
    return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::GetUpdateItemInfo(UInt32 /* index */,
    Int32* newData, Int32* newProperties, UInt32* indexInArchive)
{
    if (newData)
        *newData = BoolToInt(true);
    if (newProperties)
        *newProperties = BoolToInt(true);
    if (indexInArchive)
        *indexInArchive = (UInt32)(Int32)IndexInArc;
    return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::GetProperty(UInt32 index, PROPID propID, PROPVARIANT* value)
{
    NCOM::CPropVariant prop;

    if (propID == kpidIsAnti)
    {
        prop = false;
        prop.Detach(value);
        return S_OK;
    }

    {
        const CDirItem& dirItem = (*DirItems)[index];
        switch (propID)
        {
        case kpidPath:  prop = dirItem.Name; break;
        case kpidIsDir:  prop = dirItem.isDir(); break;
        case kpidSize:  prop = dirItem.Size; break;
        case kpidAttrib:  prop = dirItem.Attrib; break;
        case kpidCTime:  prop = dirItem.CTime; break;
        case kpidATime:  prop = dirItem.ATime; break;
        case kpidMTime:  prop = dirItem.MTime; break;
        }
    }
    prop.Detach(value);
    return S_OK;
}

HRESULT CArchiveUpdateCallback::Finilize()
{
    if (m_NeedBeClosed)
    {
        m_NeedBeClosed = false;
    }
    return S_OK;
}

static void GetStream2(const wchar_t* name)
{
    //Print("Compressing  ");
    if (name[0] == 0)
        name = kEmptyFileAlias;
    //Print(name);
}

STDMETHODIMP CArchiveUpdateCallback::GetStream(UInt32 index, ISequentialInStream** inStream)
{
    RINOK(Finilize());

    const CDirItem& dirItem = (*DirItems)[index];
    GetStream2(dirItem.Name);

    if (dirItem.isDir())
        return S_OK;

    {
        CInFileStream* inStreamSpec = new CInFileStream;
        CMyComPtr<ISequentialInStream> inStreamLoc(inStreamSpec);
        FString path = DirPrefix + dirItem.FullPath;
        if (!inStreamSpec->Open(path))
        {
            DWORD sysError = ::GetLastError();
            FailedCodes.Add(sysError);
            FailedFiles.Add(path);
            // if (systemError == ERROR_SHARING_VIOLATION)
            {
                // PrintError("WARNING: can't open file");
                 // Print(NError::MyFormatMessageW(systemError));
                return S_FALSE;
            }
            // return sysError;
        }
        *inStream = inStreamLoc.Detach();
    }
    return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::SetOperationResult(Int32 /* operationResult */)
{
    m_NeedBeClosed = true;
    return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::GetVolumeSize(UInt32 index, UInt64* size)
{
    if (VolumesSizes.Size() == 0)
        return S_FALSE;
    if (index >= (UInt32)VolumesSizes.Size())
        index = VolumesSizes.Size() - 1;
    *size = VolumesSizes[index];
    return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::GetVolumeStream(UInt32 index, ISequentialOutStream** volumeStream)
{
    wchar_t temp[16];
    ConvertUInt32ToString(index + 1, temp);
    UString res = temp;
    while (res.Len() < 2)
        res.InsertAtFront(L'0');
    UString fileName = VolName;
    fileName += '.';
    fileName += res;
    fileName += VolExt;
    COutFileStream* streamSpec = new COutFileStream;
    CMyComPtr<ISequentialOutStream> streamLoc(streamSpec);
    if (!streamSpec->Create(us2fs(fileName), false))
        return ::GetLastError();
    *volumeStream = streamLoc.Detach();
    return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::CryptoGetTextPassword2(Int32* passwordIsDefined, BSTR* password)
{
    if (!PasswordIsDefined)
    {
        if (AskPassword)
        {
            // You can ask real password here from user
            // Password = GetPassword(OutStream);
            // PasswordIsDefined = true;
            // PrintError("Password is not defined");
            return E_ABORT;
        }
    }
    *passwordIsDefined = BoolToInt(PasswordIsDefined);
    return StringToBstr(Password, password);
}

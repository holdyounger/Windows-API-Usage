#include <wdm.h>

typedef struct _LDR_DATA_TABLE_ENTRY
{
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID      DllBase;
    PVOID      EntryPoint;
    UINT64    SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
}LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    DbgPrint("DriverUnload");
}


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, UNICODE_STRING RegistryPath)
{

    NTSTATUS status = STATUS_SUCCESS;

    DbgPrint("DriverEntry");

    DriverObject->DriverUnload = DriverUnload;

    PLDR_DATA_TABLE_ENTRY pDection = DriverObject->DriverSection;    //��ȡ��ǰ������LDR_DATA_TABLE_ENTRY��ַ

    PLDR_DATA_TABLE_ENTRY pCurrentDection = pDection;    //��¼��ǰ����LDR_DATA_TABLE_ENTRY��ַ

    do
    {
        pDection = pDection->InLoadOrderLinks.Flink;   //�Ȳ�ѯ��һ��

        DbgPrint("%ws", pDection->BaseDllName.Buffer);    //���ģ����

    } while (pCurrentDection != pDection);    //��������ǰ����LDR_DATA_TABLE_ENTRY��ַʱ��˵����ѯ����

    return status;

}
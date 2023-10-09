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

    PLDR_DATA_TABLE_ENTRY pDection = DriverObject->DriverSection;    //获取当前驱动的LDR_DATA_TABLE_ENTRY地址

    PLDR_DATA_TABLE_ENTRY pCurrentDection = pDection;    //记录当前驱动LDR_DATA_TABLE_ENTRY地址

    do
    {
        pDection = pDection->InLoadOrderLinks.Flink;   //先查询下一个

        DbgPrint("%ws", pDection->BaseDllName.Buffer);    //输出模块名

    } while (pCurrentDection != pDection);    //遍历到当前驱动LDR_DATA_TABLE_ENTRY地址时，说明查询结束

    return status;

}
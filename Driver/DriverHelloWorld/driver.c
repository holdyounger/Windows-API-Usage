// #include <ntddk.h>

#include <ntifs.h>

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriver)
{
	DbgPrint("Bye, Driver");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING pReg)
{
	DbgPrint("Hello World, Driver");
	DbgPrint("PDRIVER_OBJECT-> [0x%08x]", pDriver);
	DbgPrint("PUNICODE_STRING->[%ws]", pReg->Buffer);

	pDriver->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}

// GetDriveType.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

int queryStorage(_TCHAR* argv)
{

	// 1 创建句柄
	BOOL         bResult;
	DWORD        dwOutBytes;
	STORAGE_PROPERTY_QUERY Query;
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	HANDLE hDevice=CreateFile(argv,// 要打开的驱动器设备；格式必须为“\\.\C:”，否则打开失败，返回错误码2：系统找不到指定的文件。
		GENERIC_READ,// 访问权限
		FILE_SHARE_READ | FILE_SHARE_WRITE, //共享模式
		NULL,// 使用默认的安全属性
		OPEN_EXISTING,// 打开存在的设备
		NULL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("LINE:%d-%d ",__LINE__, GetLastError());
		printf("DisksPropertyError_InvalidHandle\n");
		return 0;
	}

	STORAGE_DESCRIPTOR_HEADER *pDevDescHeader;
	pDevDescHeader = (STORAGE_DESCRIPTOR_HEADER *)malloc(sizeof(STORAGE_DESCRIPTOR_HEADER));
	if (NULL == pDevDescHeader)
	{
		CloseHandle(hDevice);
		printf("DisksPropertyError_InitStructFailed----pDevDescHeader is NULL!\n");
		return 0;
	}
	bResult = DeviceIoControl(
		hDevice,
		IOCTL_STORAGE_QUERY_PROPERTY,
		&Query,sizeof(Query),
		pDevDescHeader,
		sizeof(STORAGE_DESCRIPTOR_HEADER),
		&dwOutBytes,
		NULL);
	if (!bResult)
	{
		//内核释放
		if(pDevDescHeader)
		{
			free(pDevDescHeader);
		}

		if (hDevice)
		{
			CloseHandle(hDevice);
		}

		printf("LINE:%d-%d ",__LINE__, GetLastError());
		printf("DisksPropertyError_DeviceIoControlFailed----DeviceIoControl is failed\n");
		return 0;
	}

	PSTORAGE_DEVICE_DESCRIPTOR pDevDesc;
	pDevDesc=(STORAGE_DEVICE_DESCRIPTOR *)malloc(pDevDescHeader->Size);

	bResult = DeviceIoControl(hDevice,
		IOCTL_STORAGE_QUERY_PROPERTY,
		&Query, sizeof Query,
		pDevDesc, pDevDescHeader->Size,
		&dwOutBytes,
		NULL);

	//内核释放
	if(pDevDescHeader)
	{
		free(pDevDescHeader);
	}

	if (hDevice)
	{
		CloseHandle(hDevice);
	}

	if(pDevDesc->BusType==BusTypeUsb)//总线类型为USB，返回TRUE
	{
		if(pDevDesc)
		{
			free(pDevDesc);
		}
		printf("LINE:%d-DisksPropertyError_BusTypeUSB\n",__LINE__);
		return 0;
	}

	if (pDevDesc)
	{
		free(pDevDesc);
	}

	printf("LINE:%d-DisksPropertySuccess\n",__LINE__);
	return 0;
}


int getMediaType(_TCHAR* argv)
{
	// 1 创建句柄
	BOOL         bResult;
	DWORD        dwOutBytes;
	STORAGE_PROPERTY_QUERY Query;
	Query.PropertyId = StorageDeviceProperty;
	Query.QueryType = PropertyStandardQuery;

	HANDLE hDevice=CreateFile(argv,// 要打开的驱动器设备；格式必须为“\\.\C:”，否则打开失败，返回错误码2：系统找不到指定的文件。
		GENERIC_READ | GENERIC_WRITE,// 访问权限
		FILE_SHARE_READ | FILE_SHARE_WRITE, //共享模式
		NULL,// 使用默认的安全属性
		OPEN_EXISTING,// 打开存在的设备
		NULL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		printf("LINE:%d-%d ",__LINE__, GetLastError());
		printf("DisksPropertyError_InvalidHandle\n");
		return 0;
	}

	DISK_GEOMETRY *pDevDescHeader;
	pDevDescHeader = (DISK_GEOMETRY *)malloc(sizeof(DISK_GEOMETRY) * 10);
	if (NULL == pDevDescHeader)
	{
		CloseHandle(hDevice);
		printf("DisksPropertyError_InitStructFailed----pDevDescHeader is NULL!\n");
		return 0;
	}
	bResult = DeviceIoControl(
		hDevice,
		IOCTL_STORAGE_GET_MEDIA_TYPES,
		NULL,0,
		pDevDescHeader,
		sizeof(DISK_GEOMETRY)*10,
		&dwOutBytes,
		NULL);
	if (!bResult)
	{
		//内核释放
		if(pDevDescHeader)
		{
			free(pDevDescHeader);
		}

		if (hDevice)
		{
			CloseHandle(hDevice);
		}

		printf("LINE:[%d] GetLastError:[%d] ",__LINE__, GetLastError());
		printf("DisksPropertyError_DeviceIoControlFailed----DeviceIoControl is failed\n");
		return 0;
	}

	PDISK_GEOMETRY pDevDesc;
	pDevDesc=(DISK_GEOMETRY *)malloc(sizeof(DISK_GEOMETRY));

	bResult = DeviceIoControl(hDevice,
		IOCTL_STORAGE_QUERY_PROPERTY,
		&Query, sizeof Query,
		pDevDesc, sizeof pDevDesc,
		&dwOutBytes,
		NULL);

	//内核释放
	if(pDevDescHeader)
	{
		free(pDevDescHeader);
	}

	if (hDevice)
	{
		CloseHandle(hDevice);
	}

	printf("[LINE-%d]:pDevDesc->Cylinders%d\n",pDevDesc->Cylinders);

	if(pDevDesc)
	{
		free(pDevDesc);
	}
	printf("LINE:%d-DisksPropertyError_BusTypeUSB\n",__LINE__);
	return 0;


	if (pDevDesc)
	{
		free(pDevDesc);
	}

	printf("LINE:%d-DisksPropertySuccess\n",__LINE__);
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc >= 2)
	{
		int uType = GetDriveType(argv[1]);
		printf("Drive (%s-%s) Type:%d\n", argv[0], argv[1], uType);

		queryStorage(argv[1]);
		printf("-------------------\n");
		getMediaType(argv[1]);

	}
	else
		printf("Invalid Argument\n");



	return 0;
}


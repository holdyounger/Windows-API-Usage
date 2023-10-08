#include <Windows.h>
#include <WinIoCtl.h>
#include <stdio.h>
#include <ntddser.h>

#define BUF_LEN 4096
#define wszDrive L"\\\\.\\C:"

BOOL GetDriveGeometry(LPWSTR wszPath, DISK_GEOMETRY *pdg)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined 
	BOOL bResult = FALSE;                 // results flag
	DWORD junk = 0;                     // discard results

	hDevice = CreateFileW(wszPath,          // drive to open
		0,                // no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE,
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attributes

	if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
	{
		return (FALSE);
	}

	bResult = DeviceIoControl(hDevice,                       // device to be queried
		IOCTL_DISK_GET_DRIVE_GEOMETRY, // operation to perform
		NULL, 0,                       // no input buffer
		pdg, sizeof(*pdg),            // output buffer
		&junk,                         // # bytes returned
		(LPOVERLAPPED)NULL);          // synchronous I/O

	CloseHandle(hDevice);

	return (bResult);
}

void main()
{
   HANDLE hVol;
   CHAR Buffer[BUF_LEN];

   USN_JOURNAL_DATA JournalData;
   PUSN_RECORD UsnRecord;  

   DWORD dwBytes;
   DWORD dwRetBytes;
   int I;

   DISK_GEOMETRY pdg = { 0 };
   DWORD junk = 0;                     // discard results
   ULONGLONG DiskSize = 0;    // size of the drive, in bytes


   hVol = CreateFileW( (LPWSTR)wszDrive,          // drive to open
	   0,                // no access to the drive
	   FILE_SHARE_READ | // share mode
	   FILE_SHARE_WRITE,
	   NULL,             // default security attributes
	   OPEN_EXISTING,    // disposition
	   0,                // file attributes
	   NULL);            // do not copy file attributes

   // 获取驱动信息
   if (!GetDriveGeometry((LPWSTR)wszDrive, &pdg))          // synchronous I/O
   {
	   printf("Get Drive failed (%d)\n", GetLastError());
	   return;
   }
   else
   {
	   wprintf(L"Drive path      = %ws\n", wszDrive);
	   wprintf(L"Cylinders       = %I64d\n", pdg.Cylinders);
	   wprintf(L"Tracks/cylinder = %ld\n", (ULONG)pdg.TracksPerCylinder);
	   wprintf(L"Sectors/track   = %ld\n", (ULONG)pdg.SectorsPerTrack);
	   wprintf(L"Bytes/sector    = %ld\n", (ULONG)pdg.BytesPerSector);

	   DiskSize = pdg.Cylinders.QuadPart * (ULONG)pdg.TracksPerCylinder *
		   (ULONG)pdg.SectorsPerTrack * (ULONG)pdg.BytesPerSector;
	   wprintf(L"Disk size       = %I64d (Bytes)\n"
		   L"                = %.2f (Gb)\n",
		   DiskSize, (double)DiskSize / (1024 * 1024 * 1024));
   }


   // 创建设备HANDLE
   hVol = CreateFile(wszDrive,
	   GENERIC_READ | GENERIC_WRITE,
	   FILE_SHARE_READ | FILE_SHARE_WRITE,
	   NULL,
	   OPEN_EXISTING,
	   0,
	   NULL);
   if (hVol == INVALID_HANDLE_VALUE)
   {
	   printf("CreateFile failed (%d)\n", GetLastError());
	   return;
   }
   // 查询HANDLE ID
   if( !DeviceIoControl( hVol, 
          FSCTL_QUERY_USN_JOURNAL, 
          NULL,
          0,
          &JournalData,
          sizeof(JournalData),
          &dwBytes,
          NULL) )
   {
      printf( "Query journal failed (%d)\n", GetLastError());
      return;
   }

   // 赋值
   READ_USN_JOURNAL_DATA ReadData = { 0, -1 , 0, 0, 0, JournalData.UsnJournalID };


   printf("sizeof dwBytes: %d\n",sizeof(dwBytes));

   printf("Journal ID: %I64x\n", JournalData.UsnJournalID);
   printf( "FirstUsn: %I64x\n\n", JournalData.FirstUsn );

   // 遍历
   for(I=0; I<=1; I++)
   {
      memset( Buffer, 0, BUF_LEN );

      if( !DeviceIoControl( hVol, // 参数错误
            FSCTL_READ_USN_JOURNAL, 
            &ReadData, 
            sizeof(ReadData),
            &Buffer,
            BUF_LEN,
            &dwBytes,
            NULL) )
      {
         printf( "Read journal failed (%d)\n", GetLastError());
		 printf("Buffer:%s\n",Buffer);
         return;
      }

      dwRetBytes = dwBytes - sizeof(USN);

      // Find the first record
      UsnRecord = (PUSN_RECORD)(((PUCHAR)Buffer) + sizeof(USN));  

      printf( "****************************************\n");

      // This loop could go on for a long time, given the current buffer size.
      while( dwRetBytes > 0 )
      {
         printf( "USN: %I64x\n", UsnRecord->Usn );
         printf("File name: %.*S\n", 
                  UsnRecord->FileNameLength/2, 
                  UsnRecord->FileName );
         printf( "Reason: %x\n", UsnRecord->Reason );
         printf( "\n" );

         dwRetBytes -= UsnRecord->RecordLength;

         // Find the next record
         UsnRecord = (PUSN_RECORD)(((PCHAR)UsnRecord) + 
                  UsnRecord->RecordLength); 
      }
      // Update starting USN for next call
      ReadData.StartUsn = *(USN *)&Buffer; 
   }

   CloseHandle(hVol);

}
#pragma once
#include <libssh2.h>
#include <libssh2_sftp.h>

#define CMySFTPClientSTATUS_OPEN 1
#define CMySFTPClientSTATUS_INIT 2
#define CMySFTPClientSTATUS_SOCKET 4
#define CMySFTPClientSTATUS_HANDSHAKE 8
#define CMySFTPClientSTATUS_FTPSESSION 0x10

#define CMySFTPClientAUTH_PSW 1
#define CMySFTPClientAUTH_KBD 2
#define CMySFTPClientAUTH_PUBKEY 4

#define CMySFTPERR_SSHINITFAIL (-100002)
#define CMySFTPERR_INVALIDSOCKET (-100003)
#define CMySFTPERR_CONNECT (-100004)
#define CMySFTPERR_SESSION_INIT (-100005)
#define CMySFTPERR_HANDSHAKE (-100006)
#define CMySFTPERR_PSWERR (-100007)
#define CMySFTPERR_INVALID_STREAM (-100008)
#define CMySFTPERR_INVALID_FILENAME (-100009)
#define CMySFTPERR_SFTP_INIT (-100010)
#define CMySFTPERR_SFTP_UPLOADMKDIR (-100011)
#define CMySFTPERR_SFTP_SFTP_OPEN (-100012)
#define CMySFTPERR_SFTP_INVALIDARG (-100013)


typedef int(__stdcall* PFN_SFTP_READWRITE_STREAM_FUNCTION)(void* pStream, char* buf, size_t eleSize, size_t eleCount);

class CMySFTPClient
{
public:
	CMySFTPClient();
	~CMySFTPClient();

public:
	/************************************************************************
	* ��������sftpOpen
	* ˵��  ������SFTP����
	* ����  ��1.ip
	*           SFTP������IP
	*       ��2.port
	*           SFTP�������˿�
	*       ��3.username
	*           SFTP�û���
	*       ��4.password
	*           SFTP�û�����
	* ����   �� 0 �ɹ���1 ��������
	/************************************************************************/
	int sftpOpen(const char *ip, int port, const char *username, const char *password);

	/************************************************************************
	* ������ �� sftpClose
	* ˵��   �� �ͷ���Դ
	* ����   �� ��
	* ����   �� 0 �ɹ���1 ��������
	/************************************************************************/
	int sftpClose();

	//ȷ��Ŀ¼���ڣ�û���򴴽�һ��
	int MakeSureDirExist(const char* sftp_dir);

	int sftpOpenFile(const char *sftp_dir, const char *sftpfilename, LIBSSH2_SFTP_HANDLE *& sftp_handle, BOOL bAppendMode);
	int sftpWriteString(LIBSSH2_SFTP_HANDLE * _handle, LPCWSTR lpStr);
	int sftpWriteBytes(LIBSSH2_SFTP_HANDLE * _handle, BYTE* pBuf, DWORD dwBytes);
	int sftpCloseFile(LIBSSH2_SFTP_HANDLE * _handle);

	/************************************************************************
	* ��������uploadFile
	* ˵��  ���ϴ�������
	* ����
	*       ��1.sftppath
	*           SFTP·��
	*       ��2.fnReadStream pStream
	*           Ҫ�ϴ��ı����ļ�������������ȡ����
	*       ��3.sftpfilename
	*           �ϴ���SFTP������������ļ���
	* ����ֵ��0�ɹ�������ʧ�ܣ���
	***********************************************************************/
	int sftpUploadStream(PFN_SFTP_READWRITE_STREAM_FUNCTION fnReadStream, void* pStream, const char *sftp_dir, const char *sftpfilename, BOOL bAutoCreateDir);

	/************************************************************************
	* ��������downloadFile
	* ˵��  �������ļ�
	* ����
	*       ��1.sftpfile
	*           Ҫ���ص�SFTP�ļ�ȫ·���������ļ���
	*       ��2.fnWrite pStream
	*           ���浽���ص���
	* ����ֵ��0�ɹ�������ʧ�ܣ���
	***********************************************************************/
	int sftpDownloadFile(const char *sftpfile, PFN_SFTP_READWRITE_STREAM_FUNCTION fnWrite, void* pStream, size_t&OUT totalWrite);

private:
	/************************************************************************
	* ��������SFTPDirExist
	* ˵��  ���ж�SFTPĿ¼�Ƿ����
	* ����  ��1.sftp_session
	*           LIBSSH2_SFTP�Ựָ��
	*       ��2.sftppath
	*           SFTPĿ¼
	* ����ֵ��1ָ��Ŀ¼���ڣ�0ָ��Ŀ¼������
	***********************************************************************/
	BOOL _sftpDirExist(LIBSSH2_SFTP *sftp_session, const char *sftppath);
	/************************************************************************
	* ��������makeSFTPDir
	* ˵��  ������SFTPĿ¼
	* ����  ��1.sftp_session
	*           LIBSSH2_SFTP�Ựָ��
	*       ��2.sftppath
	*           SFTPĿ¼
	* ����ֵ��0�ɹ���-1ʧ��
	***********************************************************************/
	int _sftpMakeDir(LIBSSH2_SFTP *sftp_session, const char *sftppath);

private:
	int              m_iOpenStatus;
	unsigned long    m_hostaddr;
	SOCKET           m_sock;
	sockaddr_in      m_insockaddr;
	LIBSSH2_SESSION* m_sshSession;
	LIBSSH2_SFTP *   m_sftp_session;
};

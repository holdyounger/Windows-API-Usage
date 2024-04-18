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
	* 函数名：sftpOpen
	* 说明  ：建立SFTP连接
	* 参数  ：1.ip
	*           SFTP服务器IP
	*       ：2.port
	*           SFTP服务器端口
	*       ：3.username
	*           SFTP用户名
	*       ：4.password
	*           SFTP用户密码
	* 返回   ： 0 成功；1 其它错误
	/************************************************************************/
	int sftpOpen(const char *ip, int port, const char *username, const char *password);

	/************************************************************************
	* 函数名 ： sftpClose
	* 说明   ： 释放资源
	* 参数   ： 无
	* 返回   ： 0 成功；1 其它错误
	/************************************************************************/
	int sftpClose();

	//确保目录存在，没有则创建一下
	int MakeSureDirExist(const char* sftp_dir);

	int sftpOpenFile(const char *sftp_dir, const char *sftpfilename, LIBSSH2_SFTP_HANDLE *& sftp_handle, BOOL bAppendMode);
	int sftpWriteString(LIBSSH2_SFTP_HANDLE * _handle, LPCWSTR lpStr);
	int sftpWriteBytes(LIBSSH2_SFTP_HANDLE * _handle, BYTE* pBuf, DWORD dwBytes);
	int sftpCloseFile(LIBSSH2_SFTP_HANDLE * _handle);

	/************************************************************************
	* 函数名：uploadFile
	* 说明  ：上传数据流
	* 参数
	*       ：1.sftppath
	*           SFTP路径
	*       ：2.fnReadStream pStream
	*           要上传的本地文件的流对象和其读取函数
	*       ：3.sftpfilename
	*           上传到SFTP服务器保存的文件名
	* 返回值：0成功；其它失败（）
	***********************************************************************/
	int sftpUploadStream(PFN_SFTP_READWRITE_STREAM_FUNCTION fnReadStream, void* pStream, const char *sftp_dir, const char *sftpfilename, BOOL bAutoCreateDir);

	/************************************************************************
	* 函数名：downloadFile
	* 说明  ：下载文件
	* 参数
	*       ：1.sftpfile
	*           要下载的SFTP文件全路径，包括文件名
	*       ：2.fnWrite pStream
	*           保存到本地的流
	* 返回值：0成功；其它失败（）
	***********************************************************************/
	int sftpDownloadFile(const char *sftpfile, PFN_SFTP_READWRITE_STREAM_FUNCTION fnWrite, void* pStream, size_t&OUT totalWrite);

private:
	/************************************************************************
	* 函数名：SFTPDirExist
	* 说明  ：判断SFTP目录是否存在
	* 参数  ：1.sftp_session
	*           LIBSSH2_SFTP会话指针
	*       ：2.sftppath
	*           SFTP目录
	* 返回值：1指定目录存在；0指定目录不存在
	***********************************************************************/
	BOOL _sftpDirExist(LIBSSH2_SFTP *sftp_session, const char *sftppath);
	/************************************************************************
	* 函数名：makeSFTPDir
	* 说明  ：创建SFTP目录
	* 参数  ：1.sftp_session
	*           LIBSSH2_SFTP会话指针
	*       ：2.sftppath
	*           SFTP目录
	* 返回值：0成功；-1失败
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

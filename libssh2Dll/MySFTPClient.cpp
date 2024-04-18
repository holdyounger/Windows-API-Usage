// #include <atlstr.h>
#include <string.h>
#include <stdio.h>
#include "MySFTPClient.h"
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Crypt32.lib")

#ifdef _DEBUG
// #pragma comment(lib,"libssh2_d.lib")
// #pragma comment(lib,"MSVCRT.lib")
// #pragma comment(lib,"libcrypto_static.lib")
#pragma comment(lib,"libcrypto32MTd.lib")
#pragma comment(lib,"libssl32MTd.lib")
#pragma comment(lib,"libssh2_static_d.lib")
// #pragma comment(lib,"libssl_static.lib")
#else
// #pragma comment(lib,"libssh2.lib")
#pragma comment(lib,"libssh2_static.lib")
// #pragma comment(lib,"libssl_static.lib")
// #pragma comment(lib,"libcrypto_static.lib")
#pragma comment(lib,"libcrypto32MT.lib")
#pragma comment(lib,"libssl32MT.lib")
#endif // _DEBUG



CMySFTPClient::CMySFTPClient()
{
	m_iOpenStatus = -1;
	m_hostaddr = 0;
	m_sock = INVALID_SOCKET;
	memset(&m_insockaddr, 0, sizeof(m_insockaddr));
	m_sshSession = 0;
	m_sftp_session = 0;
}

CMySFTPClient::~CMySFTPClient()
{
	sftpClose();
}

int CMySFTPClient::sftpOpen(const char *ip, int port, const char *username, const char *password)
{
	m_iOpenStatus = 0;

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_iOpenStatus |= CMySFTPClientSTATUS_OPEN;
	m_hostaddr = inet_addr(ip);

	int rc = libssh2_init(0);
	if (rc != 0)
	{
		OutputDebugStringW(L"sftp客户端初始化失败！");
		return CMySFTPERR_SSHINITFAIL;
	}
	m_iOpenStatus |= CMySFTPClientSTATUS_INIT;
	/*
	 * The application code is responsible for creating the socket
	 * and establishing the connection
	 */
	m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sock == INVALID_SOCKET)
	{
		OutputDebugStringW(L"socket初始化失败！");
		return CMySFTPERR_INVALIDSOCKET;
	}

	m_iOpenStatus |= CMySFTPClientSTATUS_SOCKET;

	m_insockaddr.sin_family = AF_INET;
	m_insockaddr.sin_port = htons(port);
	m_insockaddr.sin_addr.s_addr = m_hostaddr;

	if (connect(m_sock, (struct sockaddr*)(&m_insockaddr), sizeof(struct sockaddr_in)) != 0)
	{
		printf("connect faild!!WSAGetLastError=%d ",WSAGetLastError());
		printf("socket连接到%s:%d失败！", (ip),port);
		return CMySFTPERR_CONNECT;
	}
	/* Create a session instance
	 */
	m_sshSession = libssh2_session_init();
	if (!m_sshSession)
	{
		OutputDebugStringW(L"sftp客户端libssh2_session_init初始化失败！");
		return CMySFTPERR_SESSION_INIT;
	}
	/* Since we have set non-blocking, tell libssh2 we are blocking */
	libssh2_session_set_blocking(m_sshSession, 1);

	/* ... start it up. This will trade welcome banners, exchange keys,
	 * and setup crypto, compression, and MAC layers
	 */
	while ((rc = libssh2_session_handshake(m_sshSession, m_sock)) == LIBSSH2_ERROR_EAGAIN);
	if (rc)
	{
		OutputDebugStringW(L"sftp客户端libssh2_session_handshake初始化失败！");
		return CMySFTPERR_HANDSHAKE;
	}
	m_iOpenStatus |= CMySFTPClientSTATUS_HANDSHAKE;

	/* check what authentication methods are available */
	char * pszUserauthlist = libssh2_userauth_list(m_sshSession, username, strlen(username));

	int auth_pw = 0;
	if (strstr(pszUserauthlist, "password") != NULL) {
		auth_pw |= CMySFTPClientAUTH_PSW;
	}
	if (strstr(pszUserauthlist, "keyboard-interactive") != NULL) {
		auth_pw |= CMySFTPClientAUTH_KBD;
	}
	if (strstr(pszUserauthlist, "publickey") != NULL) {
		auth_pw |= CMySFTPClientAUTH_PUBKEY;
	}

	if (auth_pw & CMySFTPClientAUTH_PSW)
	{
		while ((rc = libssh2_userauth_password(m_sshSession, username, password)) == LIBSSH2_ERROR_EAGAIN);
	}
	//else if (auth_pw & CMySFTPClientAUTH_KBD)
	//{
	//	while ((rc = libssh2_userauth_keyboard_interactive(session, username, &kbd_sign_callback)) == LIBSSH2_ERROR_EAGAIN);
	//}
	//else if (auth_pw & CMySFTPClientAUTH_PUBKEY)
	//{
	//	while ((rc = libssh2_userauth_publickey_fromfile(session, username, keyfile1, keyfile2, password)) == LIBSSH2_ERROR_EAGAIN);
	//}
	if (rc) {
		printf("sftp用户%s密码错误！",(username));
		return  CMySFTPERR_PSWERR;
	}

	//建立ftp会话
	m_sftp_session = libssh2_sftp_init(m_sshSession);
	if (!m_sftp_session) {
		OutputDebugStringW(L"sftp客户端libssh2_sftp_init初始化失败！");
		return CMySFTPERR_SFTP_INIT;
	}
	m_iOpenStatus |= CMySFTPClientSTATUS_FTPSESSION;
	return 0;
}

int CMySFTPClient::sftpClose()
{
	if (m_iOpenStatus & CMySFTPClientSTATUS_FTPSESSION)
	{
		libssh2_sftp_shutdown(m_sftp_session);
		m_sftp_session = 0;
	}

	if (m_iOpenStatus & CMySFTPClientSTATUS_HANDSHAKE)
	{
		libssh2_session_disconnect(m_sshSession, "Normal Shutdown");
		libssh2_session_free(m_sshSession);
		m_sshSession = 0;
	}

	if (m_iOpenStatus & CMySFTPClientSTATUS_SOCKET)
	{
		closesocket(m_sock);
		m_sock = INVALID_SOCKET;
	}

	if (m_iOpenStatus & CMySFTPClientSTATUS_INIT)
	{
		libssh2_exit();
	}

	if (m_iOpenStatus & CMySFTPClientSTATUS_OPEN)
	{
		WSACleanup();
	}
	m_iOpenStatus = 0;
	return 0;
}

int CMySFTPClient::MakeSureDirExist(const char* sftp_dir)
{
	if (!m_sftp_session)
	{
		return CMySFTPERR_SFTP_INIT;
	}
	int rc = 0;
	if (!_sftpDirExist(m_sftp_session, sftp_dir))
		rc = _sftpMakeDir(m_sftp_session, sftp_dir);
	if (rc != 0)
	{
		return CMySFTPERR_SFTP_UPLOADMKDIR;
	}
	return 0;
}

int CMySFTPClient::sftpOpenFile(const char *sftp_dir, const char *sftpfilename, LIBSSH2_SFTP_HANDLE *& sftp_handle, BOOL bAppendMode)
{
	sftp_handle = 0;

	if(!sftpfilename || !sftp_dir)
	{
		return CMySFTPERR_INVALID_FILENAME;
	}

	if(!m_sftp_session)
	{
		return CMySFTPERR_SFTP_INIT;
	}

	char sftp_filefullpath[MAX_PATH] = {};
	/* FTP文件全路径，包括文件名 */
	sprintf_s(sftp_filefullpath, MAX_PATH,
	          sftp_dir[strlen(sftp_dir) - 1] == '/' ? "%s%s" : "%s/%s",
	          sftp_dir,
	          sftpfilename
	         );
	LIBSSH2_SFTP_HANDLE * sftp_handlet = libssh2_sftp_open(m_sftp_session, sftp_filefullpath,
	                                     LIBSSH2_FXF_WRITE | (bAppendMode ? LIBSSH2_FXF_APPEND :  LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC),
	                                     LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
	                                     LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);

	if(!sftp_handlet)
	{
		printf("打开远程路径%s失败！",(sftp_filefullpath));
		return CMySFTPERR_SFTP_SFTP_OPEN;
	}

	sftp_handle = sftp_handlet;
	return 0;
}

int CMySFTPClient::sftpWriteString(LIBSSH2_SFTP_HANDLE * _handle, LPCWSTR lpStr)
{
	if(!lpStr || !_handle)
	{
		return CMySFTPERR_INVALID_FILENAME;
	}

	if(!m_sftp_session)
	{
		return CMySFTPERR_SFTP_INIT;
	}

	int len = wcslen(lpStr);
	int rc = libssh2_sftp_write(_handle, (const char*)lpStr, len * 2);

	if(rc < 0)
		return rc;

	rc = libssh2_sftp_fsync(_handle);
	return rc;
}

int CMySFTPClient::sftpWriteBytes(LIBSSH2_SFTP_HANDLE * _handle, BYTE* pBuf, DWORD dwBytes)
{
	if(!pBuf || !_handle || !dwBytes)
	{
		return CMySFTPERR_INVALID_FILENAME;
	}

	if(!m_sftp_session)
	{
		return CMySFTPERR_SFTP_INIT;
	}

	int rc = libssh2_sftp_write(_handle, (const char*)pBuf, dwBytes);

	if(rc < 0)
		return rc;

	rc = libssh2_sftp_fsync(_handle);
	return rc;
}

int CMySFTPClient::sftpCloseFile(LIBSSH2_SFTP_HANDLE * _handle)
{
	if(!_handle)
	{
		return CMySFTPERR_INVALID_FILENAME;
	}

	if(!m_sftp_session)
	{
		return CMySFTPERR_SFTP_INIT;
	}

	int rc = libssh2_sftp_close(_handle);
	return rc;
}

int CMySFTPClient::sftpUploadStream(PFN_SFTP_READWRITE_STREAM_FUNCTION fnReadStream, void* pStream, const char *sftp_dir, const char *sftpfilename, BOOL bAutoCreateDir)
{
	if (!pStream || !fnReadStream)
	{
		return CMySFTPERR_INVALID_STREAM;
	}

	if (!sftpfilename || !sftp_dir)
	{
		return CMySFTPERR_INVALID_FILENAME;
	}

	if (!m_sftp_session)
	{
		return CMySFTPERR_SFTP_INIT;
	}

	/* 确定目录存在 */
	int rc = 0;

	if (bAutoCreateDir)
	{
		if (!_sftpDirExist(m_sftp_session, sftp_dir))
			rc = _sftpMakeDir(m_sftp_session, sftp_dir);
		if (rc != 0)
		{
			return CMySFTPERR_SFTP_UPLOADMKDIR;
		}
	}

	char sftp_filefullpath[MAX_PATH] = {};
	/* FTP文件全路径，包括文件名 */
	sprintf_s(sftp_filefullpath, MAX_PATH,
		sftp_dir[strlen(sftp_dir) - 1] == '/' ? "%s%s" : "%s/%s",
		sftp_dir,
		sftpfilename
	);

	LIBSSH2_SFTP_HANDLE * sftp_handle = libssh2_sftp_open(m_sftp_session, sftp_filefullpath,
		LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
		LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
		LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);

	if (!sftp_handle) {
		printf("打开远程路径%s失败！", (sftp_filefullpath));
		return CMySFTPERR_SFTP_SFTP_OPEN;
	}

	do
	{
		char memBuffer[1024 * 10] = {};
		int nRead = fnReadStream(pStream, memBuffer, 1, sizeof(memBuffer));
		if (nRead <= 0) {
			/* end of file */
			break;
		}

		char * ptr = memBuffer;
		do
		{
			/* write data in a loop until we block */
			rc = libssh2_sftp_write(sftp_handle, ptr, nRead);
			if (rc < 0)
				break;
			ptr += rc;
			nRead -= rc;

		} while (nRead);

	} while (rc > 0);

	libssh2_sftp_close(sftp_handle);
	return 0;
}

__declspec(noinline) BOOL CMySFTPClient::_sftpDirExist(LIBSSH2_SFTP *sftp_session, const char *sftppath)
{
	LIBSSH2_SFTP_HANDLE *sftp_handle = libssh2_sftp_opendir(sftp_session, sftppath);

	if (sftp_handle)
	{
		libssh2_sftp_closedir(sftp_handle);
	}
	return !!((int)sftp_handle > 0);
}

__declspec(noinline) int CMySFTPClient::_sftpMakeDir(LIBSSH2_SFTP *sftp_session, const char *sftppath)
{
	if (!sftppath)
		return CMySFTPERR_SFTP_INVALIDARG;

	char szSftpPath[MAX_PATH] = {};
	strncpy(szSftpPath, sftppath, sizeof(szSftpPath) - 1);

	const char *seps = "/";
	char *token = strtok(szSftpPath, seps);
	char szNewDir[MAX_PATH] = {};

	while (token != NULL)
	{
		sprintf_s(szNewDir, MAX_PATH, "%s/%s", szNewDir, token);

		if (!_sftpDirExist(sftp_session, szNewDir))
			//if (libssh2_sftp_mkdir(sftp_session, szNewDir, 0755))
			if (libssh2_sftp_mkdir(sftp_session, szNewDir, 0777))
				return CMySFTPERR_HANDSHAKE;
		token = strtok(NULL, seps);
	}
	return 0;
}

int CMySFTPClient::sftpDownloadFile(const char *sftpfile, PFN_SFTP_READWRITE_STREAM_FUNCTION fnWrite, void* pStream, size_t&OUT totalWrite)
{
	if (!pStream || !fnWrite)
		return CMySFTPERR_INVALID_STREAM;

	if (!sftpfile)
		return CMySFTPERR_INVALID_FILENAME;

	if (!m_sftp_session) {
		return CMySFTPERR_SFTP_INIT;
	}

	/* 打开SFTP文件 */
	LIBSSH2_SFTP_HANDLE * sftp_handle = libssh2_sftp_open(m_sftp_session, sftpfile, LIBSSH2_FXF_READ,
		LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
		LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);
	if (!sftp_handle) {
		printf("打开远程路径%s失败！", (sftpfile));
		return CMySFTPERR_SFTP_SFTP_OPEN;
	}

	do
	{
		char memBuffer[1024 * 10] = {};
		int rc = libssh2_sftp_read(sftp_handle, memBuffer, sizeof(memBuffer));
		if (rc > 0) {
			totalWrite += fnWrite(pStream, memBuffer, 1, rc);
		}
		else {
			break;
		}
	} while (1);

	libssh2_sftp_close(sftp_handle);
	return 0;
}


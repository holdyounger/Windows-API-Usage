# ˵��

> SFTP ����

��Ҫ�Լ�� SFTP ������

```cpp
int main(int argc, char* argv[])
{
    CString name = "123.123.exe";
    GetNewFileName("C:\\123\\", name);

    CMySFTPClient sftpClient;
    int r = sftpClient.sftpOpen("192.168.19.129", 22, "test", "123123");
    sftp_write_sliding(argc, argv);
    return 0;
}
```
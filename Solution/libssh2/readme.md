# 说明

> SFTP 测试

需要自己搭建 SFTP 服务器

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
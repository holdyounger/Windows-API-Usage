> 概述：将当前进程伪装成 explorer。进而实现拷贝文件到 system32 目录下。

## 实现逻辑

1. 获取当前进程的PEB块
1. 修改PEB块内容，ImagePathName 和 CommandLine 的值为 `C:\\windows\\explorer.exe`
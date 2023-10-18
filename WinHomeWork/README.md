学习记录



[toc]



## CreateRemoteThread

- CreateRemoteThread
- CreateRemoteThread32To64

32位进程对64位进程的远程注入实现。

### 主要思路

在调用 `CreateRemoteThread` 之前，将 `CreateRemoteThread` 切换回64位的再调用执行。




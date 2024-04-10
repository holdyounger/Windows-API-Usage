# 使用 sspi 登录 windows

> SSPI 登录及相关API使用

sspi 登录的流程相对比较复杂，但是可以通过sspi登录时获取到的凭证来验证当前登录用户的密码。 当前项目仅做登录爆破，未实现当前登录用户的密码校验。

如需要实现相关的功能，可参考项目 [vletoux/DetectPasswordViaNTLMInFlow: Extract the password of the current user from flow (keylogger, config file, ..) Use SSPI to get a valid NTLM challenge/response and test passwords](https://github.com/vletoux/DetectPasswordViaNTLMInFlow)
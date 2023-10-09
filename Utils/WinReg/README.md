---
title: Windows注册表查询工具
---



<h1 align="center">Welcome to WinReg 👋</h1>
<p>
  <a href="https://www.npmjs.com/package/" target="_blank">
    <img alt="Version" src="https://img.shields.io/badge/windows-v0.01-red">
  </a>
</p>

> windows 注册表查询封装函数

## Author

👤 **mingming**

* Github: [@holdyounger](https://github.com/holdyounger)

## Show your support

Give a ⭐️ if this project helped you!

# Windows 注册表工具

用于windows系统查询注册表，具体方法查看代码

## 文件

[WinReg.h](./WinReg.h)
[WinReg.cpp](./WinReg.cpp)


## 使用Demo

```cpp
// 遍历网卡
std::set<std::string> get_tap_reg()
{
	std::set<std::string> tap_reg;

	try
	{
		WinRegKey reg_key;
		if (!reg_key.open_exist(HKLM, "SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}"))
		{
			throw("open regedit %s faild: %s",
				_adapter_key, reg_key.get_error_msg());
		}

		vector<string> sub_keys = reg_key.enum_subkeys();
		assert(!sub_keys.empty(), "%s has no child keys");
		//iterator subkeys
		for (auto& iter : sub_keys)
		{
			char connection_string[256];
			sprintf(connection_string, sizeof(connection_string),
				"%s\\%s", _adapter_key, iter.c_str());
			WinRegKey sub_reg_key;

			if (!sub_reg_key.open_exist(HKLM, connection_string))
			{
				printf("Open_exist %s failed: %s",
					connection_string, sub_reg_key.get_error_msg());
				continue;
			}
			string component_id;

			if (!sub_reg_key.read_key_value("ComponentId", component_id))
			{
				printf("Read %s\\ComponentId failed: %s",
					connection_string, sub_reg_key.get_error_msg());
				continue;
			}
			string net_cfg_instance_id;

			if (!sub_reg_key.read_key_value("NetCfgInstanceId", net_cfg_instance_id))
			{
				printf("Read %s\\NetCfgInstanceId failed: %s",
					connection_string, sub_reg_key.get_error_msg());
				continue;
			}

			if (strcmp(component_id.c_str(), _component_id) == 0)
			{
				tap_reg.insert(net_cfg_instance_id);
			}
		}

		reg_key.~WinRegKey();
		return tap_reg;
	}
	catch (exception& e)
	{
		printf("%s", e.what());
		return tap_reg;
	}
}
```

***
_This README was generated with ❤️ by [readme-md-generator](https://github.com/kefranabg/readme-md-generator)_
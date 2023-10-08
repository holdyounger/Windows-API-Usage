// GetDominName.cpp : 获取主机域名
//

#include <Windows.h>
#include <DSRole.h>

#pragma comment(lib, "netapi32.lib")

#include <stdio.h>

int main(int argc, char ** argv)
{
	DSROLE_PRIMARY_DOMAIN_INFO_BASIC * info;
	DWORD dw;

	dw = DsRoleGetPrimaryDomainInformation(NULL,
		DsRolePrimaryDomainInfoBasic,
		(PBYTE *)&info);
	if (dw != ERROR_SUCCESS)
	{
		wprintf(L"DsRoleGetPrimaryDomainInformation: %u\n", dw);
		return dw;
	}

	if (info->DomainNameDns == NULL)
	{
		wprintf(L"DomainNameDns is NULL\n");
	}
	else
	{
		wprintf(L"DomainNameDns: %s\n", info->DomainNameDns);
	}

	system("pause");

	return 0;
}

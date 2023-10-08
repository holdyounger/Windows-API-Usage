#include "define.h"


extern int GetAbailablePort();
extern int GetIpForwardTableTest();
extern int GetIsDeleteDftRoute();
extern int GetNetInfo();
extern int GetPort();
extern int GetTcpStatisticsTest();
extern int IpConvert();



int main(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp)
{

	// IpConvert();

	GetNetInfo();


	system("pause");

	return 0;
}
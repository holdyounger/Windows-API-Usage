#include <iostream>
#include "attribute.h"
using namespace std;

int main()
{
	std::string  RetStr;
	BaseFlow::Attribute::GetFileDescription("D:\\Program Files\\Typora\\Typora.exe", RetStr);
	cout << RetStr << endl;
	BaseFlow::Attribute::GetFileVersion("D:\\Program Files\\Typora\\Typora.exe", RetStr);
	cout << RetStr << endl;
	BaseFlow::Attribute::GetInternalName("D:\\Program Files\\Typora\\Typora.exe", RetStr);
	cout << RetStr << endl;
	BaseFlow::Attribute::GetCompanyName("D:\\Program Files\\Typora\\Typora.exe", RetStr);
	cout << RetStr << endl;
	BaseFlow::Attribute::GetLegalCopyright("D:\\Program Files\\Typora\\Typora.exe", RetStr);
	cout << RetStr << endl;
	BaseFlow::Attribute::GetOriginalFilename("D:\\Program Files\\Typora\\Typora.exe", RetStr);
	cout << RetStr << endl;
	BaseFlow::Attribute::GetProductName("D:\\Program Files\\Typora\\Typora.exe", RetStr);
	cout << RetStr << endl;
	BaseFlow::Attribute::GetProductVersion("D:\\Program Files\\Typora\\Typora.exe", RetStr);
	cout << RetStr << endl;
	system("pause");
}
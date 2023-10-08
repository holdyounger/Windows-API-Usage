// regex.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <string>
#include <regex>
#include <fstream>
using namespace std;

unsigned char FromHex(unsigned char x) {
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else return 0;
	return y;
}

string UrlDecode(const string& str) {
	string dst = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++) {
		if (str[i] == '+') {
			dst += ' ';
		}
		else if (i + 2 < length && str[i] == '%') {
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			dst += high * 16 + low;
		}
		else {
			dst += str[i];
		}
	}
	return dst;
}

std::string readFile(string filePath)
{
	ifstream fin;
	fin.open(filePath, ios::in);
	if (!fin.is_open())
	{
		cout << "无法找到这个文件！" << endl;
		return "";
	}

	string strRead;
	string temp;
	while (getline(fin, temp))
	{
		fin >> temp;
		strRead += temp;
		temp.clear();
	}
	cout << "[read]:\n" << strRead << endl;

	fin.close();
	return strRead;
}

// 日志脱敏
std::string serviceDesensitize(std::string strJson)
{
	std::smatch base_match;
	string reg("\\\"username\\\"\:(.)?\\\"\[a-zA-Z0-9]\*\\\"");
	string reg_sub_name("\\\"sub_name\\\"\:(.)?\\\"\[a-zA-Z0-9]\*\\\"");
	std::regex pattern(reg);
	std::regex pattern_sub_name(reg_sub_name);

	std::regex_match(strJson, base_match, pattern);
	if (base_match.size() == 2) {
		std::string base = base_match[1].str();
		std::cout << "sub-match[0]: " << base_match[0].str() << std::endl;
		std::cout << "sub-match[1]: " << base << std::endl;
	}

	cout << "[正则内容]：" << reg << endl;
	cout << "[匹配的个数]：" << std::regex_search(strJson, pattern) << endl;
	string strV1 = regex_replace(strJson, pattern_sub_name, "\"sub_name\": \"*******\"");
	return regex_replace(strV1, pattern, "\"username\": \"*******\"");
}
// 日志脱敏
std::string serviceDesensitize1(std::string strJson)
{
	std::string reg_username("\\\"username\\\"\:(.)?\\\"\[a-zA-Z0-9]\*\\\"");
	std::string reg_name("\\\"name\\\"\:(.)?\\\"\[a-zA-Z0-9]\*\\\"");
	std::string reg_sub_name("\\\"sub_name\\\"\:(.)?\\\"\[a-zA-Z0-9]\*\\\"");

	std::regex pattern_username(reg_username);
	std::regex pattern_name(reg_name);
	std::regex pattern_sub_name(reg_sub_name);

	std::string strV1 = regex_replace(strJson, pattern_username, "\"username\": \"*******\"");
	std::string strV2 = regex_replace(strV1, pattern_name, "\"name\": \"*******\"");
	return regex_replace(strV2, pattern_sub_name, "\"sub_name\": \"*******\"");
}

void ReadFileToRegex()
{
	string cnt = readFile("regexTest.txt");
	cout << "[正则]：\n" << serviceDesensitize1(cnt);


	return ;
	std::string fnames[] = { "foo.txt", "bar.txt", "test", "a0.txt", "AAA.txt" };
	// 在 C++ 中 \ 会被作为字符串内的转义符，为使 \. 作为正则表达式传递进去生效，需要对 \ 进行二次转义，从而有 \\.
	std::regex txt_regex("[a-z]+\\.txt");
	for (const auto& fname : fnames)
		std::cout << fname << ": " << std::regex_match(fname, txt_regex) << std::endl;

	string str = "\"USER_PWD\", \"username\": \"shimingming\"}]";
	regex pattern("\"username\": \".*\"");
	cout << regex_replace(str, pattern, "\"username\": \"\"") << endl;	//输出：_2018，将Hello替换为""
	cout << regex_replace(str, pattern, "\"username\": \"*******\"") << endl;	//输出：Hi_2018，将Hello替换为Hi

	//	chat to string
	char num[] = "123456";
	string strnum(num);
}


// 匹配URL编解码
bool isEncodeUrl(string url)
{
	cout << "Url Origin:" << url << endl;
	smatch result;
	string reg("[^a-zA-Z0-9\\\.%\\\+-!@#&\\\*\\\(\\\)_\\\+-=\\\?]*");
	string reg1("%{1}[a-zA-Z0-9]{1,3}");
	cout << "reg" << reg << endl;
	std::regex pattern(reg1);
	std::regex_match(url, result, pattern);

	//迭代器声明
	string::const_iterator iterStart = url.begin();
	string::const_iterator iterEnd = url.end();
	string temp;
	while (std::regex_search(iterStart, iterEnd, result, pattern))
	{
		temp = result[0];
		string decode = UrlDecode(temp);
		cout << temp << " " << decode;
		url.replace(url.find_first_of(temp), temp.length(), decode);
		iterStart = result[0].second;	//更新搜索起始位置,搜索剩下的字符串

		cout << endl << "Url:" << url << endl;
	}

	cout << endl << "Url:" << url << endl;

	return true;
}

string isEncodeUrl_1(string url)
{
	std::string reg("%{1}[a-zA-Z0-9]{1,3}");
	std::regex pattern(reg);
	std::smatch result;

	//迭代器声明
	std::string::const_iterator iterStart = url.begin();
	std::string::const_iterator iterEnd = url.end();
	std::string temp;
#if 0
	while (std::regex_search(iterStart, iterEnd, result, pattern))
	{
		temp = result[0];
		std::string decode = UrlDecode(temp);
		url.replace(url.find_first_of(temp), temp.length(), decode);
		iterStart = result[0].second;	//更新搜索起始位置,搜索剩下的字符串
		iterStart = url.begin();
	}
#endif

	while (std::regex_search(iterStart, iterEnd, result, pattern))
	{
		cout << result[0] << endl;
		url = std::regex_replace(url, pattern, UrlDecode(result[0]), std::regex_constants::format_first_only);
		iterStart = url.begin();
		iterEnd = url.end();
	}

	return url;
}

#define URL "https://10.92.4.117:8443/tac/download/client/com.qianxin.trustagent.win.standard.exe.bm-3.1.1.4585/TrustAgent_standard_3.1.1.4585.aef031d09%5B10.92.4.117%40443%5D.exe"
#define URL1 "!@#$%25%5E&*()_+-=?%5C/"
int main() {
	
	cout << isEncodeUrl_1(string(URL)) << endl;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

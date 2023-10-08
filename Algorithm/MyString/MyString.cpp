/********************************************************************************
 *
 *        Copyright (C) 2021  Beijing winicssec Technology
 *        All rights reserved
 *
 *        filename		: myString.cpp
 *        description	:
 *
 *        created by  mingming.shi on Jul. 15th, 2021
 *
 ********************************************************************************/
#include <iostream>
#include <assert.h>
#include <vld.h>
#include "mystring.h"
using namespace std;

char *LTrim(char *str);
char *RTrim(char *str);

/*
* @fn           RTrim
* @brief        去除尾部多余的空格
* @param[in]    str:需要被操作的字符串
* @param[out]   
* @return       char *:返回字符串
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
char *RTrim(char *str)
{
	if (str == NULL || *str == '\0')
	{
		return str;
	}

	int len = strlen(str);
	char *p = str + len - 1;
	while (p >= str  && isspace(*p))
	{
		*p = '\0';
		--p;
	}

	return str;
}

/*
* @fn           LTrim
* @brief        去除首部多余的空格
* @param[in]    str:需要被操作的字符串
* @param[out]   
* @return       char *:返回字符串
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
char *LTrim(char *str)
{
	if (str == NULL || *str == '\0')
	{
		return str;
	}

	int len = 0;
	char *p = str;
	while (*p != '\0' && isspace(*p))
	{
		++p;
		++len;
	}

	memmove(str, p, strlen(str) - len + 1);

	return str;
}

/*
* @fn           MyString
* @brief        构造函数
* @param[in]    
* @param[out]   
* @return       MyString对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
MyString::MyString(void)
{
	this->m_str = new char[1];
	m_str[0] = '\0';
}


MyString::MyString(const char *s)
{
	if(s == NULL)
	{
		this->m_str = new char[1];
		m_str[0] = '\0';
	}
	else
	{
		m_str = new char[strlen(s) + 1];
		strcpy(this->m_str, s);
	}
}

MyString::MyString(const MyString &s)
{
	if( s.m_str == NULL)
	{
		this->m_str = new char[1];
		m_str[0] = '\0';
	}
	else
	{
		m_str = new char[strlen(s.m_str) + 1];
		strcpy(this->m_str, s.m_str);
	}
}

/*
* @fn           析构函数
* @brief        
* @param[in]    
* @param[out]   
* @return       
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
MyString::~MyString(void)
{
	if (this->m_str != NULL)
	{
		delete[] this->m_str;
	}
}

/*
* @fn           operator+
* @brief        重载 + 操作符
* @param[in]    需要相加的MyString对象
* @param[out]   
* @return       MyString: 返回相加结果 MyString 对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
MyString MyString::operator+(const MyString &s)
{
	MyString p;
	delete [] p.m_str;
	p.m_str = new char[strlen(this->m_str) + strlen(s.m_str) + 1];
	strcpy(p.m_str, this->m_str);
	strcat(p.m_str, s.m_str);
	return p;
}

/*
* @fn           operator=
* @brief        重载 = 操作符
* @param[in]    源 MyString 对象
* @param[out]   
* @return       MyString: 返回深拷贝 MyString 对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
MyString& MyString::operator=(const MyString &s)
{
	int len = strlen(s.m_str) + 1;
	if (this == &s)
	{
		return *this;
	}
	if (this->m_str != NULL)
	{
		delete[] this->m_str;
	}
	this->m_str = new char[len];
	strcpy(this->m_str, s.m_str);
	return *this;
}

/*
* @fn           operator[]
* @brief        重载[]操作符
* @param[in]    需要相加的MyString对象
* @param[out]   
* @return       MyString: 返回相加结果 MyString 对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
char MyString::operator[](int i)
{
	int len = strlen(this->m_str) - 1;
	if (i > len)
	{
		return ('\0');
	}
	else
	{
		return (this->m_str[i]);
	}
}

/*
* @fn           operator+
* @brief        重载+操作符
* @param[in]    需要相加的MyString对象
* @param[out]   
* @return       MyString: 返回相加结果 MyString 对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
bool MyString::operator<(const MyString &s)
{
	int ret;
	ret = strcmp(this->m_str, s.m_str);

	if (ret < 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
* @fn           operator>
* @brief        重载 > 操作符
* @param[in]    s: 与当前对象计算的 MyString 对象
* @param[out]   
* @return       true/false: 如果当前对象大于 s 返回 true；反之返回 false。
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
bool MyString::operator>(const MyString &s)
{
	int ret;
	ret = strcmp(this->m_str, s.m_str);
	if (ret > 0)
		return true;
	else
		return false;
}

/*
* @fn           operator==
* @brief        重载==操作符
* @param[in]    s: 需要与当前对象计算的 MyString 对象
* @param[out]   
* @return       true/false: 如果不相等返回 false；如果相等返回 true。
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
bool MyString::operator==(const MyString &s)
{
	int ret;
	ret = strcmp(this->m_str, s.m_str);
	if (ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
* @fn           operator!=
* @brief        重载 != 操作符
* @param[in]    s: 需要与当前对象计算的 MyString 对象
* @param[out]   
* @return       true/false: 如果不相等返回 true；如果相等返回false。
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
bool MyString::operator!=(const MyString &s)
{
	int ret;
	ret = strcmp(this->m_str, s.m_str);
	if (ret != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/*
* @fn           toLower
* @brief        将字符串中的大写字母变成小写
* @param[in]    
* @param[out]   
* @return       myString对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
MyString MyString::ToLower(void)
{
	char * temp = this->m_str;
	while (*temp != '\0')
	{
		if(*temp <= 'Z' && *temp >= 'A')
		{
			*temp += 32;
			temp++;
		}
		else
		{
			temp++;
		}

	}
	return *this;
}

/*
* @fn           toLower
* @brief        将字符串中的小写字母变成大写
* @param[in]    
* @param[out]   
* @return       myString对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
MyString MyString::ToUpper(void)
{
	char * temp = this->m_str;
	while (*temp != '\0')
	{
		if(*temp <= 'z' && *temp >= 'a')
		{
			*temp -= 32;
			temp++;
		}
		else
		{
			temp++;
		}

	}
	return *this;
}

/*
* @fn           findStr
* @brief        查找str在myString对象中首次出现的位置
* @param[in]    str	:需要查找的子符串
* @param[out]   
* @return       如果查找到，则返回首次查找到时的下标；如果没有找到，则返回-1
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
int MyString::FindStr(const char* str)
{
	// 双指针
	for (size_t i = 0; i < strlen(m_str); i++)
	{
		size_t j = 0;
		if (this->m_str[i] == str[j])
		{
			while(i < strlen(m_str) 
				&& j < strlen(str) 
				&& this->m_str[i+j] == str[j])
			{
				if(j == (strlen(str)-1))
					return i;
				else
					j++;

			}
		}

	}
	return -1;
}

/*
* @fn           getStr
* @brief        从指定位置开始返回myString对象中指定长度的子串，如果没有则返回NULL
* @param[in]    n	:开始位置
				len	:从开始位置计算的长度
* @param[out]   
* @return       myString对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
MyString MyString::GetStr(const int& n,const size_t& len)
{
	assert(m_str[n] != NULL);

	char *p = this->m_str;

	MyString ret;
	delete [] ret.m_str;	// 必须释放当临时成员空间，不然会造成泄漏
	ret.m_str = new char[len+1];

	size_t i = 0;
	while (i < len)
	{
		assert(m_str[n+i] != NULL);
		ret.m_str[i] = p[n+i]; 
		i++;
	}
	ret.m_str[len] = '\0';
	
	return ret;
}

int MyString::Length(void)
{
	return strlen(m_str);
}

/*
* @fn           RemoveLRSpace
* @brief        移除首尾空格
* @param[in]    
* @param[out]   
* @return       myString对象
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
MyString MyString::RemoveLRSpace(void)
{
	if(m_str == NULL)
		return *this;
	RTrim(m_str);
	LTrim(m_str);
	return *this;
}

/*
* @fn           Compare
* @brief        比较两个MyString对象是否相等
* @param[in]    
* @param[out]   
* @return       int:如果this小于s，返回-1；
					如果this等于s，返回0；
					如果this大于s, 返回1。
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-15
*/
int MyString::Compare(const MyString &s)
{
	assert(m_str != NULL && s.m_str != NULL);

	int ret;
	ret = strcmp(this->m_str, s.m_str);

	return ret;
}


//左移运算符重载
ostream& operator<<(ostream &out, MyString &s)
{
	out << s.m_str;
	return out;
}

//右移运算符重载
istream& operator>>(istream &in, MyString &s)
{
	char p[50];
	in.getline(p, 50);
	s = p;
	return in;
}
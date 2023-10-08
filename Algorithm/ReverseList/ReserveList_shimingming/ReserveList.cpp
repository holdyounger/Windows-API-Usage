/********************************************************************************
 *
 *        Copyright (C) 2021  Beijing winicssec Technology
 *        All rights reserved
 *
 *        filename :      ReverseList.CPP
 *        description :    反转链表
 *
 *        created by  mingming.shi on Jul. 12th, 2021
 *
 ********************************************************************************/

#include <iostream>
#include <string>
#include <vld.h>
using namespace std;


struct ListNode {
	string str;
	ListNode *next;
	ListNode(void):str(" "),next(NULL){}
	ListNode(string x):str(x),next(NULL){}
};

ListNode* InitListNode(string str);
ListNode* ReverseList(ListNode* phead);
void PrintListNode(ListNode* phead);
bool IsNum(string str);
void Destory(ListNode* phead);

/*
* @fn           InitListNode
* @brief        根据字符串生成单链表   
* @param[in]    str :输入的以空格为分隔符的字符串
* @param[out]   
* @return       listNode* :链表结构体的头结点
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-12
*/
ListNode* InitListNode(string str)
{
	ListNode *head = new ListNode;
	ListNode *pf = head;

	for (size_t i =0; i < str.size(); ++i)
	{
		string tempStr;
		while(str[i] != ' ')
		{
			tempStr += str[i++];
			if(i >= str.size())
				break;
		}

		ListNode* tempNode = new ListNode(tempStr);
		pf->next = tempNode;
		pf = pf->next;
	}
	ListNode* tempNode = head->next;
	delete head;
	return tempNode;
}

/*
* @fn           ReverseList
* @brief        反转链表  
* @param[in]    phead:需要反转的链表头结点
* @param[out]   
* @return       listNode* :反转后链表结构体的头结点
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-12
*/
ListNode* ReverseList(ListNode* phead)
{
	ListNode* cur = phead;
	ListNode* pre = NULL;
	ListNode* nex = NULL;
	while(cur)
	{
		nex = cur->next;
		cur->next = pre;
		pre = cur;
		cur = nex;
	}
	return pre;
}

/*
* @fn           PrintListNode
* @brief        根据字符串生成单链表   
* @param[in]    phead:需要打印的链表头结点
* @param[out]   
* @return       void
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-12
*/
void PrintListNode(ListNode* phead)
{
	if(phead->str.empty())
	{
		return;
	}

	ListNode *cur = phead;
	while (cur->next != NULL)
	{
		cout << cur->str << ",";
		cur = cur->next;
	}
	cout << cur->str << "."<< endl;
}

/*
* @fn           IsNum
* @brief        判断string是否为纯数字格式   
* @param[in]    str :字符串
* @param[out]   
* @return       True/False
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-12
*/
bool IsNum(string str)
{
	if(str.empty())
	{
		return false;
	}

	for (size_t i=0; i<str.size(); i++ )
	{
		int temp = (int)str[i];
		
		if(temp >='0' && temp <='9'){
			continue;
		}
		else
		{
			return false;
		}
	}
	return true;

}

/*
* @fn           Str2Reverselist
* @brief		将输入的字符串转换为列表并输出反转   
* @param[in]    str :字符串
* @param[out]   
* @return
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-12
*/
void Str2Reverselist(string str)
{
	ListNode *list = InitListNode(str);
	cout << "输入序列为:";
	PrintListNode(list);

	ListNode *retList = ReverseList(list);
	cout << "反转序列为:";
	PrintListNode(retList);

	Destory(retList);
	//Destory(retList);
}

/*
* @fn           Destory
* @brief		释放链表内存   
* @param[in]    phead :需要被释放的链表头结点
* @param[out]   
* @return		void
*               
* @detail      
* @author       mingming.shi
* @date         2021-7-12
*/
void Destory(ListNode* phead)
{
	if(phead == NULL)
	{
		return;
	}

	ListNode *p=phead;
	ListNode *q;

	while(p != NULL)
	{
		q=p->next;
		delete p;
		p=q;
	}
	p = NULL;
}


// 入口函数
int main(int argc, const char* argv[])
{

	string strList,numList,letterList;
	
	/*不使用命令行方式*/
	//while(cin)
	//{
	//		string str;
	//		cin >> str;
	//		strList += str;
	//		strList += " ";

	//		if(IsNum(str) == true)
	//		{
	//			numList += str;
	//			numList += " ";
	//		}
	//		else
	//		{
	//			letterList += str;
	//			letterList += " ";
	//		}

	//		if(getchar() == '\n')
	//		{
	//			break;
	//		}

	//}


	/*使用命令行方式*/
	for(int i = 1; i < argc; ++i)
	{
		//cout << argv[i] << '\t';
		strList += argv[i];
		strList += " ";
		if(IsNum(argv[i]) == true)
		{
			numList += argv[i];
			numList += " ";
		}
		else
		{
			letterList += argv[i];
			letterList += " ";
		}
	}
	
	Str2Reverselist(strList);
	Str2Reverselist(numList);
	Str2Reverselist(letterList);

	system("pause");

}
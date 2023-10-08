/********************************************************************************
 *
 *        Copyright (C) 2021  Beijing winicssec Technology
 *        All rights reserved
 *
 *        filename :      main.cpp
 *        description :    反转链表入口函数
 *
 *        created by  mingming.shi on Jul. 12th, 2021
 *
 ********************************************************************************/
#include "MyList.h"

void Str2Reverselist(char* str);

int main(int argc, const char* argv[])
{

	char *inputStr;
	char *strList,*numList = NULL,*letterList = NULL;
	char *p;
	
	inputStr = (char*)malloc(sizeof(char)*100);

	/*不使用命令行方式*/
	gets(inputStr);
	strList = (char*)malloc(strlen(inputStr)+1); // 为strlist申请空间
	strcpy(strList,inputStr);
	

	p = strtok(inputStr, " ");
	while(p)
	{    
		if(IsNum(p))
		{
			if(numList == NULL)
			{
				numList = (char*)malloc(strlen(p) + 1);
				strcpy(numList,p);
			}
			else
			{
				char* temp = (char*)malloc(strlen(p)+1);
				strcpy(temp,p);
				strcat(numList," ");
				strcat(numList,temp);
			}

		}
		else
		{
			if(letterList == NULL)
			{
				letterList = (char*)malloc(strlen(p) + 1);
				strcpy(letterList,p);
			}
			else
			{
				char* temp = (char*)malloc(strlen(p)+1);
				strcpy(temp,p);
				strcat(letterList," ");
				strcat(letterList,temp);
			}
		}
		p = strtok(NULL, " ");  
	}

	free(inputStr);
	
	/*使用命令行方式*/
	//for(int i = 1; i < argc; ++i)
	//{
	//	//cout << argv[i] << '\t';
	//	strcat(strList,argv[i]);
	//	strcat(strList," ");
	//	if(IsNum((char*)argv[i]) == true)
	//	{
	//		strcat(numList,argv[i]);
	//		strcat(numList," ");
	//	}
	//	else
	//	{
	//		strcat(letterList,argv[i]);
	//		strcat(letterList," ");
	//	}
	//}



	Str2Reverselist(strList);
	
	//Str2Reverselist(numList);

	//Str2Reverselist(letterList);


	free(strList);
	free(numList);
	free(letterList);

	printf("end\n");

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
void Str2Reverselist(char* str)
{
	ListNode *list = InitListNode(str);
	cout << "输入序列为:";
	PrintListNode(list);

	ListNode *numList,*letterList;
	numList = (ListNode *)malloc(sizeof(ListNode));
	letterList = (ListNode *)malloc(sizeof(ListNode));

	ListSeparation(list,numList,letterList);

	ListNode *retList = ReverseList(list);
	cout << "反转序列为:";
	PrintListNode(retList);

	Destory(numList);
	Destory(letterList);
	Destory(retList);
}
/********************************************************************************
 *
 *        Copyright (C) 2021  Beijing winicssec Technology
 *        All rights reserved
 *
 *        filename		: main.cpp
 *        description	: ≤‚ ‘»Îø⁄
 *
 *        created by  mingming.shi on Jul. 13th, 2021
 *
 ********************************************************************************/
#include <iostream>
#include "myString.h"
int main(int argc, const char **argv)
{
	MyString m1("hello");
	MyString m2("WORLD£°");
	MyString m3;

	m3 = m1 + " " + m2;
	cout << "m3 = " << m3 << endl;

	MyString m4(m3);
	cout << "m4 = " << m4 << endl;

	cout << "m4.length() = " << m4.Length() << endl;	// ok
	cout << "m4[1] = " << m4[1] << endl;						// ok
	cout << "m4.toLower()" << m4.ToLower() << endl;				// ok
	cout << "m4.toUpper()" << m4.ToUpper() << endl;				// ok
	MyString ans = m4.GetStr(1,2);				// ok
	cout << "m4.getStr(1,2) = " << ans << endl;
	int pos = m4.FindStr("WOR");				// ok
	cout << "m4.findStr(\"WOR\"):" << pos << endl;
	cout << "(m1  < m2)" << (m1 < m2) << endl;
	cout << "(m1  > m2)" << (m1 > m2) << endl;
	cout << "(m1 == m3)" << (m1 == m3) << endl;

	MyString m5 = "  space  ";
	m5.RemoveLRSpace();
	cout << m5 << endl;
	
	return 0;


}
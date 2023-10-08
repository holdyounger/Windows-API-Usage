#pragma once
class UseLessClass
{
public:
	UseLessClass();
	~UseLessClass();

public:
	int x;
	int y;
	friend bool operator<(UseLessClass a, UseLessClass b);

private:

};

class MyCompare {

	friend bool operator<(UseLessClass a, UseLessClass b)
	{
		return a.x < b.x;
	}
};
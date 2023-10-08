#pragma once

class emptyBaseClass
{
public:

	void start() {};

	virtual void add() {};

private:
	int _val;

};

class emptyChdClass : public emptyBaseClass
{
public:
	void add() {};

public:
	emptyChdClass();
	~emptyChdClass();

private:

};


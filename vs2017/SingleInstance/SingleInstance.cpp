#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;
class Printer {
public:
	static Printer * getinstance() {
		return instance;
	}
	void print(string text) {
		cout << "打印的内容为：" << text << endl;
		sequence++;
		cout << "打印机今天使用了" << sequence << "次" << endl;
	}
	static int getSequence() {
		return sequence;
	}
private:
	// 定义一个类去析构单例
	class Garbo {
	public:
		~Garbo() {
			if (instance != NULL) {
				printf("调用了析构~~~");
				delete instance;
			}
		}
	};

	static Garbo garbo; // 在静态区域开辟一个对象，Garbo
	static int sequence;
	static Printer * instance;
};

int Printer::sequence = 0;
Printer *Printer::instance = new Printer;
Printer::Garbo Printer::garbo;

int main()
{
	//三个peple使用打印机
	Printer * p1 = Printer::getinstance();
	p1->print("亿封简历");

	Printer *p2 = Printer::getinstance();
	p2->print("一份工作汇报");

	Printer *p3 = Printer::getinstance();
	p3->print("离职申请");

	system("pause");
	return 0;
}
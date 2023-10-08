// multimap.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

// Using a multimap
#include <iostream>                                        // For standard streams
#include <string>                                          // For string class
#include <map>                                             // For multimap container
#include <cctype>                                          // For toupper()

using std::string;
using Pet_type = string;
using Pet_name = string;

int main()
{
	std::multimap<Pet_type, Pet_name> pets;
	Pet_type type{};
	Pet_name name{};
	char more{ 'Y' };
	while (std::toupper(more) == 'Y')
	{
		std::cout << "Enter the type of your pet and its name: ";
		std::cin >> std::ws >> type >> name;

		// Add element - duplicates will be LIFO
		auto iter = pets.lower_bound(type);
		if (iter != std::end(pets))
			pets.emplace_hint(iter, type, name);
		else
			pets.emplace(type, name);
		std::cout << "Do you want to enter another(Y or N)? ";
		std::cin >> more;
	}

	// Output all the pets
	std::cout << "\nPet list by type:\n";
	auto iter = std::begin(pets);
	while (iter != std::end(pets))
	{
		auto pr = pets.equal_range(iter->first);
		std::cout << "\nPets of type " << iter->first << " are:\n";
		for (auto p = pr.first; p != pr.second; ++p)
			std::cout << "  " << p->second;
		std::cout << std::endl;
		iter = pr.second;
	}
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

#include <iostream>
#include <Windows.h>


// 定义一个使用 stdcall 调用约定的函数
extern "C" {
    void __stdcall MyStdCallFunction(int param1, int param2, int param3);
}

int main() {
    // 调用函数，参数的顺序是 param3, param2, param1
    MyStdCallFunction(1, 2, 3);

    return 0;
}

// 假设 MyStdCallFunction 的实现如下：
extern "C"  void __stdcall MyStdCallFunction(int param1, int param2, int param3) {
    // 函数实现，参数在使用后需要自行从堆栈中清除
    std::cout << "Param1: " << param1 << std::endl;
    std::cout << "Param2: " << param2 << std::endl;
    std::cout << "Param3: " << param3 << std::endl;

    int a1 = 0, a2 = 0, a3 = 0;

    // 清除堆栈上的参数（从右向左）
     _asm {
         mov eax, [ebp + 8]     // 获取 param3 的值
         mov a1, eax            // 获取 param3 的值
         mov eax, [ebp + 12]     // 获取 param2 的值
         mov a2, eax            // 获取 param2 的值
         mov eax, [ebp + 16]     // 获取 param1 的值
         mov a3, eax            // 获取 param2 的值
     }

     // 函数体
     std::cout << "Param1: " << a1 << std::endl;
     std::cout << "Param2: " << a2 << std::endl;
     std::cout << "Param3: " << a3 << std::endl;

    // 现代编译器会处理这些细节，上面的汇编代码只是为了说明原理
}

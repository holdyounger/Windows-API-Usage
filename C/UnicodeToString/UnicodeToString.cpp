#include <iostream>
using namespace std;

int main() {
    string jsonStr = "{\"share_names\":[\"\u4e2d\u6587\u8def\u5f84\"]}";

    cout << jsonStr << endl;

    cout << "กิ is the same as \u2261" << endl;
    string s("กิ is the same as \u2261");
    cout << s << endl;
}
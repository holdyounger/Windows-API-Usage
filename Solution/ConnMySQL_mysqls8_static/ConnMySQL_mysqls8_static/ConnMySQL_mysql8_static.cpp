#include <iostream>
#include <string>
#include <list>
#include <cstdlib>

#include <mysqlx/xdevapi.h>

using namespace mysqlx;
#ifdef _DEBUG
#pragma comment(lib, "mysqlcppconn8-static-debug-openssl3-mtd.lib")    
#pragma comment(lib, "dnsapi.lib")
#else
#pragma comment(lib, "mysqlcppconn8-static-release-openssl3-mt-new.lib")
#pragma comment(lib, "dnsapi.lib")
#endif // DEBUG
// #pragma comment(lib, "dnsapi.lib")
int main() {
    // 修改控制台编码
    // std::system("chcp 65001");

    std::string username;
    std::string password;

    std::cout << "username:";
    std::cin >> username;
    std::cout << "password:";
    std::cin >> password;

    try {
        // 改成你的信息b
        Session sess(SessionOption::USER, username,
            SessionOption::PWD, password,
            SessionOption::HOST, "localhost",
            SessionOption::PORT, 33060,
            SessionOption::DB, "mysql");

        auto result = sess.sql("select * from user").execute();

        for (auto row : result.fetchAll()) {
            std::cout << row[0] << " " << row[1] << " " << row[2] << "\n";
        }

    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    system("pause");
}


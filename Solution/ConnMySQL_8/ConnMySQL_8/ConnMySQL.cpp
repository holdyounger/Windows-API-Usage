#include <stdlib.h>
#include <iostream>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

#define STATIC_CONCPP 1


using namespace std;

#pragma comment(lib, "mysqlcppconn8-static.lib")

//void ConnectMySQL()
//{
//    const char* url = "mysqlx://root@127.0.0.1";
//
//    cout << "Creating session on " << url
//        << " ..." << endl;
//
//    Session sess(url);
//}

//void ConnectMySQl_1()
//{
//    sql::mysql::MySQL_Driver* driver;
//    sql::Connection* con;
//
//    driver = sql::mysql::get_mysql_driver_instance();
//    con = driver->connect("tcp://127.0.0.1:3306", "user", "password");
//
//    delete con;
//}

void standalone_example()
{
	const string server = "tcp://localhost:3306";
	const string username = "root1";
	const string password = "Admin@2022";

	sql::Driver* driver;
	sql::Connection* con;

	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}
}

int main() {
    standalone_example();

    system("pause");

    return 0;
}
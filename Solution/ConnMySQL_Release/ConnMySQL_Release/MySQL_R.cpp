#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;

// #pragma comment(lib, "mysqlcppconn8-static.lib")
// #pragma comment(lib, "mysqlcppconn-static.lib")
#pragma comment(lib, "mysqlcppconn.lib")
// #pragma comment(lib, "mysqlcppconn-static.lib")

//for demonstration only. never save your password in the code!


int main()
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

	//please create database "quickstartdb" ahead of time
	con->setSchema("mysql");

	delete con;
	system("pause");
	return 0;
}
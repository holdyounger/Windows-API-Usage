
#if 0
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
// #include <jdbc/cppconn/*.h>
#include <jdbc/cppconn/driver.h>  
#include <jdbc/cppconn/exception.h>  
#include <jdbc/cppconn/resultset.h>  
#include <jdbc/cppconn/statement.h>  
#else
// #include <mysql/jdbc.h>
#endif

#include <stdlib.h>
#include <iostream>

#include <mysqlx/xdevapi.h>

#pragma comment(lib, "mysqlcppconn8.lib")
// #pragma comment(lib, "jdbc.lib")
// #pragma comment(lib, "mysqlcppconn.lib")

using namespace std;
using namespace mysqlx;

#if 0
void RunConnMySQL()
{
	const string server = "tcp://localhost:3306";
	const string username = "root1";
	const string password = "Admin@2022";

	sql::Driver* driver;
	sql::Connection* con;

	try
	{
		driver = sql::mysql::get_driver_instance();
		con = driver->connect(server, username, password);
	}
	catch (sql::SQLException e)
	{
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		system("pause");
		exit(1);
	}
}
#endif // 0


void ConnWithxDev()
{
	try {
		// 改成你的信息
		Session sess(SessionOption::USER, "root",
					SessionOption::PWD, "Admin@2022",
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
}

int main()
{
	// RunConnMySQL();
	ConnWithxDev();

	return 0;
}
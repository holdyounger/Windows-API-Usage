#include <iostream>
#include <windows.h>
#include <pqxx/pqxx>
#include <pqxx/transaction>

int main(int argc, char* argv[])
{
    // (Normally you'd check for valid command-line arguments.)

    if (argc < 3)
    {
        printf("Usage:\n %s <username> <password>", argv[0]);
        exit(-1);
    } 

    char conn[MAX_PATH];

    sprintf(conn, "postgresql://%s:%s@localhost/postgres", argv[1], argv[2]);

    try
    {
        pqxx::connection c{ conn };
        pqxx::work txn{ c };

        // For querying just one single value, the transaction has a shorthand method
        // query_value().
        //
        // Use txn.quote() to escape and quote a C++ string for use as an SQL string
        // in a query's text.
        pqxx::result res = txn.exec("SELECT * FROM pg_authid");

        for (int i = 0; i < res.size(); i++)
        {
            for(int j = 0; j < res[i].size(); j++)
                std::cout << "" << res[i].at(j) << '\t';

            std::cout << '\n';
        }
    }
    catch (std::bad_alloc const&)
    {
        std::cerr << "Out of memory!" << std::endl;
    }
    catch (pqxx::sql_error const& e)
    {
        std::cerr << "SQL error: " << e.what() << std::endl
            << "Query was: " << e.query() << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
    }
}
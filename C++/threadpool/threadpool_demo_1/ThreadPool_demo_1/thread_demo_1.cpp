#include "threadpool.h"

#include <iostream>
#include <vector>

void testPool_1000()
{
    ThreadPool pool(1000);
    std::vector<std::future<int>> vecFutur;

    for (int i = 0; i < 1000; i++)
    {

        // enqueue and store future
        auto result = pool.enqueue([](int answer) { return answer; }, i);

		vecFutur.emplace_back(std::move(result));
    }


	auto GetCheckResult = [=](std::vector<std::future<int>> vecResult) -> int {
		for (auto& it : vecResult)
		{
			std::future_status status = std::future_status::deferred;
			do
			{
				status = it.wait_for(std::chrono::milliseconds(10));
				if (status == std::future_status::ready)
				{
					if (it.get() == 999)
					{
						return 2;
						std::cout << "---------" << std::endl;
					}
					else
					{
						std::cout << "+++++++++" << std::endl;
					}
				}
			} while (status != std::future_status::ready);
		}

		return 1;
	};

	if (2 == GetCheckResult(std::move(vecFutur)))
	{
		std::cout << "========" << std::endl;
	}
	else
	{
		std::cout << "!!!!!!!!" << std::endl;
	}

}

int main() {
    // create thread pool with 4 worker threads

    testPool_1000();

    ThreadPool pool(1000);


    // enqueue and store future
    auto result = pool.enqueue([](int answer) { return answer; }, 42);


    // std::future_status status = result.wait_for(std::chrono::milliseconds(10));

    // get result from future
    std::cout << result.get() << std::endl;
}
#include <iostream>
#include <future>
#include <chrono>

int long_running_task() {
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 42;
}

int main() {
    std::future<int> result_future = std::async(std::launch::async, long_running_task);

    std::chrono::milliseconds timeout(1000);


    std::future_status status = result_future.wait_for(timeout);

    do {
        status = result_future.wait_for(timeout);
        if (status == std::future_status::ready) {
            int result = result_future.get();
            std::cout << "Result: " << result << std::endl;
        }
        else {
            std::cerr << "Timeout: the task is still running" << std::endl;
        }

    } while (status != std::future_status::ready);


    // 等待任务完成，以便正确获取结果
    // int result = result_future.get();
    // std::cout << "Final result: " << result << std::endl;

    getchar();

    return 0;
}

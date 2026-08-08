// c++ -std=c++11 -Wall -Wextra 05_thread_mutex.cpp -pthread -o /tmp/a && /tmp/a
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

int main() {
    std::mutex mu;
    int cash = 0;
    std::atomic<bool> done(false);

    std::thread depositor([&]() {
        for (int i = 0; i < 1000; ++i) {
            std::lock_guard<std::mutex> lock(mu);
            cash += 1;
        }
        done = true;
    });

    while (!done.load()) {
        std::lock_guard<std::mutex> lock(mu);
        // quan sát giá trị giữa chừng
        (void)cash;
    }

    depositor.join();
    std::cout << "cash=" << cash << "\n";
    return 0;
}

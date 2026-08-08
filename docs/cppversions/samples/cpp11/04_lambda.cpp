// c++ -std=c++11 -Wall -Wextra 04_lambda.cpp -o /tmp/a && /tmp/a
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> qty;
    qty.push_back(1);
    qty.push_back(5);
    qty.push_back(10);
    qty.push_back(20);

    const int threshold = 8;
    std::vector<int>::iterator it = std::find_if(
        qty.begin(), qty.end(),
        [threshold](int q) { return q > threshold; });

    if (it != qty.end()) {
        std::cout << "first qty > " << threshold << " is " << *it << "\n";
    }

    int sum = 0;
    std::for_each(qty.begin(), qty.end(), [&sum](int q) { sum += q; });
    std::cout << "sum=" << sum << "\n";
    return 0;
}

// c++ -std=c++11 -Wall -Wextra 02_move_semantics.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <utility>
#include <vector>

struct Buffer {
    std::vector<int> data;

    Buffer() = default;
    explicit Buffer(std::size_t n) : data(n, 1) {}

    Buffer(const Buffer& other) : data(other.data) {
        std::cout << "copy ctor size=" << data.size() << "\n";
    }

    Buffer& operator=(const Buffer& other) {
        std::cout << "copy assign\n";
        if (this != &other) data = other.data;
        return *this;
    }

    Buffer(Buffer&& other) noexcept : data(std::move(other.data)) {
        std::cout << "move ctor size=" << data.size() << "\n";
    }

    Buffer& operator=(Buffer&& other) noexcept {
        std::cout << "move assign\n";
        if (this != &other) data = std::move(other.data);
        return *this;
    }
};

Buffer makeBuffer() {
    Buffer b(1000);
    return b; // NRVO hoặc move
}

int main() {
    Buffer a = makeBuffer();
    Buffer b = std::move(a);
    std::cout << "b.size=" << b.data.size() << " a.size(after move)=" << a.data.size() << "\n";
    return 0;
}

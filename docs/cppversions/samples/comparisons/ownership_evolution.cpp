// c++ -std=c++14 -Wall -Wextra ownership_evolution.cpp -o /tmp/a && /tmp/a
#include <iostream>
#include <memory>
#include <string>
#include <utility>

struct Engine {
    std::string name;
    explicit Engine(std::string n) : name(std::move(n)) {
        std::cout << "ctor " << name << "\n";
    }
    ~Engine() { std::cout << "dtor " << name << "\n"; }
};

void rawStyle() {
    std::cout << "-- raw --\n";
    Engine* e = new Engine("raw");
    // nếu return sớm / exception → leak
    delete e;
}

void cxx11Style() {
    std::cout << "-- unique_ptr C++11 --\n";
    std::unique_ptr<Engine> e(new Engine("unique11"));
}

void cxx14Style() {
    std::cout << "-- make_unique C++14 --\n";
    auto e = std::make_unique<Engine>("unique14");
}

int main() {
    rawStyle();
    cxx11Style();
    cxx14Style();
    return 0;
}

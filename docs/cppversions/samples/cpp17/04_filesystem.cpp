// c++ -std=c++17 -Wall -Wextra 04_filesystem.cpp -o /tmp/a && /tmp/a
// GCC cũ đôi khi cần -lstdc++fs
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

int main() {
    const fs::path dir = fs::temp_directory_path() / "trading-app-cpp17-demo";
    fs::create_directories(dir);

    const fs::path db = dir / "trading.db";
    {
        std::ofstream out(db);
        out << "demo";
    }

    std::cout << "path=" << db << "\n";
    std::cout << "exists=" << std::boolalpha << fs::exists(db) << "\n";
    std::cout << "size=" << fs::file_size(db) << "\n";

    fs::remove_all(dir);
    return 0;
}

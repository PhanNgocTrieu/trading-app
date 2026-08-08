# docs/samples/phase3/CMakeLists.qt.snippet.cmake
# Đoạn mẫu để merge vào CMakeLists.txt gốc khi sang Phase 3.

cmake_minimum_required(VERSION 3.21)
project(trading-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
find_package(SQLite3 REQUIRED) # nếu dùng sqlite3 C API ở infrastructure

add_executable(trading-app
    apps/desktop/main.cpp
    apps/desktop/windows/login_window.cpp
    apps/desktop/windows/main_window.cpp
    # ... domain/application/infrastructure sources
)

target_include_directories(trading-app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/apps/desktop
)

target_link_libraries(trading-app PRIVATE
    Qt6::Widgets
    Qt6::Sql
    SQLite::SQLite3
)

# Optional: giữ CLI trong lúc migrate
# add_executable(trading-app-cli ui/main.cpp ...)

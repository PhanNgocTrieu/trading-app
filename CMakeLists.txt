cmake_minimum_required(VERSION 3.16)
project(trading-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TRADING_APP_BUILD_TESTS "Build GoogleTest unit tests" ON)

add_subdirectory(src)
add_subdirectory(service)

add_executable(trading-app
    ui/main.cpp
    ui/dashboard.cpp
)

target_include_directories(trading-app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/service
)

target_link_libraries(trading-app PRIVATE service)

target_compile_definitions(trading-app PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(trading-app PRIVATE -Wall -Wextra -Wpedantic)

if(TRADING_APP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

cmake_minimum_required(VERSION 3.10)
project(trading-app)

add_executable(trading-app 
    ui/main.cpp
    ui/dashboard.cpp
    src/user.cpp
    src/stock.cpp
    service/service.cpp
    service/login.cpp
)

add_subdirectory(service)

target_compile_definitions(trading-app PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(trading-app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(trading-app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/service")
target_compile_features(trading-app PRIVATE cxx_std_11)
target_compile_options(trading-app PRIVATE -Wall -Wextra -Wpedantic)
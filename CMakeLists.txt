cmake_minimum_required(VERSION 3.16)
project(trading-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# Phase 0 optional: lightweight domain tests (no external framework)
enable_testing()
add_executable(phase0_account_test
    tests/phase0_account_test.cpp
)
target_include_directories(phase0_account_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(phase0_account_test PRIVATE cxx_std_17)
target_compile_options(phase0_account_test PRIVATE -Wall -Wextra -Wpedantic)
add_test(NAME phase0_account_test COMMAND phase0_account_test)

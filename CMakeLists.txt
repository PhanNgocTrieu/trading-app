cmake_minimum_required(VERSION 3.21)
project(trading-app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TRADING_APP_BUILD_TESTS "Build GoogleTest unit tests" ON)
option(TRADING_APP_BUILD_QT "Build Qt desktop UI (trading-app)" ON)
option(TRADING_APP_BUILD_CLI "Build console CLI (trading-app-cli)" ON)

# Prefer a local Qt online installer prefix when present.
if(DEFINED ENV{Qt6_DIR})
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")
elseif(EXISTS "$ENV{HOME}/Qt/6.11.1/macos")
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.11.1/macos")
elseif(EXISTS "$ENV{HOME}/Qt/6.8.0/macos")
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{HOME}/Qt/6.8.0/macos")
endif()

add_subdirectory(src)
add_subdirectory(service)

if(TRADING_APP_BUILD_CLI)
    add_executable(trading-app-cli
        ui/main.cpp
        ui/dashboard.cpp
    )

    target_include_directories(trading-app-cli PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/service
    )

    target_link_libraries(trading-app-cli PRIVATE service)

    target_compile_definitions(trading-app-cli PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    )

    target_compile_options(trading-app-cli PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(TRADING_APP_BUILD_QT)
    find_package(Qt6 COMPONENTS Widgets QUIET)
    if(Qt6_FOUND)
        add_subdirectory(apps/desktop)
        message(STATUS "Qt6 found — building trading-app (desktop)")
    else()
        message(WARNING "Qt6 Widgets not found — skipping desktop UI. "
                        "Install Qt6 or set CMAKE_PREFIX_PATH / Qt6_DIR.")
    endif()
endif()

if(TRADING_APP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

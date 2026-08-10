add_library(service
    service.cpp
    login.cpp
    bank.cpp
)

target_include_directories(service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(service PUBLIC trading_core)

target_compile_features(service PUBLIC cxx_std_17)
target_compile_options(service PRIVATE -Wall -Wextra -Wpedantic)

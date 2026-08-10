add_library(trading_core
    infrastructure/db/migrator.cpp
    infrastructure/db/sqlite_repositories.cpp
    application/simple_password_hasher.cpp
    application/auth_app_service.cpp
    application/wallet_app_service.cpp
)

target_include_directories(trading_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(trading_core PUBLIC cxx_std_17)
target_compile_options(trading_core PRIVATE -Wall -Wextra -Wpedantic)

find_package(SQLite3 REQUIRED)
target_link_libraries(trading_core PUBLIC SQLite3::SQLite3)

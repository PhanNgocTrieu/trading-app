include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

set(TRADING_TEST_SOURCES
    phase0/test_result.cpp
    phase0/test_account.cpp
    phase0/test_user.cpp
    phase0/test_stock.cpp
    phase0/test_order_types.cpp
    phase0/test_session.cpp
    phase0/test_login_service.cpp
    phase0/test_bank_service.cpp
    phase1/test_password_hasher.cpp
    phase1/test_sqlite_infra.cpp
    phase1/test_auth_wallet.cpp
    phase2/test_position.cpp
    phase2/test_matching_engine.cpp
    phase3/test_phase3_pending.cpp
    phase4/test_phase4_pending.cpp
)

add_executable(trading_unit_tests ${TRADING_TEST_SOURCES})

target_include_directories(trading_unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/service
)

target_link_libraries(trading_unit_tests PRIVATE
    service
    GTest::gtest_main
)

target_compile_features(trading_unit_tests PRIVATE cxx_std_17)
target_compile_options(trading_unit_tests PRIVATE -Wall -Wextra -Wpedantic)

gtest_discover_tests(trading_unit_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    PROPERTIES LABELS "unit"
)

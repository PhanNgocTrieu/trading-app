set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

add_library(trading_desktop
    controllers/auth_controller.cpp
    controllers/wallet_controller.cpp
    controllers/order_controller.cpp
    models/position_table_model.cpp
    models/quote_table_model.cpp
    models/order_book_table_model.cpp
    models/working_order_table_model.cpp
    market/mock_market_data_feed.cpp
    bridge/trading_app_bridge.cpp
)

target_include_directories(trading_desktop
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(trading_desktop
    PUBLIC
        trading_core
        Qt6::Quick
        Qt6::QuickControls2
)

target_compile_features(trading_desktop PUBLIC cxx_std_17)
target_compile_options(trading_desktop PRIVATE -Wall -Wextra -Wpedantic)

add_executable(trading-app
    main.cpp
    qml.qrc
)

target_link_libraries(trading-app PRIVATE trading_desktop)
target_compile_options(trading-app PRIVATE -Wall -Wextra -Wpedantic)

# Help macOS find Qt plugins when launching from the build tree.
if(APPLE)
    set_target_properties(trading-app PROPERTIES
        BUILD_RPATH "${Qt6_DIR}/../../../lib;${CMAKE_BINARY_DIR}"
    )
endif()

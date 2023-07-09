project(order-chronicles)

add_library(orderbook Init.cpp)

target_include_directories(orderbook
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
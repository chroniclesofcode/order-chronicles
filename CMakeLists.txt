cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
project(order-chronicles)

add_subdirectory(include)
add_subdirectory(src)

add_executable(main main.cpp)
target_link_libraries(main
    PRIVATE
    orderbook
)
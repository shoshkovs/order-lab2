cmake_minimum_required(VERSION 3.16)

project(order_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(enable_project_warnings target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

enable_testing()

add_library(order_core
    src/CashbackDiscount.cpp
    src/CourierDelivery.cpp
    src/DeliveryProxy.cpp
    src/FreeDeliveryDiscount.cpp
    src/GiftDiscount.cpp
    src/IObservable.cpp
    src/LoggingProxy.cpp
    src/Operators.cpp
    src/Order.cpp
    src/OrderEvent.cpp
    src/OrderManager.cpp
    src/OrderStatus.cpp
    src/PercentDiscount.cpp
    src/PickupDelivery.cpp
    src/PostDelivery.cpp
    src/Product.cpp
    src/ProductType.cpp
)
target_include_directories(order_core PUBLIC include)
enable_project_warnings(order_core)

add_executable(order_tests tests/test_main.cpp)
target_include_directories(order_tests PRIVATE tests)
target_link_libraries(order_tests PRIVATE order_core)
enable_project_warnings(order_tests)

add_test(NAME order_tests COMMAND order_tests)

add_executable(order_demo src/main.cpp src/Console.cpp)
target_link_libraries(order_demo PRIVATE order_core)
enable_project_warnings(order_demo)

add_test(NAME order_demo_smoke COMMAND order_demo --demo)
set_tests_properties(
    order_demo_smoke
    PROPERTIES PASS_REGULAR_EXPRESSION
               "Scenario 1.*Scenario 2.*Scenario 3.*Scenario 4.*Scenario 5"
)

cmake_minimum_required(VERSION 3.10)

project(Stock-market-simulation LANGUAGES CXX)

set(CMAKE_CXX_CLANG_TIDY "clang-tidy;
-warnings-as-errors=*")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED_ON)

set(SOURCES
    main.cpp
    Order.h
    OrderBook.h
    OrderBook.cpp
    Agent.h
    AggressiveAgent.h
    AggressiveAgent.cpp
        OrderType.h
    ConservativeAgent.h
    ConservativeAgent.cpp
    Logger.h
    AgentType.h
    Market.h
    Market.cpp
    Trade.h
)

add_executable(sm_sim ${SOURCES})

if (MSVC)
    target_compile_options(sm_sim PRIVATE /W4 /WX)
else (
        target_compile_options(sm_sim PRIVATE -Wall -Wextra -Wpedantic -Werror)
)
endif()
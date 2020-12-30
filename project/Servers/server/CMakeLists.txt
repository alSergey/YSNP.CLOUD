CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
PROJECT(Server)

FIND_PACKAGE(Boost COMPONENTS log log_setup REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")

INCLUDE_DIRECTORIES(../../DataBases/MongoDB/include)
INCLUDE_DIRECTORIES(../../utils/include)

FIND_LIBRARY(Network-dev REQUIRED)
INCLUDE_DIRECTORIES(../../Network/Server/include)


INCLUDE_DIRECTORIES(include)

ADD_LIBRARY(${PROJECT_NAME}-dev
        src/Config.cpp
        src/Worker.cpp
        )
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dev ${Boost_LIBRARIES} Network-dev)

CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
PROJECT(Network)

SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")

FIND_PACKAGE(Boost COMPONENTS log REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

INCLUDE_DIRECTORIES(Server/include Client/include)

ADD_LIBRARY(Network-dev SHARED
        Server/src/NetworkServer.cpp
        Server/src/UserSession.cpp
        Server/src/ClientWorker.cpp
        )
TARGET_LINK_LIBRARIES(Network-dev ${Boost_LIBRARIES})

ADD_LIBRARY(ClientNetwork-dev STATIC
        Client/src/ClientNetwork.cpp)
TARGET_LINK_LIBRARIES(ClientNetwork-dev ${Boost_LIBRARIES})

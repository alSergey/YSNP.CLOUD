CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
PROJECT(App)

SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")

FIND_LIBRARY(InternalDB-dev REQUIRED)
INCLUDE_DIRECTORIES(../DataBases/InternalDB/include)

FIND_PACKAGE(Boost COMPONENTS log REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

FIND_LIBRARY(ClientNetwork-dev REQUIRED)
INCLUDE_DIRECTORIES(../Network/Client/include)

FIND_LIBRARY(Watcher-dev REQUIRED)
INCLUDE_DIRECTORIES(../Client/Watcher/include)

FIND_LIBRARY(Utils-dev REQUIRED)
INCLUDE_DIRECTORIES(../utils/include)

FIND_LIBRARY(Indexer-dev REQUIRED)
INCLUDE_DIRECTORIES(../Client/Indexer/include)

FIND_LIBRARY(Chunker-dev REQUIRED)
INCLUDE_DIRECTORIES(../Client/Chunker/include)

if (APPLE)
    FIND_LIBRARY(WatcherApple-dev REQUIRED)
    INCLUDE_DIRECTORIES(../Client/Watcher/Apple/include)
    INCLUDE_DIRECTORIES(../Client/Watcher/include)
    MESSAGE(STATUS "Apple Watcher")
else ()
    FIND_LIBRARY(WatcherLinux-dev REQUIRED)
    INCLUDE_DIRECTORIES(../Client/Watcher/include)
    INCLUDE_DIRECTORIES(../Client/Watcher/Linux/include)
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    if (APPLE)
        include_directories(/usr/local/Cellar/openssl@1.1/1.1.1h/include)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1h/lib/libssl.dylib)
        list(APPEND LIB_LIST /usr/local/Cellar/openssl@1.1/1.1.1h/lib/libcrypto.dylib)
        message(STATUS "OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
    else ()
        include_directories(${OPENSSL_INCLUDE_DIR})
        list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})
        message(STATUS "OpenSSL Version: ${OPENSSL_VERSION} ${OPENSSL_INCLUDE_DIR} ${OPENSSL_LIBRARIES}")
    endif ()
endif ()

INCLUDE_DIRECTORIES(include)

ADD_LIBRARY(${PROJECT_NAME}-dev
        src/App.cpp
        src/Worker.cpp
        src/Commands.cpp
        src/ClientConfig.cpp
        )
if (APPLE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dev ${Boost_LIBRARIES} OpenSSL::Crypto InternalDB-dev ClientNetwork-dev Utils-dev Indexer-dev WatcherApple-dev Chunker-dev)
else ()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dev ${Boost_LIBRARIES} OpenSSL::Crypto InternalDB-dev ClientNetwork-dev Utils-dev Indexer-dev WatcherLinux-dev Chunker-dev)
endif ()

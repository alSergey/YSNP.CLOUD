CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
PROJECT(Utils)

FIND_PACKAGE(Boost COMPONENTS log REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror -Wpedantic")

SET(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
FIND_PACKAGE(OpenSSL REQUIRED)
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

ADD_LIBRARY(${PROJECT_NAME}-dev STATIC
        src/SerializerChunk.cpp
        src/SerializerUserChunk.cpp
        src/SerializerAnswer.cpp
        src/SerializerUserDate.cpp
        src/SerializerFileInfo.cpp
        src/SerializerUserInfo.cpp
        src/SerializerUserIds.cpp
        include/structs/FileMeta.h
        )
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dev OpenSSL::Crypto ${Boost_LIBRARIES})

ENABLE_TESTING()
ADD_SUBDIRECTORY(test)
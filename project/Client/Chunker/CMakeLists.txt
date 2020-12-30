cmake_minimum_required(VERSION 3.16.5)
project(Chunker)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)
set( OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
find_package(OpenSSL REQUIRED)
FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
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

INCLUDE_DIRECTORIES(../../utils/include)
INCLUDE_DIRECTORIES(include)

ADD_LIBRARY(${PROJECT_NAME}-dev STATIC
        include/Chunker.h
        src/Chunker.cpp
        src/File.cpp
        include/File.h
        )
if (APPLE)
    target_link_libraries(${PROJECT_NAME}-dev OpenSSL::Crypto ZLIB::ZLIB ${Boost_LIBRARIES})
else ()
    target_link_libraries(${PROJECT_NAME}-dev OpenSSL::Crypto ZLIB::ZLIB ${Boost_LIBRARIES})
endif ()

ADD_SUBDIRECTORY(tests)

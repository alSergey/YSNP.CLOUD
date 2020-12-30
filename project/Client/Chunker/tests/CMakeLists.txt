FIND_PACKAGE(GTest REQUIRED)
INCLUDE_DIRECTORIES("${GTEST_INCLUDE_DIRS}")
FIND_PACKAGE(Threads REQUIRED)

set( OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

FIND_PACKAGE(Boost COMPONENTS log REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

FIND_LIBRARY(Utils-dev REQUIRED)

PROJECT(tests_gtest_client)
SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

set( OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
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

INCLUDE_DIRECTORIES("${GTEST_INCLUDE_DIRS}")


include_directories(${GTEST_INCLUDE_DIRS})
#INCLUDE_DIRECTORIES(/usr/local/opt/openssl@1.1/include)
include_directories(include)
include_directories(../include)
include_directories(../../utils)
include_directories(../../../utils/include)

add_executable(ChunkerTests main.cpp test_Chunker.cpp)
if (APPLE)
    target_link_libraries(ChunkerTests ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} Chunker-dev OpenSSL::Crypto ZLIB::ZLIB)
else ()
    target_link_libraries(ChunkerTests ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} Chunker-dev OpenSSL::Crypto ZLIB::ZLIB ${Boost_LIBRARIES} Utils-dev )
endif ()

ADD_TEST(NAME Test COMMAND ${PROJECT_NAME})
FIND_PACKAGE(GTest REQUIRED)
FIND_PACKAGE(Threads REQUIRED)
find_package(
        Boost 1.54.0
        COMPONENTS unit_test_framework system filesystem
        REQUIRED
)
INCLUDE_DIRECTORIES("${GTEST_INCLUDE_DIRS}")

PROJECT(tests_gtest)
SET(CMAKE_CXX_STANDARD 17)
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wpedantic")

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(include)
include_directories(../include)
include_directories(../../utils)

add_executable(tests_gtest main.cpp test_Watcher.cpp)

target_link_libraries(tests_gtest ${GTEST_BOTH_LIBRARIES}  ${CMAKE_THREAD_LIBS_INIT} Watcher-dev  Boost::system
        Boost::filesystem)

ADD_TEST(NAME Test COMMAND ${PROJECT_NAME})
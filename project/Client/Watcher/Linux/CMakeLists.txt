CMAKE_MINIMUM_REQUIRED(VERSION 3.13.4)
project(WatcherLinux)

FIND_PACKAGE(Boost COMPONENTS system filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

INCLUDE_DIRECTORIES(./include)

INCLUDE_DIRECTORIES(include)
ADD_LIBRARY(${PROJECT_NAME}-dev STATIC
        include/FileSystemEvent.h
        include/Watcher.h
        src/Watcher.cpp
        src/FileSystemEvent.cpp
        include/Notification.h)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}-dev ${Boost_LIBRARIES})
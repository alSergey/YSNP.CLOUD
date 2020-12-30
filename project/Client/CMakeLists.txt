cmake_minimum_required(VERSION 3.16.5)
project(Client)

set(CMAKE_CXX_STANDARD 17)

ADD_SUBDIRECTORY(Watcher)
ADD_SUBDIRECTORY(Indexer)
ADD_SUBDIRECTORY(Chunker)

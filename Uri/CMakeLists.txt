# CMakeLists.txt for Uri

cmake_minimum_required(VERSION 3.8)
set(This Uri)

set(headers
    include/Uri/Uri.hpp
)

set(Sources
    src/Uri.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})
set_target_properties(${This} PROPERTIES
    FOLDER Libraries
)

target_include_directories(${This} PUBLIC include)

add_subdirectory(test)
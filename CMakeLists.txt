cmake_minimum_required(VERSION 3.9)
project(assignment1)

set(CMAKE_CXX_STANDARD 14)

include_directories("include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g")

add_executable(traintest
        src/main_traintest.cpp
        src/file_manager.cpp
        src/tokenizer.cpp
        src/porter_stemmer.cpp
        src/util.cpp
        src/doc_preprocessor.cpp
        src/parser.cpp
)
set_target_properties(traintest PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)

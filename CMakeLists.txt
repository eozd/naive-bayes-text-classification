cmake_minimum_required(VERSION 3.9)
project(assignment1)

set(CMAKE_CXX_STANDARD 14)

include_directories("include")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++14 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++14 -O3")

add_executable(construct_datasets
        src/main_construct_datasets.cpp
        src/file_manager.cpp
        src/tokenizer.cpp
        src/porter_stemmer.cpp
        src/util.cpp
        src/doc_preprocessor.cpp
        src/parser.cpp
        src/defs.cpp)

add_executable(classifier
        src/main_classifier.cpp src/defs.cpp src/file_manager.cpp include/feature_selection.hpp)

set_target_properties(construct_datasets PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)
set_target_properties(classifier PROPERTIES RUNTIME_OUTPUT_DIRECTORY ..)

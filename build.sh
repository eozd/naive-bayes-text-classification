#!/usr/bin/env bash

# Build the project.
#
# usage: ./build.sh <BUILD_TYPE> [<CMAKE_ARGS>...]
#
# <BUILD_TYPE> must be debug, release, clean, doc.
# <CMAKE_ARGS> are arguments to forward to cmake. You can use any cmake argument
# here such as -DCMAKE_CXX_COMPILER=g++-5.
#
# example usages:
# 
# 1. Build only the library
#
#        ./build.sh release
#
# If no build type is given, the library is built in Release mode.

build=$1
shift

if [[ ${build} == "debug" ]]; then
	build_flag="-DCMAKE_BUILD_TYPE=Debug"
elif [[ ${build} == "release" ]]; then
	build_flag="-DCMAKE_BUILD_TYPE=Release"
elif [[ ${build} == "clean" ]]; then
	rm -rf build ||:
	rm -rf construct_datasets ||:
	rm -rf classifier ||:
	rm -rf doc ||:
	exit
elif [[ ${build} == "doc" ]]; then
    doxygen Doxyfile
	exit
else
	echo "Unknown build type: Use one of debug, release, clean, doc"
	exit
fi

mkdir -p build
cd build
cmake ${build_flag} $* ..
make -j6

#!/usr/bin/env bash

mkdir -p build
cd build || return 1
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .

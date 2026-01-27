# Introduction

This repository consists of some graph generation algorithms programmed in `c++`. The available graph types are listed and explained below.

## Compiling

Currently, there are no external libraries in the program. You should be able to use any `c++` compiler. I use `clang++` using the following command.

clang++ -std=c++20 -O3 -o bin/gen_graph -Iinclude src/main.cpp src/find_odd_holes.cpp src/utils.cpp src/iterative_modification_heuristic.cpp src/gen_c5_free.cpp

./scripts/run_gen_graph.sh

## Usage
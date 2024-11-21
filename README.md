# DynamicalSystemFramework
<!-- [![codecov](https://codecov.io/github/sbaldu/DynamicalSystemFramework/graph/badge.svg?token=JV53J6IUJ3)](https://codecov.io/github/sbaldu/DynamicalSystemFramework) -->
[![Standard](https://img.shields.io/badge/c%2B%2B-20/23-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)

The aim of this project is to rework the original [Traffic Flow Dynamics Model](https://github.com/Grufoony/TrafficFlowDynamicsModel).
This rework consists of a full code rewriting, in order to implement more features (like *intersections*) and get advantage from the latest C++ updates.

## Requirements

This project requieres:
- A compiler whith full support for `C++20` or greater
- `cmake`
- `vcpkg`, which can be installed following points 1 and 2 in [this tutorial](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-bash)

Utilities are written in python. To install their dependencies:
```shell
pip install -r ./requirements.txt
```

## Installation
The library can be installed using CMake. Firstly, configure vpkg:
```shell
cmake --preset=default
```
To build and install the project in the default folder run:
```shell
cmake --build build
sudo cmake --install build
```

## Testing
This project uses [Doctest](https://github.com/doctest/doctest) for testing.

To compile tests run:
```shell
cd test
cmake -B build && make -C build
```
To run all the tests together use the command:
```shell
./dsm_tests.out
```

## Benchmarking
Some functionalities of the library have been benchmarked in order to assess their efficiency.  
The benchmarks are performed using a small toolkit developed by @sbaldu, in order to keep them simple and
without needing to rely on large external libraries.  
To compile the benchmarks use the commands:
```shell
cd benchmark
cmake -B build && make -C build
```
To run all the benchmarks together use the command:
```shell
for f in ./*.out ; do ./$f ; done
```

## Citing

```BibTex
@misc{DSM,
  author = {Berselli, Gregorio and Balducci, Simone},
  title = {Framework for modelling dynamical complex systems.},
  year = {2023},
  url = {https://github.com/physycom/DynamicalSystemFramework},
  publisher = {GitHub},
  howpublished = {\url{https://github.com/physycom/DynamicalSystemFramework}}
}
```

# DynamicalSystemFramework

The aim of this project is to rework the original [Traffic Flow Dynamics Model](https://github.com/Grufoony/TrafficFlowDynamicsModel).
This rework consists of a full code rewriting, in order to implement more features (like *intersections*) and get advantage from the latest C++ updates.

## Requirements

Right now the project only requires `C++20` and `cmake`.

## installation
The library can be installed using CMake. To do this build it with the commands:
```
mkdir -p build && cd build
cmake ..
```
and then install it with:
```
sudo make install
```

## Testing
To compile tests one can run:
```shell
cd test
cmake -B build && make -C build
```
To run all the tests together use the command:
```shell
for f in ./*.out ; do ./$f ; done
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
  url = {https://github.com/sbaldu/DynamicalSystemFramework},
  publisher = {GitHub},
  howpublished = {\url{https://github.com/sbaldu/DynamicalSystemFramework}}
}

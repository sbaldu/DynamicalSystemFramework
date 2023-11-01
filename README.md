# DynamicalSystemFramework

The aim of this project is to rework the original [Traffic Flow Dynamics Model](https://github.com/Grufoony/TrafficFlowDynamicsModel).
This rework consists of a full code rewriting, in order to implement more features (like *intersections*) and get advantage from the latest C++ updates.

## Requirements

By now the project only requires `C++20` and `cmake`.
To install `cmake`:
```shell
sudo apt install cmake
```

## Installing

The framework is still WIP, more details will be available in the future.

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

## Citing

```BibTex
@misc{DSF,
  author = {Berselli, Gregorio and Balducci, Simone},
  title = {Framework for modelling dynamical complex systems.},
  year = {2023},
  url = {https://github.com/sbaldu/DynamicalSystemFramework},
  publisher = {GitHub},
  howpublished = {\url{https://github.com/sbaldu/DynamicalSystemFramework}}
}
```

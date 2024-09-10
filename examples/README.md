# DSM - Examples
Here are reported some example studies using the dsm framework.
Many of these simulations were used as thesis work.

## Simulation files
C++ files (`cpp`) are simulation files and need to be compiled.
Once set the parameters, simply run `make file_name` to compile and run the simulation using the Makefile.

### Slow Charge
Aim of these simulations is to load the road network in an adiabatic way, i.e. slowly increasing the mean density over streets.
You can try it both using all roundabouts intersections (RB case) or traffic lights (TL case).
To run the simulation you can use the Makefile:
```shell
make slow_charge_tl
```

## Utilities
Some Python scripts used to plot or draw some network properties.

### Gifter
Script to generate a GIF from the data in the temp_img folder, produced by the main library.
With this, one can see the evolution of the network over time.

### Plotter
This script generates various plots related to traffic flow analysis, including:

1. Giant Component plot: Plots the number of components and the size of the first and second ones.
2. Fundamental diagram plot: Plots the fundamental diagram of traffic flow vs. density.
3. Fluctuations plot: Plots the fluctuations in traffic density.
4. Signal plot: Plot the signal and the peak detected by the peak detection algorithm.
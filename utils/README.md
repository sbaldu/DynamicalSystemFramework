## Get OpenStreetMap data correctly formatted

In order to get data correctly formatted from OpenStreetMap you can use the *get_osm_data.py* script, passing as argument the locality for which you want the data.
For example:
```shell
python3 get_osm_data.py postua, vercelli, italy
```

## Gifter
Script to generate a GIF from the data in the temp_img folder, produced by the main library.
With this, one can see the evolution of the network over time.

## Plotter
This script generates various plots related to traffic flow analysis, including:

1. Giant Component plot: Plots the number of components and the size of the first and second ones.
2. Fundamental diagram plot: Plots the fundamental diagram of traffic flow vs. density.
3. Fluctuations plot: Plots the fluctuations in traffic density.
4. Signal plot: Plot the signal and the peak detected by the peak detection algorithm.
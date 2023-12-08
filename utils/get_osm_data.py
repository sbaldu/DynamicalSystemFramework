"""
This script is used to get the OSM data of a place and save it in a csv file.
The place is passed as a command line argument.

Example:
python3 get_osm_data.py postua, vercelli, italy

The output files are:
- nodes.csv
- edges.csv

The files are saved in the current directory.
"""

import sys
import osmnx as ox

if __name__ == "__main__":
    ox.config(use_cache=True, log_console=True)

    place = sys.argv[1]

    # get the street network for San Cesario sul Panaro
    G = ox.graph_from_place(place, network_type="drive")
    fig, ax = ox.plot_graph(G)

    gdf_nodes, gdf_edges = ox.graph_to_gdfs(G)

    # notice that osmnid is the index of the gdf_nodes DataFrame, so take it as a column
    gdf_nodes.reset_index(inplace=True)
    gdf_edges.reset_index(inplace=True)

    gdf_nodes = gdf_nodes[["osmid", "x", "y", "highway"]]
    gdf_edges = gdf_edges[
        ["u", "v", "length", "oneway", "highway", "maxspeed", "bridge"]
    ]

    gdf_nodes.to_csv("nodes.csv", sep=";", index=False)
    gdf_edges.to_csv("edges.csv", sep=";", index=False)

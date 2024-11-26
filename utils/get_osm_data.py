"""
This script is used to get the OSM data of a place and save it in a csv file.
The place is passed as a command line argument.

Example:
python get_osm_data.py --place "postua, vercelli, italy"

The output files are:
- nodes.csv
- edges.csv

The files are saved in the current directory.
"""

from argparse import ArgumentParser
import osmnx as ox

if __name__ == "__main__":
    parser = ArgumentParser("Script to get the OSM data of a place.")
    parser.add_argument(
        "--place",
        required=True,
        help="Place to get the OSM data in the format: city, province, country",
    )
    parser = parser.parse_args()

    # get the street network for San Cesario sul Panaro
    G = ox.graph_from_place(parser.place, network_type="drive")
    ox.plot_graph(G)

    gdf_nodes, gdf_edges = ox.graph_to_gdfs(G)

    # notice that osmnid is the index of the gdf_nodes DataFrame, so take it as a column
    gdf_nodes.reset_index(inplace=True)
    gdf_edges.reset_index(inplace=True)

    gdf_nodes = gdf_nodes[["osmid", "x", "y", "highway"]]
    if "lanes" not in gdf_edges.columns:
        gdf_edges["lanes"] = None
    gdf_edges = gdf_edges[
        ["u", "v", "length", "oneway", "lanes", "highway", "maxspeed", "bridge"]
    ]

    gdf_nodes.to_csv("nodes.csv", sep=";", index=False)
    gdf_edges.to_csv("edges.csv", sep=";", index=False)

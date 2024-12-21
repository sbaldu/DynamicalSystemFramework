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
import networkx as nx
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt

RGBA_RED = (1, 0, 0, 0.1)
RGBA_WHITE = (1, 1, 1, 1)

FLAGS_MOTORWAY = ["motorway", "motorway_link"]
FLAGS_NORMAL = [
    "primary",
    "secondary",
    "tertiary",
    "trunk",
    "primary_link",
    "secondary_link",
    "tertiary_link",
    "trunk_link",
]
FLAGS_RESIDENTIAL = [
    "residential",
    "living_street",
    "unclassified",
]

if __name__ == "__main__":
    parser = ArgumentParser("Script to get the OSM data of a place.")
    parser.add_argument(
        "--place",
        required=True,
        help="Place to get the OSM data in the format: city, province, country",
    )
    parser.add_argument(
        "--exclude-motorway",
        required=False,
        default=False,
        help="Exclude motorways from the data",
    )
    parser.add_argument(
        "--exclude-residential",
        required=False,
        default=False,
        help="Exclude residential roads from the data",
    )
    parser = parser.parse_args()

    # get the street network for San Cesario sul Panaro
    G_ALL = ox.graph_from_place(parser.place, network_type="drive")
    print(f"Graph created with {len(G_ALL.nodes)} nodes and {len(G_ALL.edges)} edges.")

    gdf_nodes, gdf_edges = ox.graph_to_gdfs(G_ALL)
    gdf_edges["highway"] = gdf_edges["highway"].apply(
        lambda x: x[-1] if isinstance(x, list) else x
    )
    # gdf_edges = gdf_edges[~gdf_edges["access"].isin(["no", "private"])]

    # Make a plot to visualize the removed links
    removed_patch = mpatches.Patch(color=RGBA_RED, label="Removed Nodes and Edges")

    if parser.exclude_motorway:
        gdf_edges = gdf_edges[~gdf_edges["highway"].isin(FLAGS_MOTORWAY)]
    if parser.exclude_residential:
        gdf_edges = gdf_edges[~gdf_edges["highway"].isin(FLAGS_RESIDENTIAL)]

    # rebuild the graph
    G = ox.graph_from_gdfs(gdf_nodes, gdf_edges)
    G.remove_nodes_from(list(nx.isolates(G)))
    print(f"Graph filterded: now it has {len(G.nodes)} nodes and {len(G.edges)} edges.")

    fig, ax = ox.plot_graph(
        G_ALL,
        node_color=[
            RGBA_RED if node not in G.nodes else RGBA_WHITE for node in G_ALL.nodes
        ],
        edge_color=[
            RGBA_RED if edge not in G.edges else RGBA_WHITE for edge in G_ALL.edges
        ],
        show=False,
        close=False,
    )
    ax.legend(handles=[removed_patch])
    plt.show()

    gdf_nodes, gdf_edges = ox.graph_to_gdfs(G)

    # notice that osmnid is the index of the gdf_nodes DataFrame, so take it as a column
    gdf_nodes.reset_index(inplace=True)
    gdf_edges.reset_index(inplace=True)
    # Prepare node dataframe
    gdf_nodes = gdf_nodes[["osmid", "x", "y", "highway"]]
    # Prepare edge dataframe
    if "lanes" not in gdf_edges.columns:
        gdf_edges["lanes"] = None

    gdf_edges["lanes"] = gdf_edges["lanes"].apply(
        lambda x: max(x) if isinstance(x, list) else x
    )

    gdf_edges = gdf_edges[
        ["u", "v", "length", "oneway", "lanes", "highway", "maxspeed", "name"]
    ]
    # Save the data
    gdf_nodes.to_csv("nodes.csv", sep=";", index=False)
    gdf_edges.to_csv("edges.csv", sep=";", index=False)

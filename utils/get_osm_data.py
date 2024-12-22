"""
This script is used to get the OSM data of a place and save it in a csv file.
The place is passed as a command line argument.

Example:
python get_osm_data.py --place "Bologna, Emilia-Romagna, Italy" --exclude-residential True

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
import logging

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
    "service",
    "pedestrian",
    "busway",
]


def simplify_graph(_G):
    G = _G.copy()
    previous_nodes = 0
    while previous_nodes != len(G.nodes):
        logging.info(f"New cycle: current_nodes={len(G.nodes)}")
        previous_nodes = len(G.nodes)
        for node in G.copy().nodes:
            # define neighborus as list of predecessors and successors
            neighbours = list(G.predecessors(node)) + list(G.successors(node))
            if (
                len(neighbours) != 2
                or G.in_degree(node) != G.out_degree(node)
                or G.in_degree(node) > 2
            ):
                continue
            u, v = neighbours
            if G.has_edge(u, v):
                continue
            # print(f"Node {node} has {len(neighbours)} neighbours and in_degree={G.in_degree(node)} out_degree={G.out_degree(node)}")
            edge_uv = None
            edge_vu = None
            try:
                data_u = G.get_edge_data(u, node)[0]
                data_v = G.get_edge_data(node, v)[0]
                string1 = (
                    " ".join(data_u["name"])
                    if isinstance(data_u["name"], list)
                    else data_u["name"]
                )
                string2 = (
                    " ".join(data_v["name"])
                    if isinstance(data_v["name"], list)
                    else data_v["name"]
                )
                if string1 in string2 or string2 in string1:
                    edge_uv = data_u
                    # set length as the sum
                    edge_uv["length"] = data_u["length"] + data_v["length"]
                    if "lanes" in edge_uv:
                        if "lanes" in data_v:
                            edge_uv["lanes"] = max(data_u["lanes"], data_v["lanes"])
                    else:
                        edge_uv["lanes"] = 1
                    # merge also linestrings
                    edge_uv["geometry"] = data_u["geometry"].union(data_v["geometry"])
                # else:
                # print(f"Edges {u} -> {node} and {node} -> {v} have different names: {data_u['name']} and {data_v['name']}")
            except Exception as e:
                logging.warning(f"Error while merging edges {data_u} and {data_v}: {e}")
            try:
                data_u = G.get_edge_data(node, u)[0]
                data_v = G.get_edge_data(v, node)[0]
                if not "name" in data_u:
                    data_u["name"] = ""
                if not "name" in data_v:
                    data_v["name"] = ""
                string1 = (
                    " ".join(data_u["name"])
                    if isinstance(data_u["name"], list)
                    else data_u["name"]
                )
                string2 = (
                    " ".join(data_v["name"])
                    if isinstance(data_v["name"], list)
                    else data_v["name"]
                )
                if string1 in string2 or string2 in string1:
                    edge_vu = data_u
                    # set length as the sum
                    edge_vu["length"] = data_u["length"] + data_v["length"]
                    # if it has lane attribute, use max, else 1
                    if "lanes" in edge_vu:
                        if "lanes" in data_v:
                            edge_vu["lanes"] = max(data_u["lanes"], data_v["lanes"])
                    else:
                        edge_vu["lanes"] = 1
                    # merge also linestrings
                    edge_vu["geometry"] = data_u["geometry"].union(data_v["geometry"])
                # else:
                #     print(f"Edges {v} -> {node} and {node} -> {u} have different names: {data_u['name']} and {data_v['name']}")
            except Exception as e:
                logging.warning(f"Error while merging edges {data_u} and {data_v}: {e}")

            if not (edge_uv is None and edge_vu is None):
                if edge_uv is not None:
                    # print(f"Edges {u} -> {node} and {node} -> {v} can be merged.")
                    G.add_edge(
                        u,
                        v,
                        length=edge_uv["length"],
                        name=edge_uv["name"],
                        geometry=edge_uv["geometry"],
                    )
                    # print(f"Added edge {G.get_edge_data(u, v)}")
                if edge_vu is not None:
                    # print(f"Edges {v} -> {node} and {node} -> {u} can be merged.")
                    G.add_edge(
                        v,
                        u,
                        length=edge_vu["length"],
                        name=edge_vu["name"],
                        geometry=edge_vu["geometry"],
                    )
                    # print(f"Added edge {G.get_edge_data(v, u)}")
                G.remove_node(node)
                # print(f"Removed node {node}")

    # assert that G has not isolated nodes
    assert list(nx.isolates(G)) == []
    return G


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
    logging.basicConfig(level=logging.INFO)
    # set up colored logging
    logging.addLevelName(
        logging.INFO, "\033[1;32m%s\033[1;0m" % logging.getLevelName(logging.INFO)
    )
    logging.addLevelName(
        logging.WARNING, "\033[1;33m%s\033[1;0m" % logging.getLevelName(logging.WARNING)
    )
    logging.addLevelName(
        logging.ERROR, "\033[1;31m%s\033[1;0m" % logging.getLevelName(logging.ERROR)
    )

    # get the street network for San Cesario sul Panaro
    G_ALL = ox.graph_from_place(parser.place, network_type="drive")
    print(f"Graph created with {len(G_ALL.nodes)} nodes and {len(G_ALL.edges)} edges.")

    gdf_nodes, gdf_edges = ox.graph_to_gdfs(G_ALL)
    gdf_edges["highway"] = gdf_edges["highway"].apply(
        lambda x: x[-1] if isinstance(x, list) else x
    )
    if "lanes" not in gdf_edges.columns:
        gdf_edges["lanes"] = None
    gdf_edges["lanes"] = gdf_edges["lanes"].apply(
        lambda x: max(x) if isinstance(x, list) else 1 if x is None else x
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
    print(f"Graph filtered: now it has {len(G.nodes)} nodes and {len(G.edges)} edges.")
    G = simplify_graph(G)
    print(
        f"Graph simplified: now it has {len(G.nodes)} nodes and {len(G.edges)} edges."
    )

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
    fig.set_size_inches(16, 9)
    plt.savefig("removed_nodes_and_edges.png")

    # Plot resulting graph
    fig, ax = ox.plot_graph(G, show=False, close=False)
    fig.set_size_inches(16, 9)
    plt.savefig("final_graph.png")

    # notice that osmnid is the index of the gdf_nodes DataFrame, so take it as a column
    gdf_nodes.reset_index(inplace=True)
    gdf_edges.reset_index(inplace=True)
    # Prepare node dataframe
    gdf_nodes = gdf_nodes[["osmid", "x", "y", "highway"]]
    # Prepare edge dataframe

    gdf_edges.to_csv("edges_ALL.csv", sep=";", index=False)
    gdf_edges = gdf_edges[
        ["u", "v", "length", "oneway", "lanes", "highway", "maxspeed", "name"]
    ]
    # Save the data
    gdf_nodes.to_csv("nodes.csv", sep=";", index=False)
    gdf_edges.to_csv("edges.csv", sep=";", index=False)

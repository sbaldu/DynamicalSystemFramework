"""
Some shared functions between scripts
"""

import networkx as nx
import pandas as pd


def create_graph_from_adj(adj: list, coord: pd.DataFrame):
    """Creates a graph given adjacency matrix and a dataframe of coordinates"""
    n = len(adj)
    graph = nx.DiGraph()
    graph.add_nodes_from(range(n))
    for i in range(n):
        for j in range(n):
            if adj[i, j] > 0:
                graph.add_edge(i, j, color="g", weight=adj[i, j])
                graph.add_edge(j, i, color="g", weight=adj[j, i])
    edges = graph.edges()
    pos = {}
    # coord has id as index with lat, lon columns
    for node in graph.nodes():
        pos[node] = (coord.loc[node]["lon"], coord.loc[node]["lat"])

    return (graph, edges, pos)

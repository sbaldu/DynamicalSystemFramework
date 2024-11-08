"""
Some shared functions between scripts
"""

import networkx as nx


def create_graph_from_adj(adj: list, coord: list):
    """Creates a graph given adjacency matrix and a list of coordinates"""
    n = len(adj)
    graph = nx.DiGraph()
    graph.add_nodes_from(range(n))
    for i in range(n):
        for j in range(i + 1, n):
            if adj[i, j] > 0:
                graph.add_edge(i, j, color="g", weight=adj[i, j])
                graph.add_edge(j, i, color="g", weight=adj[j, i])
    edges = graph.edges()
    pos = {}
    for i in range(n):
        pos[i] = coord[i, :]

    return (graph, edges, pos)

import networkx as nx
import numpy as np

def create_graph_from_adj(adj, coord):
    n = len(adj)
    G = nx.DiGraph()
    G.add_nodes_from(range(n))
    for i in range(n):
        for j in range(i + 1, n):
            if adj[i, j] > 0:
                G.add_edge(i, j, color="g", weight=adj[i, j])
                G.add_edge(j, i, color="g", weight=adj[j, i])
    edges = G.edges()
    pos = {}
    for i in range(n):
        pos[i] = coord[i, :]

    return (G, edges, pos)
"""
Script to generate a gif from the data in the temp_img folder, produced by the main library.
With this, one can see the evolution of the network over time.
"""

import multiprocessing as mp
import pathlib
import platform
from tkinter.filedialog import askopenfilename
from argparse import ArgumentParser
import networkx as nx
import matplotlib.pyplot as plt
from matplotlib import colormaps
import numpy as np
from tqdm import tqdm
from PIL import Image, ImageFont
import pandas as pd
import geopandas as gpd
from functions import create_graph_from_adj
import contextily as ctx

# Constants
TIME_BEGIN = None  # None to take the last frames
N_FRAMES = 10
TIME_GRANULARITY = 300
COLORMAP = colormaps["RdYlGn_r"]

OUTPUT_FILE_NAME = "evolution.gif"

# if on wsl
FONT_PATH = ""
if platform.system() == "Linux":
    FONT_PATH = "/mnt/c/Windows/Fonts/arial.ttf"
elif platform.system() == "Darwin":  # MAC OS
    FONT_PATH = "/System/Library/Fonts/Supplemental/Arial.ttf"


def create_image(__df, __time, _graph, _pos, _edges, _n, _gdf):
    """
    Generates and saves an image of a graph with edges colored based on density.

    Parameters:
    __df (DataFrame): A pandas DataFrame containing the data.
    __time (int): The specific time (in seconds) for which the graph is to be generated.

    Returns:
    tuple: A tuple containing the time in seconds and the path to the saved image.
    """
    for col in __df.columns:
        index = int(col)
        density = __df.loc[__time][col]  # / (225 / 2000)
        src = index // _n
        dst = index % _n
        # set color of edge based on density using a colormap from green to red
        _graph[src][dst]["color"] = COLORMAP(density)
        # draw graph with colors
    colors = [_graph[u][v]["color"] for u, v in _edges]
    # draw graph
    _, ax = plt.subplots(figsize=(15, 15))
    # nx.draw(_graph, _pos, edge_color=colors, with_labels=True, ax=ax)
    nx.draw_networkx(_graph, _pos, edge_color=colors, with_labels=False, ax=ax)
    if _gdf is not None:
        _gdf.plot(ax=ax, color="black", alpha=0.5)
        ctx.add_basemap(ax, crs=_gdf.crs.to_string(), source=ctx.providers.OpenStreetMap.Mapnik, zoom=15)
    plt.box(False)
    h_time = f"{(__time / 3600):.2f}"
    plt.title(f"Time: ${(__time / 3600):.2f} \\ h$")
    plt.savefig(f"./temp_img/{h_time}.png", dpi=300, bbox_inches="tight")
    return (__time, f"./temp_img/{h_time}.png")


if __name__ == "__main__":
    parser = ArgumentParser(
        description="Script to generate a road network evolution GIF."
    )
    parser.add_argument(
        "--adj-matrix",
        type=str,
        default=None,
        required=False,
        help="Path to the adjacency matrix file.",
    )
    parser.add_argument(
        "--coordinates",
        type=str,
        default=None,
        required=False,
        help="Path to the coordinates file.",
    )
    parser.add_argument(
        "--densities",
        type=str,
        default=None,
        required=False,
        help="Path to the input density csv.",
    )
    parser.add_argument(
        "--use-basemap",
        type=bool,
        default=False,
        required=False,
        help="Use basemap for plotting.",
    )
    args = parser.parse_args()
    # Load the graph
    # read the adjacency matrix discarding the first line
    RESPONSE = args.adj_matrix
    if RESPONSE is None:
        RESPONSE = askopenfilename(
            title="Select the adjacency matrix file",
            filetypes=[("DAT files", "*.dat")],
        )
    adj = np.loadtxt(RESPONSE, skiprows=1)
    n = len(adj)
    RESPONSE = args.coordinates
    if RESPONSE is None:
        RESPONSE = askopenfilename(
            title="Select the coordinates file", filetypes=[("CSV files", "*.csv")]
        )
    coord = pd.read_csv(RESPONSE, sep=";")
    coord = coord.set_index("nodeId")
    RESPONSE = args.densities
    if RESPONSE is None:
        RESPONSE = askopenfilename(
            title="Select the input densities file",
            filetypes=[("CSV files", "*.csv")],
        )
    gdf = None
    if args.use_basemap:
        # draw city map
        gdf = gpd.GeoDataFrame(coord, geometry=gpd.points_from_xy(coord.lon, coord.lat), crs="EPSG:4326")

    G, edges, pos = create_graph_from_adj(adj, coord)

    font = ImageFont.truetype(FONT_PATH, 35)

    df = pd.read_csv(RESPONSE, sep=";")
    df = df.set_index("time")
    # remove last column
    df = df.iloc[:, :-1]

    # take only rows with index % 300 == 0
    df = df[df.index % TIME_GRANULARITY == 0]
    if TIME_BEGIN is not None:
        df = df[df.index > TIME_BEGIN]
        # take N_FRAMES from the beginning
        df = df.head(N_FRAMES)
    else:
        # take the last N_FRAMES
        df = df.tail(N_FRAMES)

    # check if the temp_img folder exists, if not create it
    pathlib.Path("./temp_img").mkdir(parents=True, exist_ok=True)

    with mp.Pool(1) as pool:
        frames = []
        jobs = []

        for time in df.index:
            jobs.append(
                pool.apply_async(
                    create_image,
                    (
                        df,
                        time,
                        G,
                        pos,
                        edges,
                        n,
                        gdf,
                    ),
                )
            )

        # use tqdm and take results:
        results = [job.get() for job in tqdm(jobs)]
        results = sorted(results, key=lambda x: x[0])
        # frames = [result[1] for result in results]
        frames = [Image.open(result[1]) for result in tqdm(results)]

        # if NFRAMES is 1, save a png image
        if N_FRAMES == 1:
            frames[0].save(OUTPUT_FILE_NAME.replace(".gif", ".png"), format="PNG")
        else:
            # Save into a GIF file that loops forever
            frames[0].save(
                OUTPUT_FILE_NAME,
                format="GIF",
                append_images=frames[1:],
                save_all=True,
                duration=300,
                loop=0,
            )

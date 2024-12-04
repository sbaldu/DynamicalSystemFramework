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
from PIL import ImageFont
import pandas as pd
import geopandas as gpd
import contextily as ctx
import cv2

# Constants
COLORMAP = colormaps["RdYlGn_r"]

# if on wsl
FONT_PATH = ""
if platform.system() == "Linux":
    FONT_PATH = "/mnt/c/Windows/Fonts/arial.ttf"
elif platform.system() == "Darwin":  # MAC OS
    FONT_PATH = "/System/Library/Fonts/Supplemental/Arial.ttf"


def create_image(__row: pd.Series, __graph, __pos, __n, __gdf, __day):
    """
    Generates and saves an image of a graph with edges colored based on density.

    Parameters:
    __row (Series): A pandas DataFrame containing the data.
    __graph (Graph): A networkx Graph object.
    __pos (dict): A dictionary containing the positions of the nodes.
    __n (int): The number of nodes in the graph.
    __gdf (GeoDataFrame): A geopandas GeoDataFrame containing the coordinates of the nodes.

    Returns:
    tuple: A tuple containing the time in seconds and the path to the saved image.
    """
    for col in __row.index:
        index = int(col)
        density = __row[col]
        src = index // __n
        dst = index % __n
        # set color of edge based on density using a colormap from green to red
        __graph[src][dst]["color"] = COLORMAP(density)
        # draw graph with colors
    colors = [__graph[u][v]["color"] for u, v in __graph.edges()]
    # draw graph
    _, ax = plt.subplots(figsize=(16, 9))
    if __gdf is not None:
        limits = __gdf.total_bounds + np.array([-0.001, -0.001, 0.001, 0.001])
        ax.set_xlim(limits[0], limits[2])
        ax.set_ylim(limits[1], limits[3])
    nx.draw_networkx_edges(
        __graph,
        __pos,
        edgelist=__graph.edges(),
        edge_color=colors,
        ax=ax,
        connectionstyle="arc3,rad=0.05",
        arrowsize=10,
        arrowstyle="->",
        width=2.5,
    )
    nx.draw_networkx_nodes(__graph, __pos, ax=ax, node_size=169)
    nx.draw_networkx_labels(__graph, __pos, ax=ax, font_size=12)
    if __gdf is not None:
        # _gdf.plot(ax=ax)
        ctx.add_basemap(
            ax,
            crs=__gdf.crs.to_string(),
            source=ctx.providers.OpenStreetMap.Mapnik,
            alpha=0.5,
        )
    plt.box(False)
    plt.title(
        f"Time: {(__row.name // 3600):02d}:{(__row.name % 3600) // 60:02d} {__day}"
    )
    plt.savefig(
        f"./temp_img/{(__row.name / 3600):.2f}.png", dpi=300, bbox_inches="tight"
    )
    plt.close()
    return (__row.name, f"./temp_img/{(__row.name / 3600):.2f}.png")


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
    parser.add_argument(
        "--time-granularity",
        type=int,
        default=300,
        required=False,
        help="Time granularity in seconds.",
    )
    parser.add_argument(
        "--time-begin",
        type=int,
        default=None,
        required=False,
        help="Time to begin plotting. If None, it will take the last N_FRAMES.",
    )
    parser.add_argument(
        "--n-frames",
        type=int,
        default=None,
        required=False,
        help="Number of frames to generate.",
    )
    parser.add_argument(
        "--day",
        type=str,
        default=None,
        required=False,
        help="Day to plot.",
    )
    parser.add_argument(
        "--fps",
        type=int,
        default=1,
        required=False,
        help="Frames per second for the output video.",
    )
    parser.add_argument(
        "--output-file",
        type=str,
        default="evolution.mp4",
        required=False,
        help="Output file name for the video.",
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
    GDF = None
    if args.use_basemap:
        # draw city map
        GDF = gpd.GeoDataFrame(
            coord, geometry=gpd.points_from_xy(coord.lon, coord.lat), crs="EPSG:4326"
        )
    if args.day is None:
        args.day = "(hh:mm)"

    G = nx.from_numpy_array(adj, create_using=nx.DiGraph())
    pos = {}
    for node in G.nodes():
        pos[node] = (coord.loc[node]["lon"], coord.loc[node]["lat"])

    font = ImageFont.truetype(FONT_PATH, 35)

    df = pd.read_csv(RESPONSE, sep=";")
    df = df.set_index("time")

    # take only rows with index % 300 == 0
    df = df[df.index % args.time_granularity == 0]
    if args.time_begin is not None:
        df = df[df.index > args.time_begin]
        if args.n_frames is not None:
            # take N_FRAMES from the beginning
            df = df.head(args.n_frames)
    elif args.n_frames is not None:
        # take the last N_FRAMES
        df = df.tail(args.n_frames)

    # check if the temp_img folder exists, if not create it
    # force delete the folder if it exists
    if pathlib.Path("./temp_img").exists():
        for file in pathlib.Path("./temp_img").iterdir():
            file.unlink()
    else:
        pathlib.Path("./temp_img").mkdir(parents=True, exist_ok=True)

    with mp.Pool() as pool:
        jobs = []

        for time in df.index:
            jobs.append(
                pool.apply_async(
                    create_image,
                    (
                        df.loc[time],
                        G,
                        pos,
                        n,
                        GDF,
                        args.day,
                    ),
                )
            )

        # use tqdm and take results:
        results = [job.get() for job in tqdm(jobs)]

    results = sorted(results, key=lambda x: x[0])

    # Get the dimensions of the first frame
    first_frame_path = results[0][1]
    frame = cv2.imread(first_frame_path)
    frame_height, frame_width, _ = frame.shape

    # Create video writer
    fourcc = cv2.VideoWriter_fourcc(*"mp4v")
    video_writer = cv2.VideoWriter(
        args.output_file, fourcc, args.fps, (frame_width, frame_height)
    )

    for file_path in tqdm(results):
        frame = cv2.imread(file_path[1])
        video_writer.write(frame)

    video_writer.release()
    print(f"MP4 video saved to: {args.output_file}")

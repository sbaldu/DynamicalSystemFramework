"""
Script to generate a gif from the data in the temp_img folder, produced by the main library.
With this, one can see the evolution of the network over time.
"""

import multiprocessing
import platform
import os
from argparse import ArgumentParser
import networkx as nx
import matplotlib.pyplot as plt
from matplotlib import colormaps
import numpy as np
from tqdm import tqdm
from PIL import Image, ImageFont
import pandas as pd
from functions import create_graph_from_adj

# Constants
TIME_BEGIN = 20 * 3600  # None to take the last frames
N_FRAMES = 1
TIME_GRANULARITY = 300
COLORMAP = colormaps["RdYlGn_r"]

# INPUT_FILE_NAME = 'output_sctl_noOPT/densities.csv'
INPUT_FOLDER = "./05"
OUTPUT_FILE_NAME = "evolution.gif"

N_CORES = 3

# if on wsl
FONT_PATH = ""
if platform.system() == "Linux":
    FONT_PATH = "/mnt/c/Windows/Fonts/arial.ttf"
elif platform.system() == "Darwin":  # MAC OS
    FONT_PATH = "/System/Library/Fonts/Supplemental/Arial.ttf"


def create_image(__df, __time, _graph, _pos, _n):
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
        density = __df.loc[__time][col] / (225 / 2000)
        src = index // _n
        dst = index % _n
        # set color of edge based on density using a colormap from green to red
        _graph[src][dst]["color"] = COLORMAP(density)
        # draw graph with colors
    colors = [_graph[u][v]["color"] for u, v in edges]
    # draw graph
    _, ax = plt.subplots(figsize=(10, 10))
    nx.draw(_graph, _pos, edge_color=colors, with_labels=True, ax=ax)
    plt.box(False)
    h_time = f"{(__time / 3600):.2f}"
    print(h_time)
    plt.title(f"Time: ${(__time / 3600):.2f} \\ h$")
    plt.savefig(f"./temp_img/{h_time}.png", dpi=300, bbox_inches="tight")
    return (__time, f"./temp_img/{h_time}.png")


if __name__ == "__main__":
    parser = ArgumentParser(
        description="Script to generate a road network evolution GIF."
    )
    parser.add_argument(
        "--adj_matrix",
        type=str,
        required=True,
        help="Path to the adjacency matrix file.",
    )
    parser.add_argument(
        "--coordinates",
        type=str,
        required=True,
        help="Path to the coordinates file.",
    )
    args = parser.parse_args()
    # Load the graph
    # read the adjacency matrix discarding the first line
    adj = np.loadtxt(args.adj_matrix, skiprows=1)
    n = len(adj)
    # read the coordinates
    coord = np.loadtxt(args.coordinates)

    G, edges, pos = create_graph_from_adj(adj, coord)

    font = ImageFont.truetype(FONT_PATH, 35)

    # open densities.csv file
    # read the densities
    # for each time, create a new image with the graph and the densities
    # save the images in the temp_img folder
    # create a gif from the images in the temp_img folder
    df_array = []
    for folder in os.listdir(INPUT_FOLDER):
        if os.path.isdir(os.path.join(INPUT_FOLDER, folder)):
            # clear temp
            temp = pd.read_csv(
                INPUT_FOLDER + "/" + folder + "/" + "densities.csv", sep=";"
            )
            temp = temp.set_index("time")
            # remove last column
            temp = temp.iloc[:, :-1]
            # temp = temp * 1000

            # take only rows with index % 300 == 0
            temp = temp[temp.index % TIME_GRANULARITY == 0]
            df_array.append(temp)

    if len(df_array) > 0:
        df = pd.concat(df_array)
        df = df.groupby(df.index).mean()

    # print(df.head())
    # df = pd.read_csv(INPUT_FILE_NAME, sep=";")
    # df = df.set_index('time')
    # # remove last column
    # df = df.iloc[:,:-1]

    # take only rows with index % 300 == 0
    # df = df[df.index % TIME_GRANULARITY == 0]
    if TIME_BEGIN is not None:
        df = df[df.index > TIME_BEGIN]
        # take N_FRAMES from the beginning
        df = df.head(N_FRAMES)
    else:
        # take the last N_FRAMES
        df = df.tail(N_FRAMES)

    with multiprocessing.Pool(N_CORES) as pool:
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
                        n,
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

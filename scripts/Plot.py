import pandas as pd
import matplotlib.pyplot as plt
import numpy as np


def plot_1d(df):
    plt.figure()
    plt.plot(df["x"], df["phi"])
    plt.xlabel("x")
    plt.ylabel("phi")
    plt.title("1D Finite Volume Solution")
    plt.grid()
    plt.show()


def plot_2d(df):
    # infer structured grid size
    x_unique = np.unique(df["x"])
    y_unique = np.unique(df["y"])

    nx = len(x_unique)
    ny = len(y_unique)

    # safety check (important for debugging silent mesh issues)
    if nx * ny != len(df):
        raise ValueError("Data is not a structured grid (nx*ny mismatch)")

    Z = df["phi"].values.reshape(ny, nx)

    plt.figure()
    plt.imshow(
        Z,
        origin="lower",
        aspect="auto",
        extent=[
            df["x"].min(),
            df["x"].max(),
            df["y"].min(),
            df["y"].max()
        ]
    )
    plt.colorbar(label="phi")
    plt.xlabel("x")
    plt.ylabel("y")
    plt.title("2D Finite Volume Solution")
    plt.show()

def plot_debug(df):
    import matplotlib.pyplot as plt

    if "y" in df.columns:
        # 2D residual heatmap
        x = np.unique(df["x"])
        y = np.unique(df["y"])

        Z = df["residual"].values.reshape(len(y), len(x))

        plt.imshow(Z, origin="lower", cmap="coolwarm")
        plt.colorbar(label="Residual")
        plt.title("FV Residual Field")
        plt.show()
    else:
        plt.plot(df["x"], df["residual"])
        plt.title("Residual (1D)")
        plt.grid()
        plt.show()

def main(filename="solution.csv"):
    df = pd.read_csv(filename)

    cols = set(df.columns)

    if {"x", "phi"} <= cols:
        plot_1d(df)

    elif {"x", "y", "phi"} <= cols:
        plot_2d(df)

    elif "residual" in df.columns:
        plot_debug(df)

    else:
        raise ValueError(f"Unknown CSV format: columns = {df.columns}")


if __name__ == "__main__":
    main()

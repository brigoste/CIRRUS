import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import sys
import os

def plot_1d(x,phi):
    plt.figure()
    plt.scatter(x, phi)
    plt.plot(x, phi)
    plt.xlabel("x")
    plt.ylabel("phi")
    plt.title("1D Finite Volume Solution")
    plt.grid()
    plt.show()

def plot_2d(x,y,phi):
    plt.tricontourf(x, y, phi, levels=50)#, color="viridis")
    plt.colorbar(label="Field")
    plt.axis("equal")
    plt.title("2D Finite Volume Solution")
    plt.show()


def plot_debug(df):
    x = df[:, 0]
    y = df[:, 1]
    res = df[:, 3]   # adjust if residual is separate column

    if np.allclose(y, y[0]):
        plt.plot(x, res)
        plt.title("Residual (1D)")
    else:
        xi = np.unique(x)
        yi = np.unique(y)

        Z = res.reshape(len(yi), len(xi))

        plt.imshow(Z, origin="lower", cmap="coolwarm", aspect="auto")
        plt.colorbar(label="Residual")
        plt.title("Residual Field (2D)")

    plt.show()


def main(filename):
    df = np.loadtxt(filename, delimiter=",", skiprows=1)

    x = df[:, 0]
    y = df[:, 1]
    z = df[:, 2]
    phi = df[:, 3]

    idx = np.argsort(x)
    x = x[idx]
    y = y[idx]
    z = z[idx]
    phi = phi[idx]

    is_1d = np.allclose(y, y[0]) and np.allclose(z, 0)
    is_2d = not is_1d

    if is_1d:
        plot_1d(x,phi)
    elif is_2d:
        plot_2d(x,y,phi)
    else:
        raise ValueError("Unknown dimensional structure")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        main(sys.argv[1])
    else:
        main("output\\solution.csv")

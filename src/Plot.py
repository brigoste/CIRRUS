import numpy as np
import matplotlib.pyplot as plt
import sys
from pathlib import Path

print("Running Python:", sys.executable)

def main():
    # Anchor everything to this script location (NOT working directory)
    base_dir = Path(__file__).resolve().parent
    data_file = base_dir / "output" / "solution.csv"

    data = np.loadtxt(data_file, delimiter=',', skiprows=1)

    if data.ndim == 1:
        data = data[np.newaxis, :]

    x = data[:, 0]
    tdma = data[:, 1]
    # gs = data[:, 2]

    plt.figure(figsize=(8, 5))
    plt.plot(x, tdma, label='TDMA', lw=2)
    plt.scatter(x, tdma)

    # plt.plot(x, gs, label='GS', lw=1)

    plt.xlabel('x')
    plt.ylabel('T')
    plt.title('1D Heat Transfer: TDMA')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()

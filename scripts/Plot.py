import matplotlib.pyplot as plt
import numpy as np

data = np.genfromtxt("output/solution.csv", delimiter=",", skip_header=1)

x = data[:, 0]
y = data[:, 1]
z = data[:, 2]
value = data[:, 3]

# detect dimension automatically
if df["y"].isna().all():
    # 1D
    plt.plot(df["x"], df["value"])
    plt.xlabel("x")
    plt.ylabel("T")
    plt.show()

elif df["z"].isna().all():
    # 2D scatter
    plt.scatter(df["x"], df["y"], c=df["value"])
    plt.colorbar(label="T")
    plt.show()

else:
    # 3D (placeholder)
    print("3D not implemented yet")

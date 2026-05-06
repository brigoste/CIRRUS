import numpy as np
import matplotlib.pyplot as plt
import sys
from pathlib import Path
try:
    from scipy.interpolate import CubicSpline
    HAS_SCIPY = True
except ImportError:
    HAS_SCIPY = False

print("Running Python:", sys.executable)

# ============================================================
# Data loading
# ============================================================
from pathlib import Path
import numpy as np

def load_data():
    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parent
    data_file = project_root / "output" / "solution.csv"

    metadata = {}

    data_lines = []
    header = None

    with open(data_file, "r") as f:
        for line in f:
            line = line.strip()

            # -------------------------
            # 1. Metadata lines
            # -------------------------
            if line.startswith("#"):
                line = line[1:].strip()
                if "=" in line:
                    key, value = line.split("=", 1)
                    metadata[key.strip()] = value.strip()
                continue

            # -------------------------
            # 2. Header line
            # -------------------------
            if header is None:
                header = line
                continue

            # -------------------------
            # 3. Data lines
            # -------------------------
            if line:
                data_lines.append(line)

    # Convert numeric block
    from io import StringIO
    data = np.loadtxt(StringIO("\n".join(data_lines)), delimiter=",")

    x = data[:, 0]
    T = data[:, 1]

    A = np.ones_like(x)

    return x, T, A, metadata


# ============================================================
# Geometry preprocessing (future 2D/3D hook)
# ============================================================
def smooth_geometry(x, A, n_points=10):
    """
    Geometry smoothing:
    - Uses CubicSpline if SciPy is available
    - Falls back to linear interpolation if not
    """

    x_s = np.linspace(x.min(), x.max(), n_points)
    # x_s = np.linspace(x.min(), x.max(), n_points * np.length(x))  # this is what chat-gpt did, and I don't like it.

    if HAS_SCIPY:
        cs = CubicSpline(x, A)
        A_s = cs(x_s)
        # print("Using CubicSpline (SciPy)")
    else:
        A_s = np.interp(x_s, x, A)
        # print("SciPy not found → using linear interpolation")

    return x_s, A_s


# ============================================================
# Field mapping
# ============================================================
def interpolate_field(x, T, x_new):
    """
    Conservative-safe default: linear interpolation for physics field.
    """
    return np.interp(x_new, x, T)


# ============================================================
# 1D rendering (your current case)
# ============================================================
def render_1d(x, T, A, solver_name):

    fig, ax = plt.subplots(figsize=(8, 5))

    # -----------------------------
    # Pseudo-2D contour overlay
    # -----------------------------
    max_T = np.max(T)
    min_T = np.min(T)
    diff_T = max_T - min_T
    mid_T = (max_T + min_T)/2

    thickness = 0.05*diff_T
    y_l = mid_T - thickness
    y_u = mid_T + thickness

    x_2d = np.vstack((x, x))
    y_2d = np.vstack((
        y_l * np.ones_like(x),
        y_u * np.ones_like(x)
    ))
    T_2d = np.vstack((T, T))

    cf = ax.contourf(x_2d, y_2d, T_2d, cmap='plasma', alpha=0.8)
    # ax.contour(x_2d, y_2d, T_2d, colors='k', linewidths=0.5)

    # -----------------------------
    # Line + scatter (1D solution)
    # -----------------------------
    ax.plot(x, T, label=solver_name, lw=2, color='k')
    ax.scatter(x, T, s=20,color='k')

    # -----------------------------
    # Formatting
    # -----------------------------
    ax.set_xlabel("x (m)")
    ax.set_ylabel(r"T ($^\circ$C)")
    ax.set_title(f"1D Heat Transfer: {solver_name}")
    ax.legend()
    # ax.set_yticks([])

    fig.colorbar(cf, ax=ax, label=r"Temperature ($^\circ$C)")

    plt.tight_layout()
    plt.show()
# ============================================================
# Main pipeline
# ============================================================
def main():

    solver_name = "Unknown"

    # -------------------------
    # Load data + metadata
    # -------------------------
    x, T, A, meta = load_data()

    # metadata from file
    file_solver = meta.get("solver", "Unknown")
    data_file = sys.argv[1]
    solver_name = sys.argv[2] if len(sys.argv) > 2 else "Unknown"

    x_s, A_s = smooth_geometry(x, A)
    T_s = interpolate_field(x, T, x_s)

    render_1d(x_s, T_s, A_s, solver_name)


if __name__ == "__main__":
    main()

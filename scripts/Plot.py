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
def load_data():
    script_dir = Path(__file__).resolve().parent
    project_root = script_dir.parent
    file_name = "solution.csv"
    data_file = project_root / "output" / file_name

    print("Reading:", file_name)

    data = np.loadtxt(data_file, delimiter=',', skiprows=1)

    if data.ndim == 1:
        data = data[np.newaxis, :]

    x = data[:, 0]
    T = data[:, 1]

    # If you later add area, this will already work
    A = np.ones_like(x)

    return x, T, A


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
def render_1d(x, T, A):
    plt.figure(figsize=(8, 5))

    plt.plot(x, T, label='TDMA', lw=2)
    plt.scatter(x, T, s=20)

    plt.xlabel('x (m)')
    plt.ylabel(r'T ($^\circ$C)')
    plt.title('1D Heat Transfer: TDMA')
    plt.legend()
    plt.grid(True)

    plt.tight_layout()

    diff = 0.45*(np.max(T) - np.min(T))
    min_y_contour = diff + np.min(T)
    max_y_contour = np.max(T) - diff

    # plt.figure(figsize=(8,5))
    x_2d = np.vstack((x,x))
    y_2d_l = min_y_contour*np.ones(np.size(x))*(A/np.max(A));
    y_2d_u = max_y_contour*np.ones(np.size(x))*(A/np.max(A));
    y_2d = np.vstack((y_2d_l,y_2d_u));
    T_s = np.vstack((T,T))
    plt.contourf(x_2d,y_2d,T_s,cmap='plasma')
    c = plt.colorbar()
    c.set_label('Temperature ($^\circ$C)')
    plt.contour(x_2d,y_2d,T_s,cmap='plasma')

    plt.show()


# ============================================================
# Main pipeline
# ============================================================
def main():

    x, T, A = load_data()

    # --------------------------------------------------------
    # Preprocessing stage (future-proofed)
    # --------------------------------------------------------
    x_s, A_s = smooth_geometry(x, A)
    T_s = interpolate_field(x, T, x_s)

    # --------------------------------------------------------
    # Render stage
    # --------------------------------------------------------
    render_1d(x_s, T_s, A_s)


if __name__ == "__main__":
    main()

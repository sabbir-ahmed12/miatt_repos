"""
plot_convergence.py — animate the ITK optimizer convergence as a GIF.

Usage:
    python scripts/plot_convergence.py convergence.csv convergence.gif

The CSV is written by register_circles when a fourth argument is given:
    ./build/register_circles img1.nrrd img2.nrrd registered.nrrd convergence.csv

CSV format (header row + data rows):
    level,iteration,metric,step
"""

import sys
import csv
import math
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from PIL import Image as PILImage
import io


LEVEL_COLORS = ["#e41a1c", "#ff7f00", "#4daf4a", "#377eb8"]
LEVEL_LABELS = ["L0 – 8× shrink, σ=8 mm", "L1 – 4× shrink, σ=4 mm",
                "L2 – 2× shrink, σ=2 mm", "L3 – 1× shrink, σ=0 mm"]


def load_csv(path: str) -> list[dict]:
    rows = []
    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append({
                "level": int(row["level"]),
                "iter":  int(row["iteration"]),
                "metric": float(row["metric"]),
                "step":  float(row["step"]),
            })
    return rows


def make_frame(rows_so_far: list[dict], all_rows: list[dict],
               fig_size=(8, 5)) -> PILImage.Image:
    fig, ax = plt.subplots(figsize=fig_size, dpi=100)

    # Full x-range so axis doesn't jump
    ax.set_xlim(-1, len(all_rows) + 1)
    y_vals = [r["metric"] for r in all_rows if r["metric"] > 0]
    ax.set_ylim(min(y_vals) * 0.5, max(y_vals) * 1.5)
    ax.set_yscale("log")

    ax.set_xlabel("Cumulative iteration", fontsize=12)
    ax.set_ylabel("MSE metric (log scale)", fontsize=12)
    ax.set_title("Registration convergence — 4-level multi-resolution pyramid", fontsize=13)
    ax.grid(True, which="both", linestyle="--", alpha=0.4)

    # Plot visible data, colored by level
    if rows_so_far:
        x, y, c = [], [], []
        for i, r in enumerate(rows_so_far):
            x.append(i)
            y.append(r["metric"])
            c.append(LEVEL_COLORS[min(r["level"], len(LEVEL_COLORS) - 1)])

        # Scatter + line per level segment
        prev_level = rows_so_far[0]["level"]
        seg_x, seg_y = [x[0]], [y[0]]
        for i in range(1, len(rows_so_far)):
            lv = rows_so_far[i]["level"]
            if lv != prev_level:
                ax.plot(seg_x, seg_y,
                        color=LEVEL_COLORS[min(prev_level, len(LEVEL_COLORS) - 1)],
                        linewidth=1.5, alpha=0.8)
                seg_x, seg_y = [], []
                prev_level = lv
            seg_x.append(x[i])
            seg_y.append(y[i])
        ax.plot(seg_x, seg_y,
                color=LEVEL_COLORS[min(prev_level, len(LEVEL_COLORS) - 1)],
                linewidth=1.5, alpha=0.8)

        # Current point marker
        ax.scatter([x[-1]], [y[-1]],
                   color=LEVEL_COLORS[min(rows_so_far[-1]["level"], len(LEVEL_COLORS) - 1)],
                   s=60, zorder=5)

        # Annotate current metric
        ax.annotate(f"{y[-1]:.4f}",
                    xy=(x[-1], y[-1]), xytext=(8, 8), textcoords="offset points",
                    fontsize=9, color="black")

    # Legend
    patches = [mpatches.Patch(color=LEVEL_COLORS[i], label=LEVEL_LABELS[i])
               for i in range(len(LEVEL_COLORS))]
    ax.legend(handles=patches, loc="upper right", fontsize=9)

    buf = io.BytesIO()
    fig.tight_layout()
    fig.savefig(buf, format="png")
    plt.close(fig)
    buf.seek(0)
    return PILImage.open(buf).copy()


def main():
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} convergence.csv convergence.gif")
        sys.exit(1)

    csv_path = sys.argv[1]
    gif_path = sys.argv[2]

    rows = load_csv(csv_path)
    if not rows:
        print("ERROR: CSV is empty")
        sys.exit(1)

    print(f"Loaded {len(rows)} iterations across "
          f"{rows[-1]['level'] + 1} pyramid level(s).")

    frames: list[PILImage.Image] = []

    # Animate: one frame per iteration (subsample for large runs)
    step = max(1, len(rows) // 120)   # ≤120 frames in the GIF
    indices = list(range(0, len(rows), step))
    if indices[-1] != len(rows) - 1:
        indices.append(len(rows) - 1)

    for i, end in enumerate(indices):
        frame = make_frame(rows[:end + 1], rows)
        frames.append(frame)
        if (i + 1) % 20 == 0:
            print(f"  rendered {i + 1}/{len(indices)} frames …")

    # Append a few extra copies of the final frame so it lingers
    for _ in range(5):
        frames.append(frames[-1])

    frames[0].save(
        gif_path,
        save_all=True,
        append_images=frames[1:],
        duration=80,       # ms per frame
        loop=0,            # loop forever
        optimize=False,
    )
    print(f"Saved {len(frames)}-frame GIF → {gif_path}")


if __name__ == "__main__":
    main()

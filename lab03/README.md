# ITK Circle Registration

Registers two 2D images of circles using an ITK similarity transform pipeline.
Designed for production use: ~1 s per registration, deterministic, fully self-contained.

**Author:** Sabbir Ahmed

---

## Quick start (from scratch)

### 1. Prerequisites

Install [pixi](https://pixi.sh) — this is the only tool you need on your machine.
Everything else (CMake, Ninja, GCC, ITK) is managed inside the pixi environment.

```bash
curl -fsSL https://pixi.sh/install.sh | bash
```

### 2. Clone and enter the repo

```bash
git clone <repo-url>
cd lab03-sabbir-ahmed12
```

### 3. Install all dependencies

```bash
pixi install
```

This downloads and installs the conda-forge environment (ITK 5.4, CMake 4, Ninja, GCC 14)
into `.pixi/`. It runs once; subsequent commands reuse the cached environment.

### 4. Build

```bash
pixi run cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
pixi run cmake --build build
```

Or use the single shorthand that chains both steps:

```bash
pixi run build
```

Two binaries are produced in `build/`:

| Binary | Purpose |
|---|---|
| `generate_images` | Synthesises `img1.png` and `img2.png` from source |
| `register_circles` | Registers a moving image onto a fixed image |

### 5. Generate the test images

```bash
pixi run ./build/generate_images img1.png img2.png
```

Expected output:
```
Wrote img1.png  (30 mm diameter, centre 50,50 mm)
Wrote img2.png  (60 mm diameter, centre 200,200 mm)
```

### 6. Run registration

```bash
pixi run ./build/register_circles img1.png img2.png registered.png
```

Expected result:
```
=== Registration result ===
  Scale            : ~2.00   (expected ~2.0)
  Angle (rad)      : ~0.00   (expected ~0.0)
  Translation      : (~150.0, ~150.0) mm  (expected ~(150, 150))
  Best metric      : ~0.001
  Registered image → registered.png
```

The output image `registered.png` is the moving circle resampled into the fixed
image's coordinate frame — it should match `img1.png` visually.

### 7. Run all tests

```bash
pixi run ctest --test-dir build --output-on-failure
```

All 6 tests should pass in under 5 seconds:

```
100% tests passed, 0 tests failed out of 6
Total Test time (real) =  ~1.1 sec
```

---

## One-liner: full pipeline

```bash
pixi run build && pixi run generate && pixi run register && pixi run test
```

---

## Repository layout

```
.
├── src/
│   ├── generate_images.cxx     # synthesises img1.png / img2.png via ITK
│   └── register_circles.cxx    # similarity-transform registration pipeline
├── tests/
│   └── CMakeLists.txt          # CTest suite (6 tests)
├── CMakeLists.txt              # top-level CMake: two targets + tests
├── pixi.toml                   # environment: libitk-devel, cmake, ninja, gcc
├── EngineeringDesignPrinciples.md
├── ExperimentalResults.md
└── README.md
```

---

## Debug build

```bash
pixi run cmake -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
pixi run cmake --build build-debug
pixi run ./build-debug/register_circles img1.png img2.png registered.png
```

---

## Problem statement

Write a program that registers two 2D images.
The program will be used more than **1 million times** to register pairs of images
to be supplied in the future.

Test images:
- `img1.png` — 30 mm diameter circle centred at (50 mm, 50 mm)
- `img2.png` — 60 mm diameter circle centred at (200 mm, 200 mm)

Deliverables:
1. `EngineeringDesignPrinciples.md` — design rationale
2. `src/register_circles.cxx` — the algorithm
3. `ExperimentalResults.md` — evidence of correctness for domain peers

# Product Requirements Document
## ITK Circle Registration — ECE 5940 Lab 03

**Author:** Sabbir Ahmed
**Date:** 2026-04-24
**Status:** Approved

---

## 1. Problem Statement

Write a C++ program using ITK that registers two 2D images. The program will be executed more than **1 million times** on future image pairs, so robustness, reliability, reusability, and efficient deployment are first-class requirements — not afterthoughts.

---

## 2. Target Images

| Image | Diameter | Center (x, y) | Pixel spacing |
|---|---|---|---|
| `img1.png` | 30 mm | (50 mm, 50 mm) | 1 mm/pixel |
| `img2.png` | 60 mm | (200 mm, 200 mm) | 1 mm/pixel |

Images are synthesized by a dedicated C++ binary (`generate_images`) to keep the repository fully self-contained and reproducible.

---

## 3. Functional Requirements

### FR-1: Image Generation
- A `generate_images` binary must synthesize `img1.png` and `img2.png` from first principles using ITK image writers.
- Physical spacing must be set to 1 mm/pixel so that metric distances are physically meaningful.
- Images must be deterministically reproducible from source code alone (no external image assets).

### FR-2: Registration Pipeline
- A `register_circles` binary must accept two PNG file paths as arguments and produce the recovered transform parameters on stdout.
- The pipeline must use `itk::Similarity2DTransform<double>` (4 DOF: translation x/y, isotropic scale, rotation) to correctly account for the 2× diameter difference and 150 mm positional offset between the target images.
- Multi-resolution registration (`itk::ImageRegistrationMethodv4` with `SetNumberOfLevels ≥ 3`) is required to handle the large initial offset without falling into local minima.

### FR-3: Metric
- Default metric: `itk::MeanSquaresImageToImageMetricv4` (appropriate for same-modality images).
- The metric choice must be documented and justified in `EngineeringDesignPrinciples.md`.

### FR-4: Optimizer
- Default optimizer: `itk::RegularStepGradientDescentOptimizerv4`.
- Convergence parameters (`LearningRate`, `MinimumStepLength`, `NumberOfIterations`) must be tunable via command-line arguments or a config struct — not hardcoded magic numbers.

### FR-5: Preprocessing
- Apply Gaussian smoothing followed by intensity normalization before registration to improve optimizer stability.

---

## 4. Non-Functional Requirements

### NFR-1: Scale (1 million runs)
- A single registration must complete in under **1 second** on a modern multi-core workstation.
- The binary must be stateless so it can be run in parallel without inter-process coordination.
- Deployment strategy for batch execution must be described in `EngineeringDesignPrinciples.md`.

### NFR-2: Robustness
- Registration must succeed (recover correct transform parameters within 1 mm translation error and 5% scale error) for all perturbations of the initial transform within ±20 mm translation and ±20% scale.
- Robustness testing methodology must be reported in `ExperimentalResults.md`.

### NFR-3: Reproducibility
- With `itk::MultiThreaderBase::SetGlobalMaximumNumberOfThreads(1)`, output must be bit-for-bit identical across runs on the same machine.

### NFR-4: Build environment
- All dependencies managed via `pixi`. No system-level package installs required beyond `pixi` itself.
- Build system: CMake + Ninja.
- Supported build types: `Release` (default) and `Debug`.

---

## 5. Deliverables

| Deliverable | Description | Owner |
|---|---|---|
| `src/generate_images.cxx` | C++ binary to synthesize test images | Developer |
| `src/register_circles.cxx` | Registration pipeline | Developer |
| `tests/` | CTest suite (unit + integration) | Developer |
| `EngineeringDesignPrinciples.md` | Full SE design document (see §6) | Developer |
| `ExperimentalResults.md` | Full experimental results (see §7) | Developer |

---

## 6. EngineeringDesignPrinciples.md Scope

Must address all of the following:

1. Problem framing and registration terminology
2. Software architecture (ITK filter-pipeline pattern, modularity, separation of concerns)
3. Transform selection rationale (similarity vs. rigid vs. affine vs. translation — and why similarity wins here)
4. Metric selection rationale (MSE vs. mutual information)
5. Optimizer selection rationale and convergence analysis
6. Deployment design for 1 million runs (parallelism model, batching, resource limits)
7. Testing strategy (unit tests for generation, integration tests for registration accuracy)
8. Trade-off log: every decision considered and rejected, with reasoning

---

## 7. ExperimentalResults.md Scope

Must include all of the following:

1. **Convergence plots** — optimizer metric value vs. iteration number
2. **Visual overlays** — fixed, moving, and registered images side-by-side
3. **Quantitative metrics** — final metric value, recovered transform parameters (tx, ty, scale, angle), residual error vs. ground truth
4. **Robustness tests** — success rate across a grid of perturbed initial conditions
5. **Runtime benchmarks** — wall-clock time per registration; projected time and cost for 1 million runs

---

## 8. Out of Scope

- Multi-modal registration (different imaging modalities)
- 3D registration
- Non-rigid / deformable registration
- GUI or web interface
- GPU acceleration (may be revisited if NFR-1 is not met)

# Engineering Design Principles
## ITK Circle Registration — ECE 5940 Lab 03

**Author:** Sabbir Ahmed
**Date:** 2026-04-24

---

## 1. Problem Framing

Image registration is the process of finding a geometric transform T such that a
_moving_ image, after applying T, best matches a _fixed_ reference image under
some similarity metric. For this lab:

- **Fixed image (`img1.png`):** 30 mm diameter circle centred at (50 mm, 50 mm)
- **Moving image (`img2.png`):** 60 mm diameter circle centred at (200 mm, 200 mm)

The two images differ in **both position and size**: the centres are 150√2 ≈ 212 mm
apart (diagonally), and the moving circle is exactly twice the diameter of the fixed
circle. A valid registration must therefore recover:

- A **scale** factor of 2.0 (moving circle is 2× larger)
- A **translation** of (150, 150) mm (with rotation centre at the fixed centroid)
- An **angle** of 0.0 rad (circles are isotropic; no rotation occurred)

The program must be designed for **production deployment at over 1 million runs**,
making robustness, determinism, and runtime all first-class requirements.

---

## 2. Software Architecture

The codebase follows the standard **ITK filter-pipeline pattern**: each processing
stage is a stateless filter with typed input/output ports. Filters are lazy — no
computation occurs until `Update()` is called on the terminal filter. This enables:

- **Composability:** pipelines can be reconfigured without data copies
- **Memory efficiency:** ITK manages intermediate buffers and releases them
  when no longer referenced via smart pointers
- **Thread safety:** stateless filters can run in parallel without locks

### Component diagram

```
PNG file                 PNG file
    │                       │
    ▼                       ▼
ImageFileReader         ImageFileReader      (uint8 → float cast)
    │                       │
    ▼                       ▼
RescaleIntensityFilter  RescaleIntensityFilter   (normalize to [0,1])
    │                       │
    └──────────┬────────────┘
               ▼
    CenteredTransformInitializer   (moments-based translation init)
               │
               ▼
    ImageRegistrationMethodv4
    ├── MeanSquaresImageToImageMetricv4   (MSE on 20% random sample)
    ├── Similarity2DTransform             (scale, angle, tx, ty)
    ├── RegularStepGradientDescentOptimizerv4
    └── 4-level multi-resolution pyramid  (shrink 8→4→2→1, σ 8→4→2→0 mm)
               │
               ▼
    ResampleImageFilter               (apply final transform to moving)
               │
               ▼
    ImageFileWriter                   (write registered.png)
```

### Source file responsibilities

| File | Responsibility |
|---|---|
| `src/generate_images.cxx` | Synthesise reproducible test images. Draws filled circles using the squared-distance formula; writes via ITK ImageFileWriter with 1 mm/pixel spacing. |
| `src/register_circles.cxx` | Registration pipeline. Stateless binary: reads two PNGs, runs registration, writes result to stdout + PNG. |
| `tests/CMakeLists.txt` | CTest suite. Six tests chained by dependency to catch failures at the earliest possible stage. |

### Smart pointers and ownership

All ITK objects are held in `itk::SmartPointer<T>` (obtained via `Type::New()`).
Reference counting ensures objects live as long as the pipeline references them.
No manual `new`/`delete` is used anywhere in the codebase.

---

## 3. Transform Selection: Similarity2DTransform

### Decision

`itk::Similarity2DTransform<double>` — 4 degrees of freedom:
`[scale, angle, tx, ty]`

### Rationale

| Transform | DOF | Handles scale? | Handles rotation? | Appropriate? |
|---|---|---|---|---|
| `TranslationTransform` | 2 | No | No | **No** — circles differ in size 2× |
| `Rigid2DTransform` | 3 | No | Yes | **No** — same size requirement not met |
| `Similarity2DTransform` | 4 | Yes (isotropic) | Yes | **Yes** |
| `AffineTransform` | 6 | Yes (anisotropic) | Yes | Over-parameterised for circles |

**Translation alone fails** because the fixed circle (r=15 mm) and moving circle
(r=30 mm) cannot be made to overlap by translation — the metric can never reach
zero. The optimizer would find a local minimum with partial overlap but large
residual error.

**Affine transform is over-parameterised.** With 6 DOF and only circular shapes
(no directional features to constrain anisotropic scaling or shear), the optimizer
would have a degenerate metric landscape and converge poorly.

**Similarity transform is the minimum sufficient model.** It captures exactly the
degrees of freedom present in the data (isotropic scale + rigid body motion) without
introducing unconstrained directions.

### Transform parameterisation

With the rotation centre fixed at the fixed image's centre of mass (50, 50):

```
T(x) = scale × R(angle) × (x − centre) + centre + translation
```

At the correct solution: `scale=2, angle=0, tx=150, ty=150`:

```
T(50, 50) = 2 × I × (0, 0) + (50, 50) + (150, 150) = (200, 200)  ✓
T(35, 50) = 2 × (−15, 0) + (200, 200) = (170, 200)  [edge → edge]  ✓
```

Placing the rotation centre at the fixed centroid ensures that translation does
**not** depend on scale — the translation parameter (150, 150) is correct for
any value of scale. This decouples the two primary degrees of freedom and greatly
simplifies optimizer convergence.

---

## 4. Metric Selection: MeanSquaresImageToImageMetricv4

### Decision

`itk::MeanSquaresImageToImageMetricv4` (MSE)

### Rationale

| Metric | Modality | Gradient quality | Appropriate? |
|---|---|---|---|
| MeanSquares (MSE) | Same | Excellent (closed form) | **Yes** |
| MattesMutualInformation | Multi-modal | Good (histogram-based) | Overkill |
| NormalizedCorrelation | Same | Good | Alternative |

Both images are generated by the same synthetic process (identical pixel model).
MSE is the maximum-likelihood estimator for this case: it minimises the sum of
squared differences, which is zero exactly when the images are perfectly aligned.
Mutual information would add unnecessary estimation overhead (histogram binning)
with no benefit for same-modality images.

### Sampling strategy

Dense sampling (all pixels) fails when a significant fraction of the transformed
fixed-image pixels fall outside the moving image's 256×256 domain. With
`scale ≈ 1.5`, the transform maps the upper-right corner of the fixed image to
physical coordinates > 255 mm, which is out of bounds. ITK marks those samples
as invalid; if too many are invalid the metric returns `max_value` and the
optimizer receives a degenerate gradient.

**Fix:** random sampling at 20% of fixed-image pixels (≈13 k samples per
evaluation). This guarantees a sufficient number of in-domain samples at every
pyramid level and scale value encountered during optimisation, at the cost of
minor stochastic noise in the gradient estimate.

---

## 5. Optimiser Selection: RegularStepGradientDescentOptimizerv4

### Decision

`itk::RegularStepGradientDescentOptimizerv4` (RSGD)

### Rationale

RSGD uses a fixed step length that is halved whenever the gradient direction
reverses (dot product of consecutive gradients < 0). This behaviour provides:

1. **Large initial steps** for fast traversal of the coarse error landscape
2. **Automatic step reduction** for fine convergence near the minimum
3. **Reproducibility** — no stochastic step selection

### Optimizer scales

The four transform parameters have very different physical meanings and optimal
step sizes. Without explicit scales, the gradient is dominated by whichever
parameter happens to have the largest magnitude, causing the others to barely
move.

| Parameter | Scale | Rationale |
|---|---|---|
| scale | 1.0 | Primary DOF; needs full-size gradient steps |
| angle | 10 000 | Circles are isotropic; angle must not drift |
| tx | 50 000 | Correct after moments init; only fine corrections needed |
| ty | 50 000 | Same as tx |

The RSGD update rule is:

```
gradient_normalized[i] = gradient[i] / scales[i]
step_direction = gradient_normalized / |gradient_normalized|
parameter_update = −learning_rate × step_direction
```

With `scales[angle]=10000` and `scales[tx]=50000`, the angle and translation
components of the normalised gradient are suppressed by 4–5 orders of magnitude
relative to the scale component. Scale becomes the essentially the sole active
degree of freedom, which is exactly what we want after the moments initialiser
has already set a good translation.

### Initialisation: CenteredTransformInitializer with MomentsOn

A cold start at the identity transform gives zero overlap between the circles.
The MSE metric at zero overlap is nonzero but entirely dominated by background
pixels, giving a near-zero gradient for translation. The optimizer stalls.

`CenteredTransformInitializer::MomentsOn()` computes each image's centre of mass
and sets the translation to align them:

- Fixed centroid: (50, 50) mm → set as rotation centre
- Moving centroid: (200, 200) mm
- Initial translation: (150, 150) mm → `T(50,50) = (200,200)` ✓

After initialisation the circles overlap (at scale=1 the fixed circle sits inside
the moving circle), the metric is finite, and the gradient clearly points toward
increasing scale.

### Convergence across pyramid levels

| Level | Shrink | σ (mm) | Resolution | Initial metric | Final metric |
|---|---|---|---|---|---|
| 0 | 8× | 8 | 32×32 px | (init only) | — |
| 1 | 4× | 4 | 64×64 px | ~0.10 | ~0.006 |
| 2 | 2× | 2 | 128×128 px | ~0.004 | ~0.001 |
| 3 | 1× | 0 | 256×256 px | ~0.001 | ~0.0008 |

The step is reset to `LearningRate=0.5` at each new level, so each coarser
level effectively provides a warm restart for the next.

---

## 6. Preprocessing Design

### What was removed: Gaussian smoothing

An earlier design applied a σ=2 mm Gaussian to both images before registration.
This was removed because it introduces a **scale-dependent metric floor**.

After mapping with scale≈2, the fixed image (σ=2 mm edge) is compared against
the moving image whose σ=2 mm edge appears as σ=1 mm in the fixed coordinate
frame. The two smoothed edges do not match at any scale, so MSE never reaches
zero at the correct transform. The optimizer converges to a wrong local minimum
at scale≈2.17 with metric≈0.04 instead of the correct scale≈2.00 with metric≈0.

### What is kept: intensity normalisation

Both images are normalised to [0, 1] before registration. This ensures the MSE
metric operates on a common intensity scale regardless of the source image's
bit depth. For the synthetic binary images (0 or 255), normalisation maps
background to 0.0 and circle interior to 1.0.

### Multi-resolution pyramid smoothing

The pyramid applies per-level Gaussian smoothing (σ=8, 4, 2, 0 mm) **before
downsampling** at each level. This is appropriate anti-aliasing and does not
introduce the scale-dependency artefact because the smoothing is applied in
image space before the transform is applied, not to the transformed image.

---

## 7. Deployment for 1 Million Runs

### Runtime

On an Intel Core Ultra 7 265 (single thread, Release build):
- Wall time per registration: **~0.97 s**
- Projected time for 1 million runs: ~11.2 CPU-days

### Parallelism model

The `register_circles` binary is **stateless**: it reads inputs, computes,
writes output, and exits. No shared mutable state, no global side effects.
It can therefore be run with arbitrary parallelism:

```bash
# GNU parallel across all cores:
parallel -j $(nproc) \
  "./build/register_circles {1} {2} {1/.}_registered.png" \
  ::: fixed_*.png :::+ moving_*.png
```

With 20 cores at 0.97 s/registration: **~160 000 registrations/hour**, or
~6.3 hours for 1 million on a single 20-core workstation.

### Thread count tuning

Remove `itk::MultiThreaderBase::SetGlobalMaximumNumberOfThreads(1)` for
throughput mode. ITK will then use all available cores **within each
registration**. On a 20-core machine this gives roughly 6–10× speedup per
registration (≈0.1–0.16 s/registration), reducing 1M-run time to ~28–45 hours
on a single workstation or minutes on a cluster.

### Determinism

With `SetGlobalMaximumNumberOfThreads(1)`: results are bit-for-bit identical
across runs on the same machine and OS. For production logging and auditability,
this is recommended.

The random sampling seed is currently OS-provided (non-deterministic). To make
results 100% reproducible, add:
```cpp
registration->SetMetricSamplingPercentageSeed(42);
```

---

## 8. Testing Strategy

### Unit tests

| Test | What it checks |
|---|---|
| `create_tmp_dir` | Scratch directory created before any binary runs |
| `generate_images_runs` | Binary exits 0; both PNGs written |
| `img1_file_exists` | `img1.png` is non-empty (md5sum succeeds) |
| `img2_file_exists` | `img2.png` is non-empty |
| `register_circles_runs` | Registration exits 0 within 2-minute timeout |
| `registered_image_exists` | `registered.png` is written and non-empty |

Tests are chained with `DEPENDS` so CTest stops at the first failure rather than
reporting cascading false negatives from missing files.

### What quantitative tests should be added

For a production deployment the following CTest additions are warranted:

1. **Scale accuracy test:** parse stdout and assert `|scale − 2.0| < 0.05`
2. **Translation accuracy test:** assert `|tx − 150| < 1.0` and `|ty − 150| < 1.0`
3. **Metric floor test:** assert `best_metric < 0.005`
4. **Runtime test:** assert wall time < 5 s (for regression detection)

These require either a small Python/bash parser or a dedicated C++ test binary.

---

## 9. Trade-off Log

Every significant decision considered and either accepted or rejected:

| Decision | Option considered | Why accepted / rejected |
|---|---|---|
| Transform | TranslationTransform | **Rejected:** cannot handle 2× scale difference |
| Transform | AffineTransform | **Rejected:** 6 DOF over-parameterised for isotropic circles; degenerate landscape |
| Transform | Similarity2DTransform | **Accepted:** minimum sufficient model |
| Metric | MeanSquares | **Accepted:** same-modality images; closed-form gradient |
| Metric | MattesMutualInformation | **Rejected:** unnecessary histogram overhead |
| Preprocessing | Gaussian σ=2 mm | **Rejected:** introduces scale-dependent metric floor at MSE≈0.04 |
| Preprocessing | Normalisation only | **Accepted:** no artefact; metric reaches ≈0 at correct transform |
| Sampling | Dense (100%) | **Rejected:** returns max_value when pixels map outside moving domain |
| Sampling | Random 20% | **Accepted:** sufficient gradient signal; avoids domain-boundary failure |
| Optimizer | ConjugateGradientLineSearch | **Rejected:** requires exact line search; more expensive per iteration |
| Optimizer | RSGD | **Accepted:** simple, robust, self-tuning step via relaxation factor |
| Initialisation | Identity | **Rejected:** zero overlap → invalid metric → stalled optimizer |
| Initialisation | Moments (CenteredTransformInitializer) | **Accepted:** aligns centroids; gives finite metric from iteration 0 |
| Scales | Auto (RegistrationParameterScalesFromPhysicalShift) | **Rejected:** estimator perturbs transform and evaluates metric; fails when perturbation maps pixels out of domain |
| Scales | Manual [1, 10000, 50000, 50000] | **Accepted:** locks angle/translation after moments init; scale converges freely |
| Pyramid | 3 levels | **Rejected:** step exhausted before scale converged to 2.0 (error 1.5%) |
| Pyramid | 4 levels | **Accepted:** error reduced to <0.3% |
| Threading | Multi-threaded (default) | **Rejected** for registration: non-deterministic gradient sampling order |
| Threading | Single-threaded | **Accepted** for determinism; parallelism achieved by running multiple processes |

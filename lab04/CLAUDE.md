# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Goal

ITK-based C++ program that registers two 2D images of circles. The program must be production-grade — it will run over 1 million times on future image pairs. Robustness, reliability, reusability, and efficient deployment are primary concerns.

**Target images (NRRD format — stores spacing, origin, and direction cosines):**
- `img1.nrrd`: 30 mm diameter circle centered at (50 mm, 50 mm) — 50×50 px, 1 mm/px, origin (25,25) mm
- `img2.nrrd`: 60 mm diameter circle centered at (200 mm, 200 mm) — 80×80 px, 1 mm/px, origin (160,160) mm
- Both images have ~10 mm of background padding on each side so the circle fills the frame.
- PNG is not used internally because it cannot store physical spacing or origin metadata.

## Build System

This project uses CMake + Ninja with ITK as the image-processing backend. Use `pixi` for environment/dependency management.

```bash
# Set up environment (first time)
pixi install

# Configure
pixi run cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
pixi run cmake --build build

# Run registration
pixi run ./build/register_circles img1.png img2.png
```

For a Debug build (needed for stepping through ITK pipelines):
```bash
pixi run cmake -B build-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
pixi run cmake --build build-debug
```

## Testing

Tests live under `tests/` and are registered with CTest:
```bash
# Run all tests
pixi run ctest --test-dir build --output-on-failure

# Run a single test by name
pixi run ctest --test-dir build -R <test_name> --output-on-failure
```

## Architecture

The registration pipeline follows the standard ITK filter-pipeline pattern: each stage is a stateless filter connected by `SetInput`/`GetOutput`. Filters are not executed until `Update()` is called on the terminal filter.

Key architectural layers (to be implemented):

1. **Image I/O** — `itk::ImageFileReader` / `itk::ImageFileWriter` typed on `itk::Image<float, 2>`
2. **Preprocessing** — smoothing and normalization filters applied before registration
3. **Metric** — `itk::MeanSquaresImageToImageMetric` or `itk::MattesMutualInformationImageToImageMetric`
4. **Transform** — `itk::Similarity2DTransform<double>` (translation + uniform scale). A pure `TranslationTransform` cannot align circles that differ in size; similarity is the minimal correct choice for this image pair.
5. **Optimizer** — `itk::RegularStepGradientDescentOptimizer` or `itk::ConjugateGradientLineSearchOptimizerv4`
6. **Interpolator** — `itk::LinearInterpolateImageFunction`
7. **Registration framework** — `itk::ImageRegistrationMethodv4` wires metric, transform, optimizer, and interpolator together

All image types should be aliased at the top of each translation unit (e.g., `using ImageType = itk::Image<float, 2>;`) to keep template noise manageable.

## Required Deliverables

Per the assignment:
- `EngineeringDesignPrinciples.md` — design rationale and trade-off decisions
- `ExperimentalResults.md` — results that demonstrate correctness and trustworthiness to domain peers

## ITK-Specific Notes

- ITK uses a smart-pointer / factory pattern: always instantiate via `FilterType::New()` and hold in `FilterType::Pointer`.
- Physical spacing (mm/pixel) must be set correctly on synthetic images so metric distances are meaningful.
- The optimizer's `SetMaximumStepLength` and `SetMinimumStepLength` are the primary convergence knobs; tune these first when debugging registration failures.
- Enable `itk::MultiThreaderBase::SetGlobalMaximumNumberOfThreads(1)` during debugging to get deterministic output.

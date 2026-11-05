# Index and Physical Space Coordinate Transformations

---

## Objectives

- Understand the relationship between voxel index space and physical (world) space in ITK images
- Use all four ITK coordinate transform methods on `itk::Image`
- Build intuition for why continuous indices preserve round-trip accuracy while integer indices do not

---

## Background

Every ITK image lives simultaneously in two coordinate spaces:

| Space | Type | Description |
|---|---|---|
| **Index space** | `ImageType::IndexType` (integers) | Voxel address `[i, j, k]` |
| **Physical space** | `ImageType::PointType` (doubles, mm) | World coordinates `(x, y, z)` |

The mapping between them is encoded in three image properties set at acquisition time:

- **Origin** — physical location of the center of the voxel that would be at index location[0,0,0]
- **Spacing** — physical size (mm) of each voxel
- **Direction** — rotation matrix aligning the image axes to the physical axes

ITK provides four methods on `itk::Image` to move between these spaces:

| Method | Input → Output |
|---|---|
| `TransformIndexToPhysicalPoint` | integer index → physical point |
| `TransformPhysicalPointToIndex` | physical point → nearest integer index |
| `TransformPhysicalPointToContinuousIndex` | physical point → fractional (continuous) index |
| `TransformContinuousIndexToPhysicalPoint` | fractional index → physical point |

The continuous index pair forms an **exact round-trip**: converting a physical point to a continuous index and back recovers the original point to floating-point precision. The integer index pair introduces **quantization error** of up to half the voxel spacing, because the fractional index is rounded to the nearest integer voxel.

---

## Requirements

Complete the starter file `ImageCoordinates/ImageCoordinates.cxx` by filling in each `TODO` section. Your finished program `ImageCoordinates_${MYHAWKID}` must:

1. **Part 1 — `TransformIndexToPhysicalPoint`**
   Given the command-line integer index `(indexI, indexJ, indexK)`, compute and print its physical coordinates in mm.

2. **Part 2 — `TransformPhysicalPointToIndex`**
   Given the command-line physical point `(physX, physY, physZ)`, compute and print the nearest integer index. Check the returned `bool` to confirm the point lies inside the image.

3. **Part 3 — `TransformPhysicalPointToContinuousIndex`**
   For the same physical point, compute and print the continuous (fractional) index.

4. **Part 4 — `TransformContinuousIndexToPhysicalPoint`**
   Convert the continuous index from Part 3 back to a physical point and print it.

5. **Verdddification**
   Compute and print the Euclidean distance (mm) between the original physical point and both round-trip results (via integer index and via continuous index). The continuous round-trip error must be less than `1e-6` mm.

### Command-line Usage

```bash
ImageCoordinates_hawkid \
    --inputVolume /nfsscratch/opt/ece5490/data/Eyes.nrrd \
    --indexI 60 --indexJ 127 --indexK 93 \
    --physX <px> --physY <py> --physZ <pz>
```

### Test Data

```bash
export DATA=/nfsscratch/opt/ece5490/data
```

Run your program with `$DATA/Eyes.nrrd` and the ** index** `[60, 127, 93]`:

```bash
# Step 1: use Part 1 to determine the physical coordinates of a point in the image
ImageCoordinates_hawkid --inputVolume $DATA/Eyes.nrrd \
    --indexI 60 --indexJ 127 --indexK 93 \
    --physX 0.0 --physY 0.0 --physZ 0.0

# Step 2: re-run using the physical coordinates printed in Part 1 as --physX/Y/Z input
#         to verify all four transforms round-trip correctly
```

### Expected Output Format

```
=== Image Metadata ===
Origin:    [...]
Spacing:   [...]
Direction: ...
Size:      [...]

=== Part 1: Index to Physical ===
Index     [60, 127, 93] -> Physical: [...] mm

=== Part 2: Physical to Nearest Integer Index ===
Physical  [...] -> Nearest Index: [...]

=== Part 3: Physical to Continuous Index ===
Physical  [...] -> Continuous Index: [...]

=== Part 4: Continuous Index to Physical (Round-trip) ===
Continuous Index [...] -> Physical: [...] mm

=== Verification ===
Original physical point:                 [...]
Round-trip via continuous index:         [...]
Round-trip via integer index:            [...]
Continuous index round-trip error (mm):  <should be < 1e-6>
Integer index round-trip error (mm):     <may be non-zero>
```

---

## Additional Tasks

- [ ] Set `MYHAWKID` in `hw06/CMakeLists.txt` to your actual HawkID
- [ ] Update the `<category>` in `ImageCoordinates.xml` from `MIATT.HW06.hawkid` to `MIATT.HW06.yourhawkid`
- [ ] `git commit` with meaningful commit messages as you work
- [ ] Answer all essay questions below inline in this README

---

## Essay Questions

```txt
# Essay Question 1:
# In Part 2, when you convert the physical coordinates of Index [60, 127, 93] back
# to an integer index using TransformPhysicalPointToIndex, do you always recover [60, 127, 93]
# exactly? Explain why or why not, referencing the voxel spacing of Eyes.nrrd.

Running the same program 3-4 times, I got the same index value [60, 127, 93] for the given voxel spacing [0.98, 1.1, 1.3]. However, it may not always be the same in practice as the physical coordinates are stored as floating-point numbers, and floating point arithmetic introduces small rounding errors. This is particularly more likely when the voxel spacings are floating point numbers as the index is multiplied with the spacing while converting back to physical point. Moreover, if the physical point lies very close to a voxel boundary, the reverse transform may round to a neighboring integer index.
```

```txt
# Essay Question 2:
# Explain why the continuous index round-trip error (Part 4) is essentially zero while
# the integer index round-trip error may not be. Under what specific condition would
# the integer round-trip also be exact?

The continuous index round-trip error is essentially zero because no rounding occurs-continuous indices preserve sub-voxel precision, so the transform and its inverse cancel out up to floating-point noise. In contrast, the integer path uses `TransformPhysicalPointToIndex` which rounds to the nearest voxel and loses sub-voxel information, introducing error. The integer round-trip is exact only when the original physical point lies exactly at voxel center (i.e., was produced from an integer index).
```

```txt
# Essay Question 3:
# The midterm exam asks you to compute the distance between two eyes in physical space
# given their index locations. Why is it incorrect to compute this distance directly
# in index space using index subtraction, and when would it accidentally give the right answer?

Computing distance in index space is incorrect as the index differences ignore voxel spacing and image orientation, so they do not reflect true physical distances in mm. The correct approach is to convert indices to physical coordinates and then compute the Euclidean distance. Index subtraction would only give the correct answer if spacing is [1, 1, 1] and the direction cosine matrix is identity (i.e., no scaling or rotation).
```

```txt
# Essay Question 4:
# Describe any AI tools used to complete this assignment.

NA
```

```txt
# Essay Question 5:
# Describe any human resources (other than yourself) used to complete this assignment.

NA
```

---

## References

- ITK Software Guide Book 1, Chapter 4 — Image Fundamentals (origin, spacing, direction)
- [ITK Doxygen: `itk::Image` coordinate transform methods](https://itk.org/Doxygen/html/classitk_1_1ImageBase.html)
- [ITK Doxygen: `itk::ContinuousIndex`](https://itk.org/Doxygen/html/classitk_1_1ContinuousIndex.html)
- [Slicer Execution Model Documentation](https://www.slicer.org/wiki/Documentation/Nightly/Developers/SlicerExecutionModel)
- [ITK Examples](https://itk.org/ITKExamples/index.html)

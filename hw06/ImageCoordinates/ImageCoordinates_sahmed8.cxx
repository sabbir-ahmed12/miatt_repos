#include "itkContinuousIndex.h"
#include "itkImageFileReader.h"
#include "ImageCoordinates_sahmed8CLP.h"

int
main(int argc, char * argv[])
{
  PARSE_ARGS;

  constexpr unsigned int Dimension = 3;
  using PixelType = unsigned short;
  using ImageType = itk::Image<PixelType, Dimension>;
  using ReaderType = itk::ImageFileReader<ImageType>;
  using ContinuousIndexType = itk::ContinuousIndex<double, Dimension>;

  auto reader = ReaderType::New();
  reader->SetFileName(inputVolume);
  try
  {
    reader->Update();
  }
  catch (const itk::ExceptionObject & err)
  {
    std::cerr << "Error reading image: " << err << std::endl;
    return EXIT_FAILURE;
  }
  const ImageType::Pointer image = reader->GetOutput();

  // Print image metadata so you can reason about expected coordinate values
  std::cout << "=== Image Metadata ===" << std::endl;
  std::cout << "Origin:    " << image->GetOrigin() << std::endl;
  std::cout << "Spacing:   " << image->GetSpacing() << std::endl;
  std::cout << "Direction:\n" << image->GetDirection();
  std::cout << "Size:      " << image->GetLargestPossibleRegion().GetSize() << std::endl;

  // -----------------------------------------------------------------------
  // Part 1: TransformIndexToPhysicalPoint
  //   Convert a given integer index to its physical (mm) coordinates.
  //   physicalPoint = origin + direction * (index .* spacing)
  // -----------------------------------------------------------------------
  ImageType::IndexType givenIndex;
  givenIndex[0] = static_cast<itk::IndexValueType>(indexI);
  givenIndex[1] = static_cast<itk::IndexValueType>(indexJ);
  givenIndex[2] = static_cast<itk::IndexValueType>(indexK);

  ImageType::PointType physFromIndex;
  // TODO: Call image->TransformIndexToPhysicalPoint(givenIndex, physFromIndex);
  image->TransformIndexToPhysicalPoint(givenIndex, physFromIndex);

  std::cout << "\n=== Part 1: Index to Physical ===" << std::endl;
  std::cout << "Index     [" << givenIndex[0] << ", " << givenIndex[1] << ", " << givenIndex[2]
            << "] -> Physical: " << physFromIndex << " mm" << std::endl;

  // -----------------------------------------------------------------------
  // Part 2: TransformPhysicalPointToIndex
  //   Convert a given physical point to the nearest integer voxel index.
  //   NOTE: TransformPhysicalPointToIndex returns a bool — true if the
  //   resulting index lies within the image's LargestPossibleRegion.
  // -----------------------------------------------------------------------
  ImageType::PointType givenPhysPoint;
  givenPhysPoint[0] = physX;
  givenPhysPoint[1] = physY;
  givenPhysPoint[2] = physZ;

  ImageType::IndexType nearestIndex;
  // TODO: Your code here
  bool isInside = image->TransformPhysicalPointToIndex(givenPhysPoint, nearestIndex);
  // TODO: check the returned bool to verify the point is inside the image.
  if(!isInside) {
    std::cerr << "Warning: Physical point is outside the image." << std::endl;
  }

  std::cout << "\n=== Part 2: Physical to Nearest Integer Index ===" << std::endl;
  std::cout << "Physical  " << givenPhysPoint << " -> Nearest Index: [" << nearestIndex[0] << ", "
            << nearestIndex[1] << ", " << nearestIndex[2] << "]" << std::endl;

  // -----------------------------------------------------------------------
  // Part 3: TransformPhysicalPointToContinuousIndex
  //   Convert a given physical point to a continuous (fractional) index.
  //   Unlike Part 2, this preserves sub-voxel precision.
  // -----------------------------------------------------------------------
  // TODO: Your code here
  ContinuousIndexType continuousIndex;
  bool isInsideContinuous = image->TransformPhysicalPointToContinuousIndex(givenPhysPoint, continuousIndex);
  if (!isInsideContinuous) {
      std::cerr << "Warning: Physical point is outside the image (continuous index)!" << std::endl;
  }

  std::cout << "\n=== Part 3: Physical to Continuous Index ===" << std::endl;
  std::cout << "Physical  " << givenPhysPoint << " -> Continuous Index: [" << continuousIndex[0]
            << ", " << continuousIndex[1] << ", " << continuousIndex[2] << "]" << std::endl;

  // -----------------------------------------------------------------------
  // Part 4: TransformContinuousIndexToPhysicalPoint
  //   Recover the physical point from the continuous index computed in Part 3.
  //   Because no precision was lost in Part 3, this round-trip should be exact.
  // -----------------------------------------------------------------------
  ImageType::PointType roundtripFromContinuous;
  // TODO: Your code here
  image->TransformContinuousIndexToPhysicalPoint(continuousIndex, roundtripFromContinuous);

  std::cout << "\n=== Part 4: Continuous Index to Physical (Round-trip) ===" << std::endl;
  std::cout << "Continuous Index [" << continuousIndex[0] << ", " << continuousIndex[1] << ", "
            << continuousIndex[2] << "] -> Physical: " << roundtripFromContinuous << " mm"
            << std::endl;

  // -----------------------------------------------------------------------
  // Verification: compare round-trip errors for continuous vs integer paths.
  //   Continuous path error should be < 1e-6 mm (floating-point noise only).
  //   Integer path error can be up to half the voxel spacing in each dimension.
  // -----------------------------------------------------------------------
  ImageType::PointType roundtripFromInteger;
  // TODO: Your code here
  image->TransformIndexToPhysicalPoint(nearestIndex, roundtripFromInteger);

  const double continuousError = givenPhysPoint.EuclideanDistanceTo(roundtripFromContinuous);
  const double integerError = givenPhysPoint.EuclideanDistanceTo(roundtripFromInteger);

  std::cout << "\n=== Verification ===" << std::endl;
  std::cout << "Original physical point:                 " << givenPhysPoint << std::endl;
  std::cout << "Round-trip via continuous index:         " << roundtripFromContinuous << std::endl;
  std::cout << "Round-trip via integer index:            " << roundtripFromInteger << std::endl;
  std::cout << "Continuous index round-trip error (mm):  " << continuousError << std::endl;
  std::cout << "Integer index round-trip error (mm):     " << integerError << std::endl;

  return EXIT_SUCCESS;
}

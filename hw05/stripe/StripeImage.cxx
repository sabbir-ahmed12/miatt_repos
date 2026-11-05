#include <iostream>
#include <string>
#include <cstdlib>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"

#include "StripeImageCLP.h"

template <unsigned int Dimension>
int DoStriping(const std::string& inputImage, const std::string& outputImage,
               unsigned int stripeDirection, unsigned short stripeValue,
               unsigned int stripeWidth, unsigned int stripeDistance)
{
  using PixelType = unsigned short;
  using ImageType = itk::Image<PixelType, Dimension>;
  using ReaderType = itk::ImageFileReader<ImageType>;
  using WriterType = itk::ImageFileWriter<ImageType>;

  // Read the Image
  auto reader = ReaderType::New();
  reader->SetFileName(inputImage);
  try
  {
    reader->Update();
  }
  catch (const itk::ExceptionObject& e)
  {
    std::cerr << "Error: Could not read input image.\n" << e << std::endl;
    return EXIT_FAILURE;
  }

  auto image = reader->GetOutput();

  // Modify voxels
  using IteratorType = itk::ImageRegionIteratorWithIndex<ImageType>;
  IteratorType it(image, image->GetRequestedRegion());

  while (!it.IsAtEnd())
  {
    auto index = it.GetIndex();

    if ((index[stripeDirection] % stripeDistance) < stripeWidth)
    {
      it.Set(stripeValue);
    }
    ++it;
  }

  // Write the Image
  auto writer = WriterType::New();
  writer->SetFileName(outputImage);
  writer->SetInput(image);
  try
  {
    writer->Update();
  }
  catch (const itk::ExceptionObject& e)
  {
    std::cerr << "Error: Could not write output image.\n" << e << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


int main( int argc, char *argv[] )
{
  PARSE_ARGS;

  if (dimension < 1 || dimension > 6)
  {
    std::cerr << "Error: Dimension must be between 1 and 6. Provided: " << dimension << "\n";
    return EXIT_FAILURE;
  }
  if (inputImage.empty() || outputImage.empty())
  {
    std::cerr << "Error: Input and Output image paths must be provided.\n";
    return EXIT_FAILURE;
  }
  if (stripeDirection < 0 || stripeDirection >= dimension)
  {
    std::cerr << "Error: Stripe direction (" << stripeDirection
              << ") is invalid for dimension (" << dimension << ").\n";
    return EXIT_FAILURE;
  }
  if (stripeDistance <= 0)
  {
    std::cerr << "Error: Stripe distance must be greater than 0.\n";
    return EXIT_FAILURE;
  }
  if (stripeWidth < 1 || stripeWidth >= stripeDistance)
  {
    std::cerr << "Error: Stripe width must be at least 1 and strictly less than stripe distance.\n";
    return EXIT_FAILURE;
  }

  // Route to correct dimension template
  switch (dimension)
  {
    case 1: return DoStriping<1>(inputImage, outputImage, stripeDirection, stripeValue, stripeWidth, stripeDistance);
    case 2: return DoStriping<2>(inputImage, outputImage, stripeDirection, stripeValue, stripeWidth, stripeDistance);
    case 3: return DoStriping<3>(inputImage, outputImage, stripeDirection, stripeValue, stripeWidth, stripeDistance);
    case 4: return DoStriping<4>(inputImage, outputImage, stripeDirection, stripeValue, stripeWidth, stripeDistance);
    case 5: return DoStriping<5>(inputImage, outputImage, stripeDirection, stripeValue, stripeWidth, stripeDistance);
    case 6: return DoStriping<6>(inputImage, outputImage, stripeDirection, stripeValue, stripeWidth, stripeDistance);
    default: return EXIT_FAILURE;
  }
}

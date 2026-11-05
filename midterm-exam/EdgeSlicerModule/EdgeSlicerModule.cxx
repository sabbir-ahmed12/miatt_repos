
// inclusion of utilized header files
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "EdgeSlicerModuleCLP.h"

#include "itkLaplacianRecursiveGaussianImageFilter.h" // Correct: Required for the LoG filter option
#include "itkSobelEdgeDetectionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"


template <class EdgeFilterType>
typename EdgeFilterType::OutputImageType::Pointer
DoEdgeFiltering(typename EdgeFilterType::InputImageType::Pointer inputImage)
{
  typename EdgeFilterType::Pointer edgeFilter = EdgeFilterType::New();
  edgeFilter->SetInput(inputImage);
  edgeFilter->Update();

  typename EdgeFilterType::OutputImageType::Pointer OutputImage = edgeFilter->GetOutput();

  // Disconnect from pipeline as per README requirements
  OutputImage->DisconnectPipeline();

  return OutputImage;
}

int main(int argc, char** argv)
{
  PARSE_ARGS; // This macro populates inputVolume, outputVolume, and filterKind from XML

  // Fix: Dimension must be 3 for 3D MR images. 6000 is invalid for image processing.
  constexpr size_t Dimension = 3;

  // Fix: Using float. Requirements state non-integer types with positive/negative support.
  using PixelType = float;
  using ImageType = itk::Image<PixelType, Dimension>;

  using ReaderType = itk::ImageFileReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputVolume.c_str()); // inputVolume is declared via PARSE_ARGS/XML

  try {
      reader->Update();
    } catch (itk::ExceptionObject &err) {
      std::cerr << "Error reading input: " << err << std::endl;
      return EXIT_FAILURE;
    }

  ImageType::Pointer myEdgeImage;

  if(filterKind == "LoG")
    {
      std::cout << "Running LoG" << std::endl;
      using LoGFilterType = itk::LaplacianRecursiveGaussianImageFilter<ImageType, ImageType>;
      myEdgeImage = DoEdgeFiltering<LoGFilterType>(reader->GetOutput());
    }
  else if (filterKind == "Sobel")
    {
      std::cout << "Running Sobel" << std::endl;
      using SobelFilterType = itk::SobelEdgeDetectionImageFilter<ImageType, ImageType>;
      myEdgeImage = DoEdgeFiltering<SobelFilterType>(reader->GetOutput());
    }
  else if (filterKind == "GM")
    {
      std::cout << "Running GM" << std::endl;
      using GMFilterType = itk::GradientMagnitudeRecursiveGaussianImageFilter<ImageType, ImageType>;
      myEdgeImage = DoEdgeFiltering<GMFilterType>(reader->GetOutput());
    }
  else
    {
      std::cerr << "INVALID filterKind provided: " << filterKind << std::endl;
      return EXIT_FAILURE;
    }

  using WriterType = itk::ImageFileWriter<ImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput(myEdgeImage);
  writer->SetFileName(outputVolume.c_str());

  try
    {
      writer->Update();
    }
  catch(itk::ExceptionObject &err)
    {
      std::cerr << "Error writing output: " << err << std::endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

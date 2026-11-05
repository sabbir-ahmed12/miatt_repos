#include<iostream>
#include <string>
#include <cstdlib>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"

#include "DOG_sahmed8CLP.h"

int main(int argc, char* argv[]) {
  // Parse command line arguments generated from XML
  PARSE_ARGS;

  if (sigma1 == sigma2) {
    std::cerr << "Error: sigma1 and sigma2 must be different (sigma1 != sigma2)." << std::endl;
    return EXIT_FAILURE;
  }

  // Define Image Types
  constexpr unsigned int Dimension = 3;
  using InternalPixelType = float;
  using OutputPixelType = unsigned char;

  using InternalImageType = itk::Image<InternalPixelType, Dimension>;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;

  // 1. Read the Input Image
  using ReaderType = itk::ImageFileReader<InternalImageType>;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(inputVolume);

  // 2. Setup the two Gaussian Filters
  using GaussianFilterType = itk::SmoothingRecursiveGaussianImageFilter<InternalImageType, InternalImageType>;

  GaussianFilterType::Pointer gaussian1 = GaussianFilterType::New();
  gaussian1->SetInput(reader->GetOutput());
  gaussian1->SetSigma(sigma1);

  GaussianFilterType::Pointer gaussian2 = GaussianFilterType::New();
  gaussian2->SetInput(reader->GetOutput());
  gaussian2->SetSigma(sigma2);

  // 3. Subtract the Gaussians: DOG(f) = G_sigma1(f) - G_sigma2(f)
  using SubtractFilterType = itk::SubtractImageFilter<InternalImageType, InternalImageType, InternalImageType>;
  SubtractFilterType::Pointer subtractFilter = SubtractFilterType::New();
  subtractFilter->SetInput1(gaussian1->GetOutput());
  subtractFilter->SetInput2(gaussian2->GetOutput());

  // 4. Rescale Output to 0-255
  using RescaleFilterType = itk::RescaleIntensityImageFilter<InternalImageType, InternalImageType>;
  RescaleFilterType::Pointer rescaleFilter = RescaleFilterType::New();
  rescaleFilter->SetInput(subtractFilter->GetOutput());
  rescaleFilter->SetOutputMinimum(0);
  rescaleFilter->SetOutputMaximum(255);

  // 5. Cast to unsigned char
  using CastFilterType = itk::CastImageFilter<InternalImageType, OutputImageType>;
  CastFilterType::Pointer castFilter = CastFilterType::New();
  castFilter->SetInput(rescaleFilter->GetOutput());

  // 6. Write the Output
  using WriterType = itk::ImageFileWriter<OutputImageType>;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(outputVolume);
  writer->SetInput(castFilter->GetOutput());

  try {
    writer->Update();
  }
  catch (itk::ExceptionObject & error) {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
#include <iostream>
#include <string>
#include <cstdlib>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"

#include "GradientAD_sahmed8CLP.h"

int main(int argc, char* argv[]) {
  PARSE_ARGS;

  using PixelType  = float;
  using ImageType  = itk::Image<PixelType, 3>;
  using ReaderType = itk::ImageFileReader<ImageType>;
  using WriterType = itk::ImageFileWriter<ImageType>;
  using FilterType = itk::GradientAnisotropicDiffusionImageFilter<ImageType, ImageType>;

  auto reader = ReaderType::New();
  reader->SetFileName(inputVolume);

  auto filter = FilterType::New();
  filter->SetInput(reader->GetOutput());

  filter->SetNumberOfIterations(iterations);
  filter->SetTimeStep(timeStep);
  filter->SetConductanceParameter(conductance);

  auto writer = WriterType::New();
  writer->SetFileName(outputVolume);
  writer->SetInput(filter->GetOutput());

  try {
    writer->Update();
    std::cout << "Filtering complete: " << outputVolume << std::endl;
  }
  catch (itk::ExceptionObject &err) {
    std::cerr << "ITK Exception caught!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
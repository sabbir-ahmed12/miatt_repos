#include<iostream>
#include <string>
#include <cstdlib>

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"

#include "CurvatureAD_sahmed8CLP.h"

int main(int argc, char *argv[]) {
  PARSE_ARGS;

  using PixelType = float;
  static constexpr unsigned int Dimension = 3;
  using ImageType = itk::Image<PixelType, Dimension>;

  using ReaderType = itk::ImageFileReader<ImageType>;
  auto reader = ReaderType::New();
  reader->SetFileName(inputVolume.c_str());

  using FilterType = itk::CurvatureAnisotropicDiffusionImageFilter<ImageType, ImageType>;
  auto filter = FilterType::New();
  filter->SetInput(reader->GetOutput());

  filter->SetNumberOfIterations(iterations);
  filter->SetTimeStep(timeStep);
  filter->SetConductanceParameter(conductance);

  using WriterType = itk::ImageFileWriter<ImageType>;
  auto writer = WriterType::New();
  writer->SetFileName(outputVolume.c_str());
  writer->SetInput(filter->GetOutput());

  try {
    writer->Update();
  }
  catch (itk::ExceptionObject &err) {
    std::cerr << "ExceptionObject caught!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
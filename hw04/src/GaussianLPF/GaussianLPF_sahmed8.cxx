#include <iostream>
#include <string>
#include <cstdlib>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRecursiveGaussianImageFilter.h"
#include "itkCastImageFilter.h"

#include "GaussianLPF_sahmed8CLP.h"

int main(int argc, char* argv[]) {
  PARSE_ARGS;

  constexpr unsigned int Dimension = 3;

  if (sigma <= 0.0) {
    std::cerr << "ERROR: sigma must be > 0." << std::endl;
    return EXIT_FAILURE;
  }

  try {
    using PixelType = float;
    using ImageType = itk::Image<PixelType, Dimension>;

    using ReaderType = itk::ImageFileReader<ImageType>;
    using WriterType = itk::ImageFileWriter<ImageType>;
    using GaussianFilterType = itk::RecursiveGaussianImageFilter<ImageType, ImageType>;

    // Reader
    auto reader = ReaderType::New();
    reader->SetFileName(inputVolume);

    // Apply Gaussian recursively along X
    auto gaussianX = GaussianFilterType::New();
    gaussianX->SetInput(reader->GetOutput());
    gaussianX->SetSigma(sigma);
    gaussianX->SetDirection(0);
    gaussianX->SetNormalizeAcrossScale(false);

    // Then Y
    auto gaussianY = GaussianFilterType::New();
    gaussianY->SetInput(gaussianX->GetOutput());
    gaussianY->SetSigma(sigma);
    gaussianY->SetDirection(1);
    gaussianY->SetNormalizeAcrossScale(false);

    // Then Z
    auto gaussianZ = GaussianFilterType::New();
    gaussianZ->SetInput(gaussianY->GetOutput());
    gaussianZ->SetSigma(sigma);
    gaussianZ->SetDirection(2);
    gaussianZ->SetNormalizeAcrossScale(false);

    // Writer
    auto writer = WriterType::New();
    writer->SetFileName(outputVolume);
    writer->SetInput(gaussianZ->GetOutput());

    writer->Update();
  }
  catch (const itk::ExceptionObject& error) {
    std::cerr << "ITK exception caught:\n" << error << std::endl;
    return EXIT_FAILURE;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Standard exception caught:\n" << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch (...)
  {
    std::cerr << "Unknown exception caught." << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
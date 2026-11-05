#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"

// IO Factories
#include "itkPNGImageIOFactory.h"
#include "itkJPEGImageIOFactory.h"
#include "itkNrrdImageIOFactory.h"
#include "itkNiftiImageIOFactory.h"
#include "itkMetaImageIOFactory.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

namespace
{
    bool ParseUnsigned(const char* str, unsigned int& value)
    {
        char* end = nullptr;
        errno = 0;
        unsigned long parsed = std::strtoul(str, &end, 10);
        if (errno != 0 || end == str || *end != '\0')
            return false;
        value = static_cast<unsigned int>(parsed);
        return true;
    }

    bool ParseDouble(const char* str, double& value)
    {
        char* end = nullptr;
        errno = 0;
        value = std::strtod(str, &end);
        if (errno != 0 || end == str || *end != '\0')
            return false;
        return true;
    }
}

// C++11/14/17-compatible EndsWith
bool EndsWith(const std::string& str, const std::string& suffix)
{
    if (str.length() < suffix.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

template <unsigned int Dimension>
int RecursiveGaussianSmooth(double sigma, const std::string& inputPath, const std::string& outputPath)
{
    using FloatPixelType = float;
    using FloatImageType = itk::Image<FloatPixelType, Dimension>;
    using ReaderType = itk::ImageFileReader<FloatImageType>;
    using FilterType = itk::SmoothingRecursiveGaussianImageFilter<FloatImageType, FloatImageType>;

    auto reader = ReaderType::New();
    reader->SetFileName(inputPath);

    auto filter = FilterType::New();
    filter->SetInput(reader->GetOutput());
    filter->SetSigma(sigma);

    // Determine output type from extension
    std::string lower = outputPath;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c){ return std::tolower(c); });

    bool needsUnsignedChar = EndsWith(lower, ".png") || EndsWith(lower, ".jpg") || EndsWith(lower, ".jpeg");

    try
    {
        if (needsUnsignedChar)
        {
            // Visualization export: rescale to 0-255 and cast to unsigned char
            using OutputPixelType = unsigned char;
            using OutputImageType = itk::Image<OutputPixelType, Dimension>;
            using RescaleType = itk::RescaleIntensityImageFilter<FloatImageType, OutputImageType>;
            using WriterType = itk::ImageFileWriter<OutputImageType>;

            auto rescaler = RescaleType::New();
            rescaler->SetInput(filter->GetOutput());
            rescaler->SetOutputMinimum(0);
            rescaler->SetOutputMaximum(255);

            auto writer = WriterType::New();
            writer->SetFileName(outputPath);
            writer->SetInput(rescaler->GetOutput());
            writer->Update();
        }
        else
        {
            // Scientific output: preserve float
            using WriterType = itk::ImageFileWriter<FloatImageType>;
            auto writer = WriterType::New();
            writer->SetFileName(outputPath);
            writer->SetInput(filter->GetOutput());
            writer->Update();
        }

        std::cout << "Successfully processed " << Dimension << "D image and saved to " << outputPath << std::endl;
    }
    catch (const itk::ExceptionObject& err)
    {
        std::cerr << "ITK Exception:\n" << err << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    itk::PNGImageIOFactory::RegisterOneFactory();
    itk::JPEGImageIOFactory::RegisterOneFactory();
    itk::NrrdImageIOFactory::RegisterOneFactory();
    itk::NiftiImageIOFactory::RegisterOneFactory();
    itk::MetaImageIOFactory::RegisterOneFactory();

    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] << " <dimension> <sigma> <input> <output>" << std::endl;
        return EXIT_FAILURE;
    }

    unsigned int dimension = 0;
    double sigma = 0.0;

    if (!ParseUnsigned(argv[1], dimension))
    {
        std::cerr << "Invalid dimension: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    if (!ParseDouble(argv[2], sigma))
    {
        std::cerr << "Invalid sigma: " << argv[2] << std::endl;
        return EXIT_FAILURE;
    }

    if (sigma <= 0.0)
    {
        std::cerr << "Sigma must be positive." << std::endl;
        return EXIT_FAILURE;
    }

    switch(dimension)
    {
        case 2: return RecursiveGaussianSmooth<2>(sigma, argv[3], argv[4]);
        case 3: return RecursiveGaussianSmooth<3>(sigma, argv[3], argv[4]);
        default:
            std::cerr << "Unsupported dimension: " << dimension << ". Only 2D and 3D are supported." << std::endl;
            return EXIT_FAILURE;
    }
}
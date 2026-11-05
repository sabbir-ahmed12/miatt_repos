#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkRescaleIntensityImageFilter.h>

#include <cstdlib>
#include <iostream>
#include <string>

// Internal representation is float so physical coordinates are exact.
using FloatImageType = itk::Image<float, 2>;
// PNG requires integer pixels; rescale to uint8 before writing.
using ByteImageType  = itk::Image<unsigned char, 2>;

// Creates a 2D image of given pixel dimensions with 1 mm/pixel spacing,
// origin at (0,0), containing a filled circle at the specified physical centre
// and radius (all in mm). Interior pixels are set to 1.0, exterior to 0.0.
static FloatImageType::Pointer
makeCircleImage(double centerX_mm, double centerY_mm, double radius_mm,
                unsigned int width, unsigned int height)
{
    auto image = FloatImageType::New();

    FloatImageType::SizeType size;
    size[0] = width;
    size[1] = height;

    FloatImageType::IndexType start;
    start.Fill(0);

    image->SetRegions(FloatImageType::RegionType(start, size));

    // 1 mm per pixel — physical distances equal pixel distances
    FloatImageType::SpacingType spacing;
    spacing.Fill(1.0);
    image->SetSpacing(spacing);

    FloatImageType::PointType origin;
    origin.Fill(0.0);
    image->SetOrigin(origin);

    image->Allocate(true); // zero-initialize

    const double r2 = radius_mm * radius_mm;
    itk::ImageRegionIteratorWithIndex<FloatImageType> it(image, image->GetBufferedRegion());
    while (!it.IsAtEnd()) {
        const auto& idx = it.GetIndex();
        // With spacing=1 and origin=0, physical coordinate equals index value.
        const double dx = static_cast<double>(idx[0]) - centerX_mm;
        const double dy = static_cast<double>(idx[1]) - centerY_mm;
        if (dx * dx + dy * dy <= r2) {
            it.Set(1.0f);
        }
        ++it;
    }

    return image;
}

// Rescales a float image to [0, 255] and writes it as a PNG.
static void writePng(FloatImageType::Pointer image, const std::string& path)
{
    using RescaleType = itk::RescaleIntensityImageFilter<FloatImageType, ByteImageType>;
    auto rescale = RescaleType::New();
    rescale->SetInput(image);
    rescale->SetOutputMinimum(0);
    rescale->SetOutputMaximum(255);

    using WriterType = itk::ImageFileWriter<ByteImageType>;
    auto writer = WriterType::New();
    writer->SetFileName(path);
    writer->SetInput(rescale->GetOutput());
    writer->Update();
}

int main(int argc, char* argv[])
{
    const std::string path1 = (argc > 1) ? argv[1] : "img1.png";
    const std::string path2 = (argc > 2) ? argv[2] : "img2.png";

    // Image must be large enough to contain both circles without clipping.
    // img1: center (50,50), radius 15 → max extent 65 px
    // img2: center (200,200), radius 30 → max extent 230 px
    // 256×256 contains both comfortably.
    constexpr unsigned int W = 256;
    constexpr unsigned int H = 256;

    try {
        auto img1 = makeCircleImage(50.0, 50.0, 15.0, W, H);
        writePng(img1, path1);
        std::cout << "Wrote " << path1
                  << "  (30 mm diameter, centre 50,50 mm)\n";

        auto img2 = makeCircleImage(200.0, 200.0, 30.0, W, H);
        writePng(img2, path2);
        std::cout << "Wrote " << path2
                  << "  (60 mm diameter, centre 200,200 mm)\n";
    } catch (const itk::ExceptionObject& e) {
        std::cerr << "ITK error: " << e << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

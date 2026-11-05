// Generates img1.nrrd and img2.nrrd as synthetic binary circle images.
//
// Image design (1 mm/pixel isotropic, ~10 mm padding on each side):
//   img1: 50×50 px, origin=(25,25) mm, circle r=15 mm centred at (50,50) mm
//   img2: 80×80 px, origin=(160,160) mm, circle r=30 mm centred at (200,200) mm
//
// NRRD stores spacing, origin and direction cosines, so the registration
// operates entirely in physical (mm) space.

#include <cmath>
#include <iostream>
#include <string>

#include "itkImage.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"

using ImageType = itk::Image<float, 2>;

static ImageType::Pointer MakeCircleImage(
    const itk::Size<2>&       size,
    double                    spacing_mm,
    const ImageType::PointType& origin_mm,
    const ImageType::PointType& center_mm,
    double                    radius_mm)
{
    auto image = ImageType::New();
    ImageType::RegionType region;
    region.SetSize(size);
    image->SetRegions(region);

    ImageType::SpacingType sp;
    sp.Fill(spacing_mm);
    image->SetSpacing(sp);
    image->SetOrigin(origin_mm);
    image->Allocate();
    image->FillBuffer(0.0f);

    itk::ImageRegionIteratorWithIndex<ImageType> it(image, image->GetLargestPossibleRegion());
    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
        ImageType::PointType pt;
        image->TransformIndexToPhysicalPoint(it.GetIndex(), pt);
        const double dx = pt[0] - center_mm[0];
        const double dy = pt[1] - center_mm[1];
        if (dx * dx + dy * dy <= radius_mm * radius_mm)
            it.Set(1.0f);
    }
    return image;
}

int main(int argc, char* argv[])
{
    const std::string out1 = (argc > 1) ? argv[1] : "img1.nrrd";
    const std::string out2 = (argc > 2) ? argv[2] : "img2.nrrd";

    // img1: r=15 mm circle at (50,50) mm — 50×50 px image, 10 mm padding each side
    {
        const itk::Size<2> size = {{ 50, 50 }};
        ImageType::PointType origin;  origin[0]  = 25.0;  origin[1]  = 25.0;
        ImageType::PointType center;  center[0]  = 50.0;  center[1]  = 50.0;

        auto img = MakeCircleImage(size, 1.0, origin, center, 15.0);

        auto writer = itk::ImageFileWriter<ImageType>::New();
        writer->SetFileName(out1);
        writer->SetInput(img);
        writer->Update();
        std::cout << "Wrote " << out1
                  << "  (50x50 px, 1 mm/px, r=15 mm circle at (50,50) mm)\n";
    }

    // img2: r=30 mm circle at (200,200) mm — 80×80 px image, 10 mm padding each side
    {
        const itk::Size<2> size = {{ 80, 80 }};
        ImageType::PointType origin;  origin[0]  = 160.0;  origin[1]  = 160.0;
        ImageType::PointType center;  center[0]  = 200.0;  center[1]  = 200.0;

        auto img = MakeCircleImage(size, 1.0, origin, center, 30.0);

        auto writer = itk::ImageFileWriter<ImageType>::New();
        writer->SetFileName(out2);
        writer->SetInput(img);
        writer->Update();
        std::cout << "Wrote " << out2
                  << "  (80x80 px, 1 mm/px, r=30 mm circle at (200,200) mm)\n";
    }

    return EXIT_SUCCESS;
}

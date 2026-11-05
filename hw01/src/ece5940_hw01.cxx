#include <iostream>
#include "itkImage.h"

int main(int, char * argv[]) {

	// Create a 2D image using unsigned short data
	const unsigned int dimension = 2;
	using PixelType = unsigned short;
	using ImageType = itk::Image<PixelType, dimension>;

	// Create the image
	auto image = ImageType::New();

	// Define the origin
	ImageType::IndexType index;
	index[0] = 0;    // First index on X
	index[1] = 0;    // First index on Y

	// Define the size
	ImageType::SizeType size;
	size[0] = 8;    // Size along X
	size[1] = 8;    // Size along Y

	// Create the region
	ImageType::RegionType region;
	region.SetSize(size);
	region.SetIndex(index);

	// Set the spacing
	ImageType::SpacingType spacing;
	spacing[0] = 4;    // Spacing along X
	spacing[1] = 4;    // Spacing along Y

	// Set the direction to identity
	ImageType::DirectionType direction;
	direction.SetIdentity();

	// Set the pixel value to 5940
	PixelType fillValue = 5940;

	// Pass the region to Image object and allocate memory
	image->SetRegions(region);
	image->Allocate();
	image->SetSpacing(spacing);
	image->SetDirection(direction);
	image->FillBuffer(fillValue);
    
    // Print the size, spacing, origin and pixel value at (4,4)
	const ImageType::RegionType largestRegion = image->GetLargestPossibleRegion();
	const ImageType::SizeType imgSize = largestRegion.GetSize();
	std::cout << "Size: " << imgSize << std::endl;

	const ImageType::SpacingType pixelSpacing = image->GetSpacing();
	std::cout << "Spacing: " << pixelSpacing << std::endl;

	const ImageType::PointType imageOrigin = image->GetOrigin();
	std::cout << "Origin: " << imageOrigin << std::endl;

	constexpr ImageType::IndexType pixelIndex = {{4, 4}};
	const ImageType::PixelType pixelValue = image->GetPixel(pixelIndex);	
	std::cout << "Pixel value at (4, 4) is: " << pixelValue << std::endl;
	
	return 0;
}	

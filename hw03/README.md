# Image Conversion Application Using ITK

# Objective
- To become familiar with the basic mechanisms for building trivial ITK applications.

1. You must review materials in chapters 1-5 of the ITKSoftwareGuide
1. You must answer the questions in 'hw03/src/CMakeLists.txt'
1. You must download, configure, and build your own private version of ITK (see Chapter in ITKSoftwareGuide) 
1. You program for this homework must have the following features:

It must convert a 2D image from one format to another and smooth the image (recursive gaussian) with a specified
$\sigma$ Value

```bash
# HW03.exe <image dimension> <sigmaValue> <input image filename> <output image filename>
% HW03.exe 2 3.7811 input2Dimage.png ouput2Dimage.jpg
```

It must convert a 3D image from one format to another and smooth the image (recursive gaussian) with a specified
$\sigma$ Value

```bash
# HW03.exe <image dimension> <sigmaValue> <input image filename> <output image filename>
% HW03.exe 3 4.2312 input3Dimage.nrrd output3Dimage.nii.gz
```

Useful resources:

* [ITK Examples](https://itk.org/ITKExamples/index.html)
* Useful Example files under ITK source: Examples/IO/

##### HINTS : NOTE: The following code is not valid!!! It is intended *only* to suggest one of many approaches this problem. The code is not ordered properly or complete. 

```cpp
constexpr size_t prog_name_cmdline_index=0;
constexpr size_t img_dim_cmdline_index=1;
constexpr size_t sigma_value_cmdline_index=2;
constexpr size_t input_filename_cmdline_index=3;
constexpr size_t output_filename_cmdline_index=4;

const unsigned long inputDim = std::strtoul( argv[img_dim_cmdline_index], nullptr );
const float sigmaValue = std::strtof( argv[sigma_value_cmdline_index], nullptr )
const std::string inputFilename { argv[input_filename_comdline_index] };
const std::string outputFilename { argv[output_filename_cmdline_index] };

template< Dimension >
void DoWorkHere( inFileName, outFileName )
{
  using PixelType = unsigned char;
}

int main( int argc, char * argv[] )
{
  if( argc < 5 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[prog_name_cmdline_index] << " inputImageDimension sigmaValue inputImageFile outputImageFile" << std::endl;
    return EXIT_FAILURE;
    }
}

try
{
}
catch( itk::ExceptionObject & err )
{
  std::cerr << "ExceptionObject caught !" << std::endl;
  std::cerr << err << std::endl;
  return EXIT_FAILURE;
}
```
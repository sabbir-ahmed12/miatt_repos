// inclusion of utilized header files
#include "itkImageFileReader.h"
#include "ComputeInterPupillaryDistanceCLP.h" //TODO:  XXXX include header file generated from xml
#include <cmath> // HINT: std::sqrt, std::fabs may be useful for distance computation

int main(int argc, char* * argv)
{
  PARSE_ARGS;

  // Definition of input/output pixel types and Dimension variable
  constexpr unsigned int Dimension = 3;
  using InputPixelType = float;

  using InputImageType = itk::Image<InputPixelType, Dimension>;   // Defining Input/Output Image types

  using ReaderType = itk::ImageFileReader<InputImageType>;  // Defining Reader/Writer
  ReaderType::Pointer reader = ReaderType::New();   // Creating a new instance of the Reader/Writer
  reader->SetFileName( inputVolume.c_str() );     // Setting of filenames
  reader->Update();

  InputImageType::Pointer subjectImage = reader->GetOutput();

  InputImageType::IndexType LeftEyeIndex;
  InputImageType::IndexType RightEyeIndex;
  {
    // These are the index locations of the Left and Right Eye
    // for the image /nfsscratch/opt/ece5490/data/Eyes.nrrd
    // They are hard-coded for the midterm exam, and are only
    // relevant for the Eyes.nrrd file.
    LeftEyeIndex[0]=60;
    LeftEyeIndex[1]=127;
    LeftEyeIndex[2]=93;

    RightEyeIndex[0]=59;
    RightEyeIndex[1]=129;
    RightEyeIndex[2]=41;
  }
  //===============================================================================
  //===============================================================================
  //TODO: XXXX  Write code here

  // Convert index locations to physical points
  InputImageType::PointType leftEyePoint;
  InputImageType::PointType rightEyePoint;

  subjectImage->TransformIndexToPhysicalPoint(LeftEyeIndex, leftEyePoint);
  subjectImage->TransformIndexToPhysicalPoint(RightEyeIndex, rightEyePoint);

  // Compute Euclidean distance in physical space
  double distance_between_eyes = std::sqrt(
    std::pow(leftEyePoint[0] - rightEyePoint[0], 2) +
    std::pow(leftEyePoint[1] - rightEyePoint[1], 2) +
    std::pow(leftEyePoint[2] - rightEyePoint[2], 2)
  );

  //===============================================================================
  //===============================================================================
  //NOTE:  The distance between the eyes is a single numerical value.  It is NOT a vector.
  std::cout << "===========================================" << std::endl;
  std::cout << "The distance between the eyes is " << distance_between_eyes << " mm." << std::endl;
  
  return EXIT_SUCCESS;
}

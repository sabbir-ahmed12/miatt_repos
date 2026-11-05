
# Edge Detection

##### In this task, you will write one SlicerExecutionModel program that can select and execute one of three 3D edge detection filters, with their default state values.

(Sobel) - Sobel:  https://itk.org/ITKExamples/src/Filtering/ImageFeature/SobelEdgeDetectionImageFilter/Documentation.html?highlight=sobel

(LoG) - LaplacianRecursiveGaussian:    https://itk.org/ITKExamples/src/Filtering/ImageFeature/LaplacianRecursiveGaussianImageFilter/Documentation.html?highlight=laplacian

(GM) - GradientMagnitudeRecursiveGaussian:     https://itk.org/ITKExamples/src/Filtering/ImageGradient/ComputeGradientMagnitudeRecursiveGaussian/Documentation.html?highlight=gradientmagnitudeimagefilter

Once the program is completed, display the program in the Slicer application, and process the default "MRHead" image using each filter.


* The input and output images must support non-integer types with both positive and negative values.
* You will need to modify the CMakeLists.txt file to compile the components of this exam.
* All computations should be completed on images of type floating point.  All results should be written to disk in floating point format.
* The selection of which filter to use will be based on using one of 3 strings "Sobel", "LoG", or "GM" as the command line argument for the `--filterKind` command line argument.


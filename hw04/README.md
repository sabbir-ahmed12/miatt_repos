# Slicer Execution Model

## Objective:
- To get familiar with building SlicerExecutionModel and use ITK to build extensions for Slicer3D.

```bash
# These commands are only hints ** DO NOT COPY WITHOUT UNDERSTANDING

cd ~/mysrc
git clone https://github.com/InsightSoftwareConsortium/ITK.git
mkdir ITK-bld
cd ITK-bld
cmake -G Ninja ../ITK
ninja

export ITK_DIR=~/mysrc/ITK-bld

cd ~/mysrc
git clone https://github.com/Slicer/SlicerExecutionModel.git
mkdir SEM-bld
cd SEM-bld
cmake -G "Unix Makefiles" -DITK_DIR:PATH=~/mysrc/ITK-bld ../SlicerExecutionModel
make -j 7

export SlicerExecutionModel_DIR=~/mysrc/SEM-bld

cd ~/mysrc
git clone git@github.com:UIOWAECE5490SP19/hw04-{githubid}.git
cd hw04-{githubid}
clion .  # Configure ITK-DIR and SlicerExecutionModelDir

```

```
export DATA=/nfsscratch/opt/ece5490/data
```

Store your source code for this homework in your github repository.  
Please be sure to submit your homework on time (or before the deadline), 
since we  will  be using an automatic script to checkout, compile, and test much of your work.

Many files have already been deposited in your github account to help you get started.
A full description of the program, and proper acknowledgements must be included in the Slicer interface.

The IO parameters must be separate from the Thresholding parameters in the Slicer GUI.
For this homework we will be comparing the noise-reducing and edge-preserving characteristics
 of three different filters.

##	Read Software Guide
 chapters 2 and 3 https://itk.org/ITKSoftwareGuide/html/Book2/ITKSoftwareGuide-Book2.html 
or second book in the pdf https://itk.org/ItkSoftwareGuide.pdf

##	Review the [Slicer Execution Model Documentation](https://www.slicer.org/wiki/Documentation/Nightly/Developers/SlicerExecutionModel)
##	SimpleThreshold

Store the source code for this part of the assignment in the hw04-{githubid}/src/SimpleThreshold subdirectory.
 For this project you will make a simple thresholding filter that reads the input filename,
  output filename, and threshold values from the command line. The command line usage is:
```bash
SimpleThreshold_{hawkid} \
 --inputVolume <input>    --outputVolume <output> \
 --lowThreshold <low>     --highThreshold <high> \
 --outsideValue <outside> --insideValue <inside>
```

where the last four parameters are defined in Figure 6.1 of the ITK Software Guide.
 For example, if the program is run with these arguments:
 
```bash
SimpleThreshold_{hawkid} --outputVolume outimg.nii.gz \
 --inputVolume $DATA/subj1 T1 raw.nii.gz \
  --lowThreshold 50 --highThreshold 100 \
  --outsideValue 150 --insideValue 200
``` 

then the program would read the `NIFTI` file subj1 T1 raw.nii.gz, 
and set every pixel in the range [50, 100] to 200 (the “inside value”) 
and all other pixels to 150 (the “outside value”). The resulting image is written to output file
 outimg.nii.gz in NIFTI format.
Assume the input image is a 3D “NIFTI” format image with a signed short data type. Your program should 
return a value of `EXIT_SUCCESS` if there was no error, `EXIT_FAILURE` with a diagnostic message of what went wrong
otherwise.

The Slicer Module must be displayed in the Slicer module pull down under the sub-menu ”MIATT” ”HW04” ”{hawkid}”,
 where hawkid is your hawkid.

```xml
<category> MIATT.HW04.hawkid</category>
```

## Gaussian low-pass filter (LPF)
In this problem we will use a Gaussian low-pass filter (LPF) to reduce noise in an image. 
Store the source code for this part of the assignment in the hw04-{githubid}/src/GaussianLPF subdirectory.
Write a Slicer Execution Model compliant command line program that reads a 3D input
 image and a filter size parameter (the σ for the Gaussian filter, with σ > 0), 
 and produces a filtered 3D output image where the same σ value
  is applied in the X,Y,Z directions. Use the
ITK filter `itk::RecursiveGaussianImageFilter` as the LPF.
For example, if the program is invoked this way from the command line:

```bash
GaussianLPF_{hawkid} --inputVolume inimg.nii.gz --sigma 2.5 --outputVolume outimg.nii.gz
```

then your program would read the file `inimg.nii.gz`, apply 3D Gaussian LPF with `σx = 2.5`, `σy = 2.5`, `σz = 2.5` 
units to the respective directions, and then write the output file `outimg.nii.gz`.
A few representative noisy test images are available in the $DATA/noisy directory. 
Experiment with various values of σ to see the tradeoff between smoothing and loss of image resolution.
 In a few paragraphs, describe how changing the σ affects smoothing and loss of image resolution
in the file hw04-{githubid}/src/GaussianLPF/README.md.

## Difference of Guassians (DOG)
Store the source code for this part of the assignment in the hw04-{githubid}/src/DOG subdirectory.

We can use the Gaussian LPF to make an edge-enhancing filter by calculating the
 difference between two low-pass filtered images with different σ. Let Gσ() be a 
 symmetric Gaussian LPF with filter parameter σ. Then, the “difference of Gaussians” or
  DOG filter can be written as

```asm
DOG(f ) = Gσ1 (f ) − Gσ2 (f )
```

where `f` is the input image and `DOG(f )` is the edge-enhanced output. Varying σ1 and σ2 gives
 changes the sensitivity of the filter to edges at different scales. 
 Usually we want σ2 > σ1, and we require that σ1 != σ2.
Write a Slicer Execution Model compliant command line program that reads a 3D input 
image and filter size parameters σ1 and σ2 and produces an edge-enhanced DOG-filtered 3D output image. Rescale the output image so that min(DOG(f )) is mapped to graylevel 0 and max(DOG(f )) is mapped to graylevel 255 and the output image is stored as an unsigned char image.
For example, if the program is invoked this way:

```bash
DOG_{hawkid} --inputVolume inimg.nii.gz \
--signam1 2.5 --sigma2 3.5 \
--outputVolume outimg.nii.gz
``` 

then your program would read the file `inimg.nii.gz`, apply a 3D DOG filter with `σ1 = 2.5` units and `σ2 = 3.5`
 units, and then write the output file outimg.nii.gz.
Use  the  test  images  in  the  `${DATA}/data` and  `${DATA}/noisy directories`.  Experiment  with various 
values of σ1 and σ2 and describe in a few paragraphs how the edge
 response changes in relation to these two values in the file hw04-{githubid}/src/DOG/README.md.

## GradientAD

Store the source code for this part of the assignment in the hw04-{githubid}/src/GradientAD
subdirectory.

Write a Slicer Execution Model compliant command line program that reads a 3D input  image, a filter 
conductance parameter, a time step, and a number of iterations, and produces  a 3D output image filtered by gradient anisotropic diffusion. For example, the program might be invoked this way:

```bash
GradientAD_{hawkid} --inputVolume inimg.nii.gz \
--conductance 2.0 --timeStep 0.1 \
--iterations 5 --outputVolume outimg.nii.gz
```

Experiment with various parameter values and document the parameters you choose that
achieve good smoothing without losing too much edge detail in the file hw04-{githubid}/src/GradientAD/README.md

## CurvatureAD
Store the source code for this part of the assignment in the hw04-{githubid}/src/CurvatureAD subdirectory.

Create a curvature-driven anisotropic diffusion filter. For example, the program might be invoked this way:

```bash
CurvatureAD_{hawkid} --inputVolume inimg.nii.gz --conductance 2.0 \
--timeStep 0.1 --iterations 5 \
--outputVolume outimg.nii.gz
```

Experiment with various parameter values and document the parameters you choose that
achieve good smoothing without losing too much edge detail in the file hw04-{githubid}/src/CurvatureAD/README.md

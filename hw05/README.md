# Image Processing Using ITK

For this homework we will be writing a program that will put stripes into an image.

  In this problem you must write a program to put stripes into a 2 dimensional or a 3 dimensional image.
  Store the source code for this part of the assignment in the:

  The filter should be able to set the desired stripe value, 
  the direction of the stripes (with respect to the voxel lattice),
  the value of the voxels to be changed,
  the width of the strip,
  and the distance between start of each stripe.
  The start of the first stripe is at the edge of the image.

  The program parameters should be entered on the command line.

```bash
StripeImage --dimension input_dim  \
    --inputImage intput_fn  \
    --outputImage output_fn  \
    --stripeDirection stripe_dir \
    --stripeValue stripe_val \
    --stripeWidth stripe_width \
    --stripeDistance stripe_distance
```

   For example, if the program is invoked this way from the command line:

```bash
StripeImage --dimension 3  \
    --inputImage inputImg.nii.gz  \
    --outputImage outputImg.nii.gz  \
    --stripeDirection 0 \
    --stripeValue 50 \
    --stripeWidth 3 \
    --stripeDistance 7
```

then a 3 dimensional input image will be read in, and a 3 dimensional output image will be created with
stripes in the first voxel lattice direction that are 3 voxels wide of value 50 will be 7 voxels apart
from the start of one stripe to the start of the next stripe.

You can assume that the end user of this program always tells the 
truth about the --inputImageDimension.  That is to say that you do not need
to verify that the image really is the the size that is specified on the command line.

Your program must do sanity checking for invalid parameter combinations. For example,
give a meaningful error message
if the image dimension is 5 and the 7th dimension is requested for striping, then
an error should provided.  Your programs `main` function must `return EXIT_FAILURE` if an
invalid parameter combination is encountered.

You program needs to work for images with 1 to 6 dimensions.  You program must work with unsigned integer
valued images where the image values will always be in the range of [ 0, 10,000 ].  You should choose a pixel value type
that is most appropriate for this intensity range.

Your program should have good command line documentation in response to the `StripeImage -h` flag.


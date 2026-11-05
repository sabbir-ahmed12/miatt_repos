# Difference of Gaussian Filter
This program implements a 3D Difference of Gaussians (DoG) filter using ITK and the Slicer Execution Model. The DoG output is computed as:

## Behavior of sigma1 and sigma2
- The parameter sigma1 controls the amount of smoothing in the first Gaussian filter, while sigma2 controls the smoothing in the second Gaussian filter. Usually sigma2 is chosen to be larger than sigma1. The first filtered image retains more local structure, while the second filtered image is smoother and contains more coarse background information.
- When sigma1 and sigma2 are close to each other, the subtraction result is weaker because the two blurred images are similar. This gives a more subtle edge response. When the difference between sigma1 and sigma2 becomes larger, the DoG result emphasizes larger-scale transitions more strongly, often producing a stronger edge-enhanced appearance.
- Smaller sigma values make the filter sensitive to finer details and small edges, but they can also make the output more sensitive to noise. Larger sigma values suppress noise better and respond to broader structures, but very large values may oversmooth the image and reduce fine edge detail.
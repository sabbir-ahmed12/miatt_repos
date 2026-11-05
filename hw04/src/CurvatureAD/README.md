# Curvature-Driven Anisotropic Diffusion
- For the same set of parameters, the image looked blurrier than GradientAD method. Lower value of conductance preserved subtle edges but left some noise. However, higher values blur more.
- Same numerical instability error was thrown for higher value (0.1) for timeStep. However, such warning was not shown for lower value of the timeStep.
- Increasing the number of iterations made the image look like blocky. So, less number of iterations was chosen to get better smoothing.
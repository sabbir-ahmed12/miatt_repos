# Gradient Anisotropic Diffusion Image Filter
- **Conductance:** It controls the sensitivity to edges. When a lower value is chosen, it preserves more edges but smooth less. For a higher value, it blurs across edges.
- **Time Step:** For a higher value of time step, numeric instability warning was shown. So, a smaller value was chosen to ensure numerical stability.
- **Iterations:** It controls how many times the diffusion is applied. More number of iterations resulted in more smoothing of the image.
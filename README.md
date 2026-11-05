# Multi-Dimensional Image Analysis Tools and Techniques

This course provides a hands-on experience of medical imaging tools and techniques as they apply to problems in commercial, clinical, and research settings. It is built on image processing, machine learning, and software engineering topics. It seeks to provide broad exposure to standard tools and provide experience in applying them to contemporary problem settings. Besides, it includes building high-performance multi-threaded algorithm development, rapid prototyping and analysis pipeline building using ITK, MONAI and PyTorch for reproducible research.

## Contents

This repository contains all the code developed as part of the course mentioned above.

- **HW01:** Shows how to build ITK from source and linking a C++ application to ITK using CMake.
- **HW02:** Introduces built-in ITK data structures and possible ways to use them.
- **HW03:** Implemented image conversion application using ITK and applying Gaussian smoothing on the image.
- **HW04:** Implemented different filters such as - SimpleThreshold, Gaussian Low Pass Filter, Curvature AD etc using ITK and build extensions for Slicer3D application.
- **HW05:** Implemented a image manipulation pipeline using ITK and Slicer3D.
- **HW06:** Introduces the idea of physical space and index space and the transformation from one to another using ITK.
- **Lab01:** Introduces tools for doing reproducible research such as `pixi`, writing clean code using `ruff`, `black`, `pre-commit`.
- **Lab02:** Implemented a solution to register two circles using C++ and ITK following best engineering practices to make the solution trustworthy and appropriate.
- **Lab03:** Same as `Lab02` but with the help of a coding agent. The objective is to learn directing a coding agent towards a trustworthy solution while inspecting critical details in the implementation.
- **Lab04:** Same as `Lab02` and `Lab03` but by using a multi-resolution approach.
- **Midterm-Exam:** Implemented edge detection filters such as - `Sobel`, `LaplacianRecursiveGaussian` and `GradientMagnitudeRecursiveGaussian` using ITK and implemented extensions for Slicer3D image viewer. It also included an exercise to accurately calculate the distance between the pupils of two eyes in physical space using Slicer3D and ITK.
- **Final-Exam:** It provided 3D head MRI datasets from six different sites simulating a real-world scenario of a multi-center project. Each site had its own quirks to mimic the heterogeneity of a real multi-site neuroimaging study. The goal was to find landmarks from every image and ACPC align all the images irrespective of the sites they came from and compare the results.


***This was an intensive hands-on course. It helped me learn a lot and the questions being difficult, the faculty allowed the students to use AI in some assignments. I learned about writing `Product Requirement Document (PRD)`, working with coding agents to direct it towards building a trustworthy product, learned about building image processing pipeline and doing reproducible research***
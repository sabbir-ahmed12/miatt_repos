# Free Response

Answer the following questions by modifying this file directly.

##### 1 . Does cmake compile your program?

 - [x] Yes
 - [ ] No

##### 2 . Explain your answer to question #1 by describing the primary utility of CMake (i.e. why does the cmake exist, what common problem is the cmake tool well suited to solve)?

 > CMake is a build system generator used to simplify compiling complex C/C++ projects. It solves the problem of managing multiple source files, dependencies, and platform-specific build steps by generating appropriate build files (like Makefiles) from a single configuration (CMakeLists.txt). So, it allows developers to write build instructions once and use them across different systems and compilers.

##### 3 . How should the `typedef` or `using` C++ directives be used in your ITK based homework assignments?

 > `typedef` or `using` should be used to create clear, readable aliases for complex template types (such as images, readers, filters, and iterators). Since ITK uses heavily templated classes, these aliases simplify code, reduce repetition, and improve readability.

##### 4 . For an ITK image, what are the 3 state variables that are required to define the physical representation of an image? (NOTE: Regions, Size, Dimension, and Pixel types are incorrect answers.)

 > Origin, Spacing and Direction
 
##### 5 . For an `itk::Euler3DTransform<float>` what do the first 3 values of the FixedParameters represent in the transform model?

 > FixedParameter[0] is the x-coordinate of the center of rotation

 > FixedParameter[1] is the y-coordinate of the center of rotation

 > FixedParameter[2] is the z-coordinate of the center of rotation


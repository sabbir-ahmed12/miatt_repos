# Standalone ITK Application Linking to a Pre-Built ITK

## Objective

Build a standalone C++ application (outside the ITK source tree) that links against your pre-built installation of ITK and generates a synthetic 2D image.

This assignment focuses on:

- Consumer-side CMake configuration
- Proper use of `find_package(ITK REQUIRED)`
- Linking against installed ITK
- Basic ITK image construction and I/O


---

## Functional Requirements

Your program binary must:

1. Create a 2D `itk::Image`
2. Use:
   - **Size:** 8 × 8 pixels
   - **Spacing:** 4.0 mm × 4.0 mm (isotropic)
   - **Origin:** (0.0, 0.0)
   - **Direction:** Identity (default is acceptable)
3. Allocate and fill all pixel values deterministically with the value 5940
4. Print the following to stdout:
   - Size
   - Spacing
   - Origin
   - the value of the image at index location [4,4]

---

## Build System Requirements

Your application must:

- Use `CMake`
- Use `find_package(ITK REQUIRED)`
- Build out-of-source
- Link correctly against ITK

## Hints (Not all are correct or needed)

```bash
cmake -S . -B build -DITK_DIR=/path/to/ITKConfigDirectory
cmake --build build --config Release
./build/ece5940_hw0
```

You must have the following files/directory names
```txt
your-repo/
  README.md
  CMakeLists.txt
  src/
    CMakeLists.txt
    exe5940_hw01.cxx

```

## Essay Questions -- Answer inline in this README.md file below each question.

1. What is your Name?
- Sabbir Ahmed

2. What is your hawkid?
- sahmed8

3. What is your github username?
- sabbir-ahmed12

4. Describe the design decisions you made in solving this problem?
- First, to reduce the memory footprint, unsigned short was used as the data type of the pixels (PixelType). `unsigned short` takes 2 Bytes of memory and so it made it possible to set the pixel value as `5940`. Second, the size of the image, orientation and spacing were set as mentioned in the question. While setting the pixel value as `5940`, instead of using iterator-based filling, `FillBuffer()` method was used which is fast. Third, two separate CMakeLists.txt files were used so that it is easier to scale the project when it grows. 

5. Describe how AI code generation tools were used in solving this problem.
- I used AI code generatioin tool (ChatGPT) to write and refactor the CMakeLists.txt files. The CMakeLists.txt file inside the `src` directory worked as explained in Chapter 2 of the first book. However, I could not figure out the way to write the CMakeLists.txt file in the root directory. Moreover, I was getting a deprication warning while building the project which I resolved using the AI tool.


## Building the Application
```
git clone <url>
cd <hw01 repo>
cmake -S . -B build -DITK_DIR="path/to/ITK-build/"
cd build
make
```
- To run the application, go to the `src` directory inside `build` directory and run the program as given below:
```
cd src/
./ece5940_hw01
```

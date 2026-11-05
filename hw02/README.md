# Build C++ Application Using ITK and CMake

## OBJECTIVES
  - Understand how to define high-level build directives in the cmake language
  - Write simple C++ programs intended for command line execution
  - Understand how to process command line string arguments for use as other C++ data types.
  - Configure cmake for pre-processor include file path configuration to ITK header files
  - Configure cmake for library linkage with ITK pre-built binaries
  - Submit the homework assignment to GitHub
  - Use and understand CLion

## Problem Requirements

### part1
Download and build ITK version v6.0b02

```bash
# HINTS:  The following commands are HINTS, and may not be properly
#         formatted, or correct.  Understand the commands before using them.
git clone https://github.com/InsightSoftwareConsortium/ITK.git
cd ITK
git checkout v6.0b02
mkdir build
cmake -G Ninja -S $(pwd) -B $(pwd)/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$(pwd)/install
cmake --build $(pwd)/build
```

Write a program that:
 - [ ] Takes in values representing real numbers on the command line
 - [ ] Stores each command line value as an IEEE 754 32-bit floating point number into variables named`multiplier1` and `multiplier2`
 - [ ] Creates an IEEE 754 32 bit floating point number with variable named `result` for the value resulting from multiplying the two command line values together.
 - [ ] Prints the computation and the result to the screen with up to 10 significant digits of precision.

```bash
# Example desired output
% ${my_git_repo}/hw/hw02/cmake-build-debug/part1/hw02part1 6.13 10
6.130000114 * 10 = 61.30000305
```

### part2
Write a program that:
  - [ ] Takes in a value representing an unsigned long integer on the command line
  - [ ] Stores the command line number as an unsigned long integer called `num_repeats`
  - [ ] Prints the ITK Source Version using `itk::Version::GetITKSourceVersion()` num repeats times to the screen.

```bash
# Example desired output
% miatt2023_workspace-hjmjohnson/hw/hw02/cmake-build-debug/part2/hw02part2 5
0: ITK Source Version: itk version 6.0.0
1: ITK Source Version: itk version 6.0.0
2: ITK Source Version: itk version 6.0.0
3: ITK Source Version: itk version 6.0.0
4: ITK Source Version: itk version 6.0.0
```

### Additional Tasks

- [x] `git commit` your assignment (perhaps many times) with meaningful commit messages describing the work you have completed
- [x] Ensure that your completed assignment is represented on the `main` branch on github
- [X] Celebrate by marking each item completed in this homework with an [X]
- [x] Answer all the essay questions inline
-
```txt
# Essay Question #1 : Describe below why the "hw02part1 6.13 10" results in a value of "61.30000305" instead of "61.300000000"

# We know that computers store numbers in binary, not decimal. Some decimal numbers cannot be written exactly in binary, no matter how many bits are allowed. $6.13 = 613 / 100$ and the denominator $100 = 2^2 \times 5^2$. However, binary numbers can only exactly represent fractions whose denominators are powers of $2$ such as: $1/2, 1/4, 1/8, ...$. Since $6.13$ contains a factor of $5$, its binary repeats forever. On the other hand, IEEE-754 float 32 has $1$ sign bit, $8$ exponent bits, $23$ fraction bits. That means, only $23$ binary digits of precision. So, the computer stores the first $23$ bits after converting the floating part into binary. That cut causes tiny error in the multiplication. Moreover, after multiplying with $10$, rounding happens again to fit into `float32`. Thus, the result appears $61.30000305$ instead of $61.300000000$.

```


```txt
# Essay Question #2 : What happens in hw02_part2.cxx when you change the value of num_repeats to -3?
# It starts printing the text for infinite times. This is because, when the variable num_repeats was defined as unsigned long, it wraps-around when we pass a negative value. So, if we pass $-3$, the computer computes it as $-3$ mod $2^64$ = $2^64 - 5$ which is large value. Therefore, the computer starts printing the text for infinite number of times.

```

```txt
# Essay Question #3 : What storage type did you choose for the loop index in hw02_part2.cxx?  Why did you choose that type?
# I chose `unsigned long` as the type of loop index variable as the value can get as high as `num_repeats` variable.

```


```txt
# Essay Question #4 : Describe the AI, or other automated tools used to complete this assignement.
# I used AI to understand the concept of nullptr and the use of nullptr in reading stdin in part2 of the assignment. 

```

```txt
# Essay Question #5 : Describe the human resources (other than yourself) used to complete this assignement.
# I did not take help from anyone other than the resource links provided and AI as mentioned above.

```

# References
- [ ] https://en.wikipedia.org/wiki/IEEE_754
- [ ] https://itk.org/Doxygen/html/itkVersion_8h_source.html
- [ ] http://www.cplusplus.com/reference/cstdlib/strtoul/
- [ ] Github intro:  https://www.linkedin.com/learning-login/share?forceAccount=false&redirect=https%3A%2F%2Fwww.linkedin.com%2Flearning%2Fgithub-for-data-scientists%3Ftrk%3Dshare_ent_url%26shareId%3DMqQxnGnsTEaZ0%252FwQg7Znjg%253D%253D&account=42459020

#include <iostream>
#include <iomanip> // Ref: http://www.cplusplus.com/reference/iomanip/setprecision/
#include "math_stuff.h"
#include <cstdlib> // Needed for EXIT_SUCCESS and EXIT_FAILURE definitions
#include <string>
#include <stdexcept>
#include <limits>

//HINT: You may need another header file to resolve missing definitions hardcoded below
//HINT: https://cplusplus.com/reference/string/stof/

// -- DO NOT CHANGE THE print_usage_error_messages function
static void print_usage_error_messages() {
  std::cout << "ERROR: must provide command line arguments for the \n"
            << "two floating point values that need to be multiplied."
            << std::endl;
  std::cout << "% ./hw02part1 6.13 10" << std::endl;
}

int main(int argc, char *argv[]) {
  // If less than 3 arguments are provided on the command line, print the error message provided
  // print_usage_error_message
  if (argc < 3) { print_usage_error_messages();  return EXIT_FAILURE; }


  // HINT: use const correctness to for producing the result.
  // Store each of the input as IEEE 754 32-bit float
  const IEEE754_type multiplier1 = static_cast<IEEE754_type>(std::stof(argv[1]));
  const IEEE754_type multiplier2 = static_cast<IEEE754_type>(std::stof(argv[2]));

  // Compute result and store it
  const IEEE754_type result = multiply(multiplier1, multiplier2);

  // -- DO NOT CHANGE ANY CODE BELOW THIS LINE
  std::cout << std::setprecision(10) << multiplier1 << " * "
            << std::setprecision(10) << multiplier2 << " = "
            << std::setprecision(10) << result << std::endl;
  return EXIT_SUCCESS;
}
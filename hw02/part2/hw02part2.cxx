#include <iostream>
#include <itkVersion.h>
#include <cstdlib> // Needed for EXIT_SUCCESS and EXIT_FAILURE definitions
#include <cerrno>
//HINT: You may need another header file to resolve missing definitions hardcoded below

// -- DO NOT CHANGE THE print_usage_error_messages function
static void print_usage_error_messages() {
  std::cout << "ERROR: must provide a command line argument for the number of "
               "times to print the ITK source version."
            << std::endl;
  std::cout << "% ./hw02part2 7" << std::endl;
}

int main(int argc, char *argv[]) {
  // If less than 2 arguments are provided on the command line, print the error message provided
  // print_usage_error_message
  if (argc < 2) { print_usage_error_messages(); return EXIT_FAILURE; }

  // HINT look up std::strtoul
  // HINT use nullptr instead of NULL for C++17

  // TODO: Add your code below with the main body of the work to be done here
  errno = 0;
  char* endptr = nullptr;
  unsigned long num_repeats = std::strtoul(argv[1], &endptr, 10);

  // Validate parsing of long integer
  // - endptr must move (so at least one digit parsed)
  // - must consume the whole string (*endptr == '\0')
  // - errno must not indicate overflow/underflow
  if(endptr == argv[1] || *endptr != '\0' || errno == ERANGE) {
      print_usage_error_messages();
      return EXIT_FAILURE;
  }

  for (unsigned long i = 0; i < num_repeats; i++) {
      std::cout << i << ": ITK Source Version: "
                << itk::Version::GetITKSourceVersion()
                << std::endl;
  }

  return EXIT_SUCCESS;
}
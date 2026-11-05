#ifndef HW02_MATH_STUFF_H
#define HW02_MATH_STUFF_H

// IEEE-754 32-bit floating point in C++ is `float`
using IEEE754_type = float;

/**
 *  Multiply two IEEE-754 32-bit floating point numbers.
 * @param num1 First floating-point value.
 * @param num2 Second floating-point value.
 * @return Product of num1 and num2.
 */
IEEE754_type  multiply(IEEE754_type num1, IEEE754_type num2);

#endif //HW02_MATH_STUFF_H

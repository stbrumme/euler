// ////////////////////////////////////////////////////////
// # Title
// Smallest multiple
//
// # URL
// https://projecteuler.net/problem=5
// http://euler.stephan-brumme.com/5/
//
// # Problem
// 2520 is the smallest number that can be divided by each of the numbers from 1 to 10 without any remainder.
// What is the smallest positive number that is evenly divisible by all of the numbers from 1 to 20?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// Basically we have to find the Least Common Multiple of 1,2,..,20 (abbreviated as `lcm`, see https://en.wikipedia.org/wiki/Least_common_multiple ).
// In general, the `lcm` of two numbers `a` and `b` can be computed as: `lcm(a,b)=frac{ab}{gcd(a,b)}`
//
// `gcd` stands for the Greatest Common Divisor (see https://en.wikipedia.org/wiki/Greatest_common_divisor ).
//
// Euclid's algorithm ( https://en.wikipedia.org/wiki/Euclidean_algorithm ) produces the `gcd` in a recursive way:
// `gcd(a,0) = 0`
// `gcd(a,b) = gcd(b mod a, a)`
//
// An iterative version in C++ consists of just a few lines, see my ''gcd'' function.
//
// Now that we know how to determine `lcm(a,b)` there is a pretty easy way to do the same for `lcm(x_1,x_2,x_3,...,x_n)`:
// `lcm(x_1,x_2,x_3,...,x_{n-1},x_n) = lcm(lcm(x_1,x_2,x_3,...,x_{n-1}), x_n)`
//
// Example:
// `lcm(1,2,3,4)`
// `=lcm(lcm(1,2,3),4)`
// `=lcm(lcm(lcm(1,2),3),4)`
// `=lcm(lcm(2,3),4)`
// `=lcm(6,4)`
// `=12`
//
// # Note
// Wikipedia lists some interesting alternatives with different runtime behavior.
// Especially the binary algorithm can be much faster (at the cost of more code).
//
// By the way: the G++ compiler offers an intrinsic called ''__gcd()'' which may be faster on some systems.
// I highly suspect it is based on the binary algorithm.

#include <iostream>

// greatest common divisor
unsigned long long gcd(unsigned long long a, unsigned long long b)
{
  while (a != 0)
  {
    unsigned long long c = a;
    a = b % a;
    b = c;
  }
  return b;
}

// least common multiple
unsigned long long lcm(unsigned long long a, unsigned long long b)
{
  // parenthesis avoid overflow
  return a * (b / gcd(a, b));
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int x;
    std::cin >> x;

    // find least common multiple of all numbers
    unsigned long long result = 1;
    for (unsigned int i = 2; i <= x; i++)
      result = lcm(result, i);

    std::cout << result << std::endl;
  }
  return 0;
}

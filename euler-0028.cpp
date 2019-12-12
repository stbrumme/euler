// ////////////////////////////////////////////////////////
// # Title
// Number spiral diagonals
//
// # URL
// https://projecteuler.net/problem=28
// http://euler.stephan-brumme.com/28/
//
// # Problem
// Starting with the number 1 and moving to the right in a clockwise direction a 5 by 5 spiral is formed as follows:
//
// ''__21__ 22 23 24 __25__''
// ''20  __7__  8  __9__ 10''
// ''19  6  __1__  2 11''
// ''18  __5__  4  __3__ 12''
// ''__17__ 16 15 14 __13__''
//
// It can be verified that the sum of the numbers on the diagonals is 101.
//
// What is the sum of the numbers on the diagonals in a 1001 by 1001 spiral formed in the same way?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My code contains two approaches: a basic brute-force version which easily solves the original problem
// and a vastly different solution if the spiral is way bigger than 1001 (actually it can easily find the result for `10^18` spiral).
//
// Let's start with the brute force solution:
// The spiral of the problem statement has size 5x5. We can split it into three circles/rings:
// 1. the inner ring  (`width=1`) contains just ''1''
// 2. the middle ring (`width=3`) contains all numbers from ''2'' to ''9''
// 3. the outer ring  (`width=5`) contains all numbers from ''1'' to ''25''
//
// The "corners" of each ring are:
// 1. ''1'', sum is 1
// 2. ''3'', ''5'', ''7'' and ''9'', their sum is 24
// 3. ''13'', ''17'', ''21'' and ''25'', their sum is 76
//
// The "largest" corner of each ring can be found in its upper-right corner (let's call it `ur_{width}`) and has an interesting pattern:
// `ur_1=1=1^2`, `ur_3=9=3^2`, `ur_5=25=5^2`, and so on:
// `ur_i = i^2`
//
// All other corners can be derived from the upper-right corner (upper-left is `ul_i`, lower-left is `ll_i`, lower-right is `lr_i`):
// `ul_i = ur_i - (i - 1) = i^2 - (i - 1)`
// `ll_i = ul_i - (i - 1) = i^2 - 2 * (i - 1)`
// `lr_i = ll_i - (i - 1) = i^2 - 3 * (i - 1)`
//
// The sum of all corners of a ring is:
// `corners_i = ur_i + ul_i + ll_i + lr_i`
// `= i^2 + i^2 - (i - 1) + i^2 - 2 * (i - 1) + i^2 - 3 * (i - 1)`
// `= 4 * i^2 - 6i + 6`
//
// A ''for''-loop running from 1, 3, 5, ... to 1001 (the maximum size of the spiral) computes the sum of all rings.
// It shows the correct result pretty much instantly.
//
// ... but ...
//
// Larger spirals (Hackerrank test cases ask for a width of up to `10^{18}-1`) increase the computation time considerably.
// Fortunately, there is a closed form to find the solution without any loops:
//
// We saw that `ur_i=i^2` where `i` is the width of the spiral (1, 3, 5, ...).
// If we replace the width by the number `x` of the ring minus 1 (`x=0` stands for the 1st ring, `x=1` means 2nd ring, `x=2` ==> 3rd ring, ...), then
// `i = 2x + 1` and `x=floor{i/2}`
//
// Hence the upper-right corner of each ring is:
// `ur_x = 4x^2 + 4x + 1`  (produces the series `ur_0=1`, `ur_1=9`, `ur_2=25`, ...)
//
// The three other corners of each ring:
// `ul_x = ur_x - (i - 1) = ur_x - (2x + 1 - 1) = ur_x - 2x = 4x^2 + 4x + 1 - 2x`
// `ll_x = ul_x - (i - 1) = 4x^2 + 4x + 1 - 4x = 4x^2 + 1`
// `lr_x = ll_x - (i - 1) = 4x^2 + 4x + 1 - 6x = 4x^2 - 2x + 1`
//
// And their sum becomes:
// `corners_x = ur_x + ul_x + ll_x + lr_x = 16x^2 + 4x + 4`
//
// A spiral consists of `\lfloor frac{width}{2} \rfloor` rings, therefore the sums of the diagonals is the sum of all corners:
// `result = sum_{x=0..width/2}{16x^2 + 4x + 4}`
// `=16 * sum_{x=0..width/2}{x^2} + 4 * sum_{x=0..width/2}{x} + sum_{x=0..width/2}{4}`
// `=16 * sum{x^2} + 4 * sum{x} + sum{4}`
// (I removed the ranges from of the sums signs to make the formula shorter)
//
// The series `x^2` is called __Square pyramidal numbers__ (see https://en.wikipedia.org/wiki/Square_pyramidal_number ) and their sum is:
//
// `sum{x^2} = frac{x * (x + 1) * (2x + 1)}{6}`
//
// The series `x` is called __Triangle numbers__ (see https://en.wikipedia.org/wiki/Triangular_number ) and their sum is:
//
// `sum{x} = frac{x * (x + 1)}{2}`
//
// We can replace:
// `16 * sum{x^2} + 4 * sum{x} + sum{4}`
// `=16x(x + 1)(2x + 1) / 6 + 4(x(x + 1) / 2) + 4x`
// `=8x(x + 1)(2x + 1) / 3 + 2x(x + 1) + 4x`
//
// The only problem is that the sum formulas for Triangle numbers and Square pyramidal numbers are defined beginning at 1
// whereas my original formula began at 0: our inner-most ring is missing. It is always 1 ==> let's add 1:
// `8x(x + 1)(2x + 1) / 3 + 2x(x + 1) + 4x + 1`
//
// # Hackerrank
// Even though Hackerrank defines the maximum input to be `10^{18}`, the output should be printed modulo 1000000007.
// Languages that support big integer computations can easily evaluate this formula and are done.
// But life is tough for C++ programmers ...
//
// My C++ code needs a few more mathematical tricks to solve this problem. The rules for modular arithmetic are (see https://en.wikipedia.org/wiki/Modular_arithmetic ):
// `(a + b) mod c = ((a mod c) + (b mod c)) mod c`
// `(a * b) mod c = ((a mod c) * (b mod c)) mod c`
//
// In general, `frac{a}{a} = a * a^{-1} = 1` which means that `a^{-1}` is the inverse of `a`
//
// `a^{-1}` in modular arithmetic is defined as (see https://en.wikipedia.org/wiki/Modular_multiplicative_inverse ):
// if `a mod b == 0` then `frac{a}{b} mod c = ((a mod c) * inverse(b, c)) mod c`
//
// 1000000007 is a prime number, therefore the inverse can be computed based on Fermat's little theorem ( https://en.wikipedia.org/wiki/Fermat%27s_little_theorem ):
// `a^{p-1} mod p = 1` (if `a < p`, in our case `a=3` and `p=10^9+7`)
// `(a * a^{p-2}) mod p = 1` ==> `a^{p-2}` is the multicative inverse of `a` in `p`
//
// I could precompute ''inverseModulo(3, 1000000007)'' (e.g. with this online tool: https://planetcalc.com/3311/ )
// but I decided to include the full source code to figure out the modular multiplicative inverse at runtime (''inverseModulo'' and ''powerModulo'').
//
// My program splits the original formula into smaller parts:
// `8x(x + 1)(2x + 1) / 3 + 2x(x + 1) + 4x + 1`
// ''sharedTerm = (2*x * (x + 1)) % Modulo''
// ''sum1 = ((4 * sharedTerm * (2*x + 1) ) % Modulo) * inverseModulo(3, Modulo)''
// ''sum2 = sharedTerm + 4*x + 1''
// ''sum = (sum1 % Modulo + sum2 % Modulo) % Modulo''
//
// __Note:__ the first half of ''sum1'' must be a multiple of 3 (remember the condition `a mod b == 0` when deriving the multiplicative inverse).
// We know that the result as well as ''sum2'' are integers. Therefore ''sum1'' be an integer, too, and `8x(x + 1)(2x + 1)` is always a multiple of 3.
//
// I know, it's very awkward but gets the job done ... no other of my solutions for a Project Euler problem has so many words for so little code !

#include <iostream>

// return (base^exponent) % modulo
unsigned int powmod(unsigned long long base, unsigned int exponent, unsigned int modulo)
{
  unsigned long long result = 1;
  while (exponent > 0)
  {
    // https://en.wikipedia.org/wiki/Exponentiation_by_squaring
    // odd exponent ?
    if (exponent % 2 == 1)
    {
      result = (result * base) % modulo;
      exponent--;
    }
    else
    {
      base = (base * base) % modulo;
      exponent /= 2;
    }
  }
  return result;
}

// return modulo multiplicative inverse of a such that (a*inverse(a)) % p = 1
unsigned int inverseModulo(unsigned int a, unsigned int modulo)
{
  // if p is prime, then https://en.wikipedia.org/wiki/Fermat%27s_little_theorem applies:
  //      a^(p-1)  % p = 1
  // (a * a^(p-2)) % p = 1
  // that means a^(p-2) is the result
  return powmod(a, modulo - 2, modulo);
  // inspired by https://barkingbogart.wordpress.com/2013/02/21/cnk-mod-1000000007/
}

int main()
{
  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long n;
    std::cin >> n;

    // sum along the diagonals, initially for width = 1
    unsigned long long sum = 1;

    // brute-force approach (good enough for original problem):
    //for (int64_t width = 3; width <= n; width += 2)
    //{
      // all four corners
      //sum     += width * width;                   // upper-right
      //sum     += width * width -     (width - 1); // upper-left
      //sum     += width * width - 2 * (width - 1); // lower-left
      //sum     += width * width - 3 * (width - 1); // lower-right

      // same as:
    //  sum += 4*width*width - 6*width + 6;
    //}

    // direct computation:
    // half side length
    unsigned long long x = n / 2;
    // the closed form is:
    //sum  = 8 * x * (x + 1) * (2*x + 1) / 3   +   2 * x * (x + 1) + 4 * x + 1;

    // apply Modulo whenever an overflow is possible
    const unsigned int Modulo = 1000000007;

    x %= Modulo;

    // first part: 8 * x * (x + 1) * (2*x + 1) / 3
    //           = 4 *  sharedTerm * (2*x + 1) / 3
    unsigned long long sharedTerm = (2*x * (x + 1)) % Modulo;

    // the division by 3 becomes a multiplication by its modulo multiplicative inverse
    unsigned long long sum1 = ((4 * sharedTerm * (2*x + 1)) % Modulo) * inverseModulo(3, Modulo);

    // second part: 2 * x * (x + 1)      + 4 * x + 1
    //            =     secondTerm       + 4 * x + 1
    unsigned long long sum2 = sharedTerm + 4*x + 1;

    // sum = first part + second part
    sum = (sum1 % Modulo + sum2 % Modulo) % Modulo;

    std::cout << sum << std::endl;
  }
  return 0;
}

// ////////////////////////////////////////////////////////
// # Title
// Square remainders
//
// # URL
// https://projecteuler.net/problem=120
// http://euler.stephan-brumme.com/120/
//
// # Problem
// Let `r` be the remainder when `(a-1)^n + (a+1)^n` is divided by `a^2`.
//
// For example, if `a = 7` and `n = 3`, then `r = 42`: `6^3 + 8^3 = 728 == 42 mod 49`.
// And as `n` varies, so too will `r`, but for `a = 7` it turns out that `r_{max} = 42`.
//
// For `3 <= a <= 1000`, find ` sum{r_{max}}`.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// The binomial expansion (see https://en.wikipedia.org/wiki/Binomial_theorem ) is:
//
// `(x+y)^n = {{n}choose{0}} x^{n}y^0 + {{n}choose{1}} x^{n-1}y^1 + ... {{n}choose{n-1}} x^{1}y^{n-1} + {{n}choose{n}} x^{0}y^n`
//
// If `x=a` and `y = \pm 1`:
// `(a+1)^n = {{n}choose{0}} a^{n} + {{n}choose{1}} a^{n-1} + {{n}choose{2}} a^{n-2} + ... {{n}choose{n-1}} a^{1}            + {{n}choose{n}} a^{0}`
// `(a-1)^n = {{n}choose{0}} a^{n} - {{n}choose{1}} a^{n-1} + {{n}choose{2}} a^{n-2} - ... {{n}choose{n-1}} a^{1} (-1)^{n-1} + {{n}choose{n}} a^{0} (-1)^n`
//
// Most of the terms are multiples of `a^2`. The modulo "removes" them. And keep in mind that `a^0 = 1` and `{{n}choose{n}} = 1` and `{{n}choose{n-1}} = n`:
// `(a+1)^n mod a^2 = na            + 1`
// `(a-1)^n mod a^2 = na (-1)^{n-1} + (-1)^n`
//
// If `n` is __even__ then:
// `((a+1)^n + (a-1)^n) mod a^2 = (na + 1 + na * (-1) + 1) mod a^2 = 2 mod a^2`
// ==> For any even `n` the result is always 2.
//
// If `n` is __odd__ then:
// `((a+1)^n + (a-1)^n) mod a^2 = (na + 1 + na - 1) mod a^2 = 2na mod a^2 = 2n mod a`
//
// The maximum `n_{max}` such that `2n_{max}` is as close as possible to `a`:
// `2n_{max} = a - 1`
//
// `n_{max} = dfrac{a - 1}{2}`
//
// `n_{max}` must be an integer, therefore the division is actually an integer division:
// `n_{max} = \lfloor dfrac{a - 1}{2} \rfloor`
//
// And finally the remainder becomes:
// `r_{max} = 2an_{max} = 2a \lfloor dfrac{a - 1}{2} \rfloor`
//
// # Hackerrank
// I wrote very dirty code to solve a few test cases. Some test cases time out, some give just a wrong result.
// Not very proud of it !

#include <iostream>

#define ORIGINAL
#ifdef ORIGINAL
int main()
{
  unsigned int sum = 0;

  // iterate over 3..1000
  for (unsigned int a = 3; a <= 1000; a++)
  {
    unsigned int n_max = (a - 1) / 2;
    sum += 2*a*n_max;
  }

  // print result
  std::cout << sum << std::endl;

  return 0;
}

#else

#include <deque>

// return (a*b) % modulo
unsigned long long mulmod(unsigned long long a, unsigned long long b, unsigned long long modulo)
{
  // (a * b) % modulo = (a % modulo) * (b % modulo) % modulo
  a %= modulo;
  b %= modulo;

  // fast path
  if (a <= 0xFFFFFFF && b <= 0xFFFFFFF)
    return (a * b) % modulo;

  // we might encounter overflows (slow path)
  // the number of loops depends on b, therefore try to minimize b
  if (b > a)
    std::swap(a, b);

  // bitwise multiplication
  unsigned long long result = 0;
  while (a > 0 && b > 0)
  {
    // b is odd ? a*b = a + a*(b-1)
    if (b & 1)
    {
      result += a;
      result %= modulo;
      // skip b-- because the bit-shift at the end will remove the lowest bit anyway
    }

    // b is even ? a*b = (2*a)*(b/2)
    a <<= 1;
    a  %= modulo;

    // next bit
    b >>= 1;
  }

  return result;
}

// return (base^exponent) % modulo
unsigned long long powmod(unsigned long long base, unsigned long long exponent, unsigned long long modulo)
{
  unsigned long long result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = mulmod(result, base, modulo);

    // even exponent ? a^b = (a*a)^(b/2)
    base = mulmod(base, base, modulo);
    exponent >>= 1;
  }
  return result;
}

int main()
{
  const unsigned int minA   = 1;
  const unsigned int Modulo = 1000000007;

  std::deque<unsigned int> sums2(minA, 0);
  std::deque<unsigned int> sums3(minA, 0);

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int exponent = 2;
    unsigned long long maxA = 1000;
    std::cin >> maxA >> exponent;

    unsigned long long sum = 0;
    if (exponent == 2)
    {
      // closed formula
      {
        unsigned __int128 half = maxA / 2;
        sum  = half * (8*half*half - 3*half - 5) / 3;

        if (maxA % 2 == 1)
          sum += (maxA*maxA -   maxA) % Modulo;

        sum += 2; // maxA == 1
        sum %= Modulo;
        std::cout << sum << std::endl;
        continue;
      }

      sum = sums2.back();
      for (auto a = sums2.size(); a <= maxA; a++)
      {
        unsigned long long A = a; // make sure that a*a is computed in  64 bit to avoid overflows
        unsigned int even = (A*A - 2*A) % Modulo;
        unsigned int odd  = (A*A -   A) % Modulo;

        // direct evaluation
        if (a == 1) // the formulas below fail for a == 1 ...
          sum += 2;
        else if (a % 2 == 0)
          sum += even; // for even numbers
        else
          sum += odd;  // for odd  numbers

        sum %= Modulo;
        sums2.push_back(sum);
      }

      sum = sums2[maxA];
    }
    else // exponent = 3
    {
      sum = sums3.back();
      for (auto a = sums3.size(); a <= maxA; a++)
      {
        unsigned __int128 A = a; // make sure that a*a*a is computed in 128 bit to avoid overflows
        unsigned int even = (A*A*A - 2*A) % Modulo;
        unsigned int odd  = (A*A*A -   A) % Modulo;

        if (a == 1) // the formulas below fail for a == 1 ...
          sum += 0;
        else if (a % 2 == 0)
          sum += even; // for even numbers
        else
          sum += odd;  // for odd  numbers

        sum %= Modulo;
        sums3.push_back(sum);
      }

      sum = sums3[maxA];
    }

    // print result
    std::cout << sum << std::endl;
  }

  return 0;
}

#endif

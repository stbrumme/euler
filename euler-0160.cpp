// ////////////////////////////////////////////////////////
// # Title
// Factorial trailing digits
//
// # URL
// https://projecteuler.net/problem=160
// http://euler.stephan-brumme.com/160/
//
// # Problem
// For any N, let f(N) be the last five digits before the trailing zeroes in N!.
// For example,
//
// 9! = 362880 so f(9)=36288
// 10! = 3628800 so f(10)=36288
// 20! = 2432902008176640000 so f(20)=17664
//
// Find f(1,000,000,000,000)
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// Wow, this was probably the most trial'n'error based algorithm so far.
// I knew the correct answer (just entered "1000000000000!" in [WolframAlpha](http://www.wolframalpha.com/input/?i=1000000000000!) ) but my code was very slow.
//
// It was obvious to me that a zero is appended to a factorial whenever the current number is a multiple of 5.
// Actually, some of the previous numbers in the factorial needs to be a multiple of 2 because only 2*5=10 produces a zero,
// but there are far more numbers which are a multiple of 2 than a multiple of 5.
//
// The concept was to multiply all odd number (except those ending with 5) modulo 10000.
// Then multiply all even numbers, but divided by 2.
// Count the number of multiples of 2 (let's call them "twos") and count the multiple of 5 ("fives")
// and multiply the result by `2^{twos-fives}`.
//
// My first attempts needed a few hours to display the correct result but using smaller factorials (which I verified with WolframAlpha, too)
// I played around with the code which parts I can optimize in a true trial'n'error way:
// - then I grouped ten consecutive numbers and saved a few modulo operations ...
// - then I reduced all numbers that are neither a multiple of 2 nor 5 to their last five digits ...
// - then I discovered that actually only the last digit of those numbers matters and in each iteration I can multiply by 1*3*7*9 right away
// - then I saw that all multiples of 4 provides enough "twos" for the "fives", thus 1*2*3*6*7*8*9 is possible; only treat 4, 5, and 10 (=0) separately
// - then I included the ''powmod'' function from my [toolbox](/toolbox) and did ''powmod(1*2*3*6*7*8*9, 10^12/10)'' once at the beginning
// ... and then my code ''algorithm1()'' needed just 12 minutes to find the correct result. Still too slow.
//
// I knew that `f(10^12) != f(10^11)`. More or less by chance I tried `f(10^12 / 2)` and `f(10^12 / 5)`.
// It turns out that `f(10^12) = f(10^12 / 5)` (but `f(10^12) != f(10^12 / 2)`.
// Even more, `f(5N) = f(N)` for `5N > Modulo`. This can be applied repeatedly: `f(5^k * N) = f(N)` for `5N > Modulo`.
//
// My super-optimized code computes `f(2560000)` in less than 10 milliseconds. Yeah !
//
// If I had known that trick right from the start then I would have probably only written ''algorithm2()''.
// But I really enjoyed the ride of optimizing ''algorithm1()'' !
//
// # Note
// Most of my optimizations of ''algorithm1()'' focus on solving `f(N)` for large `N`.
// It produces wrong results for small `N`.
//
// # Alternative
// True mathematicians solved this problems with a weird series of modulo computations, Wilson's theorem (which I've never heard of) and many more.
// It seems to me that problem 160 is among those problems with the biggest variety of solution strategies.

#include <iostream>

// last five digits
const unsigned int Modulo = 100000;

// ---------- powmod was taken from my toolbox ----------

// return (base^exponent) % modulo
unsigned int powmod(unsigned int base, unsigned long long exponent, unsigned int modulo)
{
  unsigned int result = 1;
  while (exponent > 0)
  {
    // fast exponentation:
    // odd exponent ? a^b = a*a^(b-1)
    if (exponent & 1)
      result = ((unsigned long long)result * base) % modulo;

    // even exponent ? a^b = (a*a)^(b/2)
    base = ((unsigned long long)base * base) % modulo;
    exponent >>= 1;
  }
  return result;
}

// ---------- and now my first "brute force" algorithm ----------

unsigned int algorithm1(unsigned long long limit)
{
  // process ten numbers in one iteration
  unsigned long long iterations = limit / 10;

  // count "excess"/"spare" twos that can be used whenever I encounter a five (2*5=10)
  unsigned long long twos = 0;

  // find last five digits of (2*3*6*7*8*9)^iterations
  // note: the 4 is missing because sometimes I need a few "spare" twos
  unsigned long long result = powmod(1*2*3*6*7*8*9, iterations, Modulo);

  // process numbers 10i+1, 10i+2, 10i+3, ..., 10i+10
  for (unsigned long long i = 0; i < iterations; i++)
  {
    // enough "spare" twos ?
    if (twos > 100) // 100 was chosen "at random" but even 10 works fine
      // yes, no need to hold back a few twos (2*2=4)
      result *= 4;
    else
      // not enough "spare" twos, avoid multiplying by 4 and hold back 2 twos
      twos   += 2;

    // five = (i*10 + 5) / 5 = i*2 + 1
    auto five = i*2 + 1;
    // need one "spare" two
    twos--;
    // as long as it's still a multiple of 5 ...
    while (five % 5 == 0)
    {
      five /= 5;
      twos--; // and need one more "spare" two
    }
    result *= five % Modulo;

    // ten = (i*10 + 10) = i + 1
    auto ten = i + 1;
    // note: unlike five, now no "spare" two is needed

    // same loop as used for five ...
    while (ten % 5 == 0)
    {
      ten /= 5;
      twos--; //
    }
    result *= ten % Modulo;

    // keep only the last five digits
    result %= Modulo;
  }

  // multiply with remaining "spare" twos
  while (twos-- > 0)
    result = (result * 2) % Modulo;

  return result;
}

// ---------- a very basic algorithm, sufficient for f(2560000) ----------

unsigned int algorithm2(unsigned long long limit)
{
  unsigned long long result = 1;
  // multiply all numbers
  for (unsigned long long i = 1; i <= limit; i++)
  {
    auto current = i;
    // divisible by 5 ?
    while (current % 5 == 0)
    {
      current /= 5;
      // there were at least the same number of twos
      result  /= 2;
    }

    result *= current % Modulo;
    // note: I can't use % Modulo because whenever a 5 appear, I divide by 2, too
    //       and therefore can't erase too many digits higher than 10^5
    result %= 1000000000ULL;
  }

  // reduce to five digits
  return result % Modulo;
}

int main()
{
  // 10^12
  auto limit = 1000000000000ULL;
  std::cin >> limit;

  // f(5^k * n) = f(n) until n%10000 != 0
  while (limit % Modulo == 0)
    limit /= 5;

  if (limit > 2560000)
    // faster algorithm
    std::cout << algorithm1(limit) << std::endl;
  else
    // simpler algorithm
    std::cout << algorithm2(limit) << std::endl;

  return 0;
}

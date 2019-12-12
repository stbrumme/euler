// ////////////////////////////////////////////////////////
// # Title
// Resilience
//
// # URL
// https://projecteuler.net/problem=243
// http://euler.stephan-brumme.com/243/
//
// # Problem
// A positive fraction whose numerator is less than its denominator is called a proper fraction.
// For any denominator, d, there will be d-1 proper fractions; for example, with d=12:
// 1/12 , 2/12 , 3/12 , 4/12 , 5/12 , 6/12 , 7/12 , 8/12 , 9/12 , 10/12 , 11/12 .
//
// We shall call a fraction that cannot be cancelled down a resilient fraction.
// Furthermore we shall define the resilience of a denominator, R(d), to be the ratio of its proper fractions that are resilient;
// for example, R(12) = 4/11 .
// In fact, d = 12 is the smallest denominator having a resilience R(d) < 4/10 .
//
// Find the smallest denominator d, having a resilience R(d) < 15499/94744 .
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// If a fraction is __not__ a proper fraction then `gcd(numerator, denominator) > 1`, that means both can be divided by the same integer.
// The Euler totient function `phi(x)` returns how many numbers `0 < y < x` are relatively prime to `x`.
// `phi(12) = 4` because 12 is relatively prime to 1, 5, 7 and 11.
//
// And indeed, 1/12, 5/12, 7/12 and 11/12 are the only proper fraction where the denominator is 12.
// Therefore the program should find a denominator d such that
// `dfrac{phi(d)}{d-1} < dfrac{15499}{94744}`
//
// The code of my ''phi()'' function is shared with problem 214 (and was originally written for problem 70).
// I had to change it's parameter type from ''unsigned int'' to ''unsigned long long'' but everything else is identical to problem 214.
//
// Unfortunately it's much too slow to check every single number, beginning with 12 (hint: the correct solution is a nine-digit number).
// But I did exactly that "to get a feeling" - and obviously stopped after a few hundred numbers.
// It turns out that `phi(x)/(x-1)` is low when `x` is the product of the first `n` prime numbers: `x_n = 2 * 3 * 5 * 7 * ...`.
//
// My current implementation multiplies all prime numbers until `phi(x_n)/(x_n-1)` is smaller than 15499/94744 (`x_n` is my variable ''current'').
// Then it reverts the last step (now `phi(x_{n-1})/(x_{n-1}-1)` is bigger than 15499/94744). ''current'' is updated to hold `x_{n-1}`.
// And finally all multiples of `x_{n-1}` (==> ''current'') are compared against the ratio until it's below 15499/94744.
//
// I consider the last step to be a "lucky guess". Yes, it produces the correct result but my mathematical knowledge isn't sufficient to know why.

#include <iostream>
#include <vector>

std::vector<unsigned int> primes;

// return phi(x) if x/phi(x) <= minQuotient (else the result is undefined but > minQuotient)
unsigned long long phi(unsigned long long x)
{
  // totient function can be computed by finding all prime factors p
  // and subtracting them from x
  auto result  = x;
  auto reduced = x;
  for (auto p : primes)
  {
    // prime factors have to be p <= sqrt
    if (p*p > reduced)
      break;

    // not a prime factor ...
    if (reduced % p != 0)
      continue;

    // prime factors may occur multiple times, remove them all
    do
    {
      reduced /= p;
    } while (reduced % p == 0);

    // but subtract from result only once
    result -= result / p;
  }

  // we only checked prime factors <= sqrt(x)
  // there might exist one (!) prime factor > sqrt(x)
  // e.g. 3 is a prime factor of 6, and 3 > sqrt(6)
  if (reduced > 1)
    result -= result / reduced;

  return result;
}

// return true if a1/b1 < a2/b2
bool isLess(unsigned long long a1, unsigned long long b1, unsigned long long a2, unsigned long long b2)
{
  // a1/b1 < a2/b2 is the same as
  // a1*b2 < a2*b1 if all numbers are positive
  return a1*b2 < a2*b1;
}

int main()
{
  unsigned int numerator   = 15499;
  unsigned int denominator = 94744;

  unsigned long long current = 1;
  for (unsigned int i = 2; ; i++)
  {
    bool isPrime = true;

    // test against all prime numbers we have so far (in ascending order)
    for (auto p : primes)
    {
      // next prime is too large to be a divisor ?
      if (p*p > i)
        break;

      // divisible ? => not prime
      if (i % p == 0)
      {
        isPrime = false;
        break;
      }
    }

    if (!isPrime)
      continue;

    // yes, we have a prime number
    primes.push_back(i);

    // multiply prime numbers until the ratio becomes too small
    current *= i;
    if (isLess(phi(current), current - 1, numerator, denominator))
      break;
  }

  // undo last prime
  current /= primes.back();

  // simple iterative scan
  for (unsigned int i = 1; ; i++)
  {
    auto next = current * i;

    // below threshold ?
    if (isLess(phi(next), next - 1, numerator, denominator))
    {
      std::cout << next << std::endl;
      break;
    }
  }

  return 0;
}

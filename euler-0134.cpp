// ////////////////////////////////////////////////////////
// # Title
// Prime pair connection
//
// # URL
// https://projecteuler.net/problem=134
// http://euler.stephan-brumme.com/134/
//
// # Problem
// Consider the consecutive primes `p_1 = 19` and `p_2 = 23`.
// It can be verified that 1219 is the smallest number such that the last digits are formed by `p_1` whilst also being divisible by `p_2`.
//
// In fact, with the exception of `p_1 = 3` and `p_2 = 5`, for every pair of consecutive primes, `p_2 > p_1`, there exist values of `n` for which the last digits
// are formed by `p_1` and `n` is divisible by `p_2`. Let `S` be the smallest of these values of `n`.
//
// Find `sum{S}` for every pair of consecutive primes with `5 <= p_1 <= 1000000`.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// I solved this problem twice:
// 1. a ''bruteForce'' approach (finishes in about 140 seconds)
// 2. a smarter ''chineseRemainderTheorem'' solution (finishes in about 0.1 second)
//
// Both call ''tens(x)'' which returns the smallest number `10^k` that is bigger than x, e.g. ''tens(456) = 1000''.
//
// My brute-force algorithm consists of a simple loop starting at ''tens(smallPrime) + smallPrime'' (e.g. 100+19=119 when checking the pair 19,23)
// and increments until the division by ''largePrime'' has a zero remainder.
// Short and simple code but very, very slow ...
//
// I must admit that I heard the name https://en.wikipedia.org/wiki/Chinese_remainder_theorem years ago but had no idea what it is about.
// Wikipedia's explanations weren't exactly clear to me and it took me 3 hours to come up with correct code.
//
// Nevertheless, now I have a small class to compute the extended Euclidean algorithm (see ''ExtendedGcd'') and learnt
// a few things about Monsieur Bezout ( https://en.wikipedia.org/wiki/%C3%89tienne_B%C3%A9zout ), too ...
//
// In order to find `S` I have to solve
// `n == 0 mod p_2` and
// `n == p_1 mod t` where `t_1 = tens(p_1)`
//
// The extended Euclidean algorithm of `(p_2,t)` returns two values `x` and `y`.
// One solution `n` is `0 * y * t + p_1 * x * p_2` which can be reduced to `p_1 x p_2`.
//
// The smallest positive solutions `S` is `n mod (p_2 t)`.
// If this is negative, then I have to add the modulo `p_2 t` once.
//
// # Note
// Calling ''tens()'' every time is overkill: when the current prime exceeds the previous value, then just multiply it by 10.
// But I doubt that you observe any noticeable performance gains when optimizing that aspect.

#include <iostream>
#include <vector>

// return the smallest 10^k bigger than x
// e.g. tens(456) = 1000 => "a 1 followed by as many 0s as x has digits"
unsigned long long tens(unsigned long long x)
{
  unsigned long long result = 1;
  while (result <= x)
    result *= 10;
  return result;
}

// when you are too lazy to type and your CPU is too fast ...
// no, seriously: quite useful for verifying small solutions
unsigned long long bruteForce(unsigned long long smallPrime, unsigned long long largePrime)
{
  // solve x == 0 mod largePrime
  // and   x == smallPrime mod tens(smallPrime)

  // find 10^k with the minimum amount of zeros
  auto shift = tens(smallPrime);

  // start with smallest possible value
  auto result = shift + smallPrime;
  // is it a multiple of b ?
  while (result % largePrime != 0)
    result += shift; // no, keep going ...

  return result;
}

// extended Euclidean algorithm
struct ExtendedGcd
{
  // this typedef allows me to switch easily between int and long long
  typedef long long Number;

  // find solutions x and y (so-called Bezout coefficients)
  ExtendedGcd(Number a, Number b)
  {
    // iterative algorithm from https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
    Number s = 0, lastS = 1;
    Number t = 1, lastT = 0;
    Number r = b, lastR = a; // remainder
    while (r != 0)
    {
      Number quotient = lastR / r;
      Number tmp;
      tmp = lastR; lastR = r; r = tmp - quotient * r;
      tmp = lastS; lastS = s; s = tmp - quotient * s;
      tmp = lastT; lastT = t; t = tmp - quotient * t;
    }

    // fill members
    gcd = lastR;
    x   = lastS;
    y   = lastT;
  }

  // Bezout coefficients
  Number x;
  Number y;
  // just in case we need it, too ...
  Number gcd;
};

// use Chinese Remainder Theorem
unsigned long long chineseRemainderTheorem(unsigned int smallPrime, unsigned int largePrime)
{
  // solve x == 0 mod largePrime
  // and   x == smallPrime mod tens(smallPrime)
  // e.g. Wolfram Alpha "x = 0 mod 23, x = 19 mod 100" => 1219
  auto modulo1 = largePrime;
  auto modulo2 = tens(smallPrime);

  // apply extended Euclidean algorithm
  ExtendedGcd gcd(modulo1, modulo2);

  //long long result = 0 * gcd.y * modulo2 + (long long)smallPrime * gcd.x * modulo1;
  // => multiplying by zero cancels half of the equation
  auto result = smallPrime * gcd.x * modulo1;

  // reduce to smallest solution
  auto product = modulo1 * modulo2;
  result %= (long long)product; // make sure it's a signed modulo
  // "too small" ?
  if (result < 0)
    result += product;

  return result;
}

int main()
{
  // result
  unsigned long long sum = 0;

  // sieve based on trial division
  unsigned int lastPrime = 2;
  std::vector<unsigned int> primes = { lastPrime };
  for (unsigned int i = 3; ; i += 2)
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

    // no prime ?
    if (!isPrime)
      continue;

    auto lastPrime = primes.back();
    primes.push_back(i);

    // find solution
    if (lastPrime >= 5)
    {
      //sum += bruteForce(lastPrime, i);
      sum += chineseRemainderTheorem(lastPrime, i);
    }

    // done ?
    if (i > 1000000)
      break;
  }

  std::cout << sum << std::endl;
  return 0;
}

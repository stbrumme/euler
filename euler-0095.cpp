// ////////////////////////////////////////////////////////
// # Title
// Amicable chains
//
// # URL
// https://projecteuler.net/problem=95
// http://euler.stephan-brumme.com/95/
//
// # Problem
// The proper divisors of a number are all the divisors excluding the number itself.
// For example, the proper divisors of 28 are 1, 2, 4, 7, and 14. As the sum of these divisors is equal to 28, we call it a perfect number.
//
// Interestingly the sum of the proper divisors of 220 is 284 and the sum of the proper divisors of 284 is 220, forming a chain of two numbers.
// For this reason, 220 and 284 are called an amicable pair.
//
// Perhaps less well known are longer chains. For example, starting with 12496, we form a chain of five numbers:
//
// 12496 ==> 14288 ==> 15472 ==> 14536 ==> 14264 (==> 12496 ==> ...)
//
// Since this chain returns to its starting point, it is called an amicable chain.
//
// Find the smallest member of the longest amicable chain with no element exceeding one million.
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// A pre-computation step creates a container ''divsum'' such that ''divsum[x]'' is the sum of all proper divisors of ''x''.
// My function ''getSum()'' from problem 21 produces the correct sum of all proper divisors but turns out to be too slow,
// therefore I searched the web for faster algorithms and found this one:
// 1. split a number into its prime factors `x = p_1^{e_1} * p_2^{e_2} * ...`
//    where `p_1`, `p_2`, ... are the primes and `e_1`, `e_2`, ... how often they appear in `x` (=exponents), for example `96 = 2^5 * 3^1`
// 2. then the sum of all divisors is `(2^0 + 2^1 + 2^2 + 2^3 + 2^4 + 2^5) * (3^0 + 3^1) = 252`
// 3. but since we need only the proper divisors instead of all divisors (which include the number itself): `252 - 96 = 156`
// ==> ''divsum[96] = 156''
//
// The chain algorithm appends ''chain[i] = divsum[chain[i - 1]]'' until:
// 1. it's the initial number, then an amicable chain was found or
// 2. it's a number lower than the initial number ==> abort because we should have seen this loop before, when we processed that lower number or
// 3. it's a number above the limit ==> abort according to problem description ("no element exceeding one million") or
// 4. it's a number that is already part of the chain ==> a loop
//
// If the chain is longer than anything before than its first element is stored in ''smallestMember'' (it must be the first element of the chain).

#include <iostream>
#include <vector>

int main()
{
  unsigned int limit;
  std::cin >> limit;

  // the usual prime sieve
  std::vector<unsigned int> primes;
  primes.push_back(2);
  for (unsigned int i = 3; i <= limit; i += 2)
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

    // yes, we have a prime number
    if (isPrime)
      primes.push_back(i);
  }

  // initial mapping of each number to its proper divisor's sum
  std::vector<unsigned int> divsum(limit + 1, 0);
  for (unsigned int i = 2; i <= limit; i++)
  {
    unsigned int sum = 1;
    unsigned int reduce = i;
    for (auto p : primes)
    {
      // note: reduce itself might be prime in the end
      if (p*p > reduce)
        break;

      // divide by all primes
      unsigned int factor = 1;
      while (reduce % p == 0)
      {
        reduce /= p;

        // add 1 to each exponent, e.g. p^0 + p^1 becomes p^1 + p^2
        factor *= p;
        // add a new term p^0
        factor++;
      }
      sum *= factor;
    }

    // if a large prime was left over
    if (reduce > 1 && reduce < i)
      sum *= reduce + 1;

    // subtract number itself if it isn't a prime
    if (sum > 1)
      sum -= i;

    divsum[i] = sum;
  }

  // loop until numbers are mapped to themselves (or get stuck in a loop)
  unsigned int longestChain = 0;
  unsigned int smallestMember = limit;
  for (unsigned int i = 1; i <= limit; i++)
  {
    // re-use the same vector over and over again to avoid memory re-allocations
    static std::vector<unsigned int> chain;
    chain.clear();
    chain.push_back(i);

    // until we:
    // 1. return to i or
    // 2. exceed the limit or
    // 3. get stuck in a loop
    while (true)
    {
      unsigned int add = divsum[chain.back()];
      chain.push_back(add);

      // yes, we found an amicable chain
      if (add == i)
        break;

      // can't be a new shorter loop:
      // we have already seen this number in an earlier iteration
      if (add < i)
        break;

      // abort if limit exceeded
      if (add > limit)
        break;

      // stuck in a loop ?
      bool isLoop = false;
      for (size_t j = 1; j < chain.size() - 1; j++) // except last element
        if (add == chain[j])
        {
          isLoop = true;
          break;
        }
      if (isLoop)
        break;
    }

    // did we return to i ?
    if (chain.back() != i)
      continue;

    // too short ?
    if (chain.size() < longestChain)
      continue;

    // shorter chain ?
    if (longestChain < chain.size())
    {
      longestChain = chain.size();
      smallestMember = chain.front();
    }
  }

  std::cout << smallestMember << std::endl;
  return 0;
}

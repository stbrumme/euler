// ////////////////////////////////////////////////////////
// # Title
// Counting numbers with at least four distinct prime factors less than 100
//
// # URL
// https://projecteuler.net/problem=268
// http://euler.stephan-brumme.com/268/
//
// # Problem
// It can be verified that there are 23 positive integers less than 1000 that are divisible by at least four distinct primes less than 100.
//
// Find how many positive integers less than `10^16` are divisible by at least four distinct primes less than 100.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// There are 25 primes less than 100.
// I start a loop running from 0 to `2^25`. I multiply all the n-th primes if the n-th bit of the counter (named ''mask'') is set.
// If ''mask'' has at least four bits set then the product of those primes needs to be further analyzed:
// - ''product'' can exceed `10^16` and cause strange results, reject all those large products
// - there are `10^16 / product` numbers divisible by ''product''
//
// I have to be careful to avoid counting the same numbers twice:
// for example 2310 is divisible by 2,3,5,7 and 11 (in fact 2*3*5*7*11 = 2310). But 2310 is also divisible by (2,3,5,7), (2,3,5,11), (2,5,7,11) and (3,5,7,11).
// That means (2,3,5,7,11) "overlaps" all combinations of its primes where I take one prime away.
// The tetrahedral numbers can compute this "overlap" (see https://en.wikipedia.org/wiki/Tetrahedral_number ) together with
// the inclusion-exclusion principle (see https://en.wikipedia.org/wiki/Inclusion%E2%80%93exclusion_principle ).
//
// In plain English: I add the count of numbers divisible by 4 primes, subtract the count divisible by 5 primes, add 6s, subtract 7s, etc.
// ==> if ''numBits'' is even, then add else subtract, the fast way is just to look at the lowest bit of the ''numBits'' counter

#include <iostream>
#include <vector>

// all 25 primes less than 100
std::vector<unsigned int> primes = {  2,  3,  5,  7, 11,
                                     13, 17, 19, 23, 29,
                                     31, 37, 41, 43, 47,
                                     53, 59, 61, 67, 71,
                                     73, 79, 83, 89, 97 };

// count numbers with at least "minPrimes" prime factors
// slow version - helping me while debugging the fast code below
unsigned int bruteForce(unsigned int limit, unsigned int minPrimes)
{
  unsigned int result = 0;
  for (unsigned int i = 1; i <= limit; i++)
  {
    // count distinct prime factors
    unsigned int numPrimeFactors = 0;
    for (auto p : primes)
      if (i % p == 0)
        numPrimeFactors++;

    // at least four prime factors ?
    if (numPrimeFactors >= minPrimes)
      result++;
  }
  return result;
}

// return the n-th tetrahedral number
unsigned int tetrahedral(unsigned int n)
{
  // see https://en.wikipedia.org/wiki/Tetrahedral_number
  return n * (n + 1) * (n + 2) / 6;
}

int main()
{
  // at least 4 primes, search up to 10^16
  unsigned int   minPrimes = 4;
  unsigned int   numPrimes = primes.size();
  unsigned long long limit = 10000000000000000ULL;

  std::cin >> minPrimes >> numPrimes >> limit;

  // find result for small limits
  //std::cout << bruteForce(limit, minPrimes) << std::endl;

  // precompute tetrahedral numbers
  std::vector<unsigned long long> count(numPrimes + 1, 0);
  for (unsigned int i = minPrimes; i < count.size(); i++)
    count[i] = tetrahedral(i - minPrimes + 1); // same as choose(i - 1, minPrimes - 1)

  unsigned long long sum = 0;
  unsigned int maxMask = (1 << numPrimes) - 1;
  for (unsigned int mask = 0; mask < maxMask; mask++)
  {
    // multiply all primes
    unsigned long long product = 1;
    // but don't exceed 10^16
    bool tooLarge = false;
    // at least four bits must be set (=> pick at least four different primes)
    unsigned int numBits = 0;
    // look at each prime
    for (unsigned int current = 0; current < numPrimes; current++)
    {
      // is this prime used ?
      unsigned int bitpos = 1 << current;
      if ((mask & bitpos) == 0)
        continue;

      // include this prime factor
      numBits++;
      product *= primes[current];

      // avoid too large products (beware of 64 bit overflows ...)
      if (product > limit)
      {
        tooLarge = true;

        // speed optimization:
        // if the next mask has the same number of bits set (or more), then it will exceed the limit as well
        // because the primes are getting larger
        // => get closer to the next number with the less bits set
        // add the lowest set bit to the current number
        // => worst case: same number of bits if the last bit is a single bit
        // => best  case: several bits next to the lowest bit are set to and all flip to zeros (and a zero becomes a one)
        auto withLowestBit = mask & (mask - 1);
        auto lowestBit     = mask - withLowestBit;
        mask += lowestBit;
        // hope for the best case ...
        while (mask & (lowestBit << 1))
        {
          lowestBit <<= 1;
          mask += lowestBit;
        }

        mask--; // because the for-loop will add 1 in each iteration
        break;
      }
    }

    // skip if too few primes or their product became too large
    if (tooLarge || numBits < minPrimes)
      continue;

    // count numbers divisible by the current primes
    auto divisible = limit / product;
    // they were part of products with less primes, too, hence compute the "overlap"
    divisible *= count[numBits];

    // add if even number of primes, else subtract (inclusion-exclusion principle)
    if (numBits & 1)
      sum -= divisible;
    else
      sum += divisible;
  }

  // that's it !
  std::cout << sum << std::endl;
  return 0;
}

// ////////////////////////////////////////////////////////
// # Title
// Cyclic numbers
//
// # URL
// https://projecteuler.net/problem=358
// http://euler.stephan-brumme.com/358/
//
// # Problem
// A cyclic number with n digits has a very interesting property:
// When it is multiplied by 1, 2, 3, 4, ... n, all the products have exactly the same digits, in the same order, but rotated in a circular fashion!
//
// The smallest cyclic number is the 6-digit number 142857 :
// `142857 * 1 = 142857`
// `142857 * 2 = 285714`
// `142857 * 3 = 428571`
// `142857 * 4 = 571428`
// `142857 * 5 = 714285`
// `142857 * 6 = 857142`
//
// The next cyclic number is 0588235294117647 with 16 digits :
// `0588235294117647 * 1 = 0588235294117647`
// `0588235294117647 * 2 = 1176470588235294`
// `0588235294117647 * 3 = 1764705882352941`
// ...
// `0588235294117647 * 16 = 9411764705882352`
//
// Note that for cyclic numbers, leading zeros are important.
//
// There is only one cyclic number for which, the eleven leftmost digits are 00000000137 and the five rightmost digits are 56789
// (i.e., it has the form 00000000137...56789 with an unknown number of digits in the middle).
// Find the sum of all its digits.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// Wikipedia explains basically all I need (see https://en.wikipedia.org/wiki/Cyclic_number ).
// Cyclic numbers `c` are related to so-called reptend primes: `c = dfrac{b^{p-1}-1}{p}`
//
// `b` is the base and for decimal numbers `b=10`: `c = dfrac{10^{p-1}-1}{p}`
// The numerator is simply a number consisting of 9s only (and the number of its digits is `p-1`) and `p` is a reptend prime.
// A list of small reptend primes can be found on the Wikipedia page or in OEIS A001913.
//
// The smallest reptend prime is 7. For the first example the formula becomes:
// `c = dfrac{10^{7-1}-1}{7} = dfrac{999999}{7} = 142857`
//
// The next reptend prime is 17. And it verifies the second example:
// `c = dfrac{10^{17-1}-1}{17} = dfrac{9999999999999999}{17} = 588235294117647`
// Note that I still must prepend a zero to that cyclic number so it has `p-1 = 17-1 = 16` digits.
//
// I know that the cyclic number ends with ...56789:
// `56789 * p mod 100000 = 99999`
// `56789 * p + 1 mod 100000 = 0`
// The function ''endsWith56789()'' checks exactly that.
//
// Furthermore, the cyclic number must start with 00000000137...:
// the cyclic number also appears as the first digits of `1/p`, so `1/p` must be `0.00000000137...`
// The function ''startsWith137()'' verifies this property.
//
// The algorithm found three candidates for the reptend prime - but according to the problem statement the number must be unique.
// When I wrote the ''cycleDigitSum()'' function I observed that two of these candidates have a cycle length shorter than `p-1`.
//
// The digit sum is computed as follows:
// - start with a fraction `n/p = 1/p`
// - multiply it by 10 (==> `n` becomes `10n` in the first iteration)
// - extract the integer part `i = \lfloor n/p \rfloor`
// - `n := n - i * p` so that `n/p < 1` again
// - add all `i`
// - that's more or less the long division algorithm taught in school (plus adding the digits)
//
// Whenever `n = 1` is encountered I know that the next cycle starts. It must only happen after exactly `p - 1` iterations because the cyclic number of a reptend prime has exactly `p - 1` digits.
// Only one number out of the three candidates satisfies this condition ==> and that's the correct result.
//
// # Note
// There is a faster way to calculate the digit sum:
// when a cyclic number is split in two halves and I add the n-th digit of the first half to the n-th digit of the second half then their sum is always 9.
// The number of digits of a cyclic number is `p - 1`, so there will be `(p - 1) / 2` nines, which gives a digit sum of `9 * (p - 1) / 2`.
// However, my ''cyclicDigitSum'' function serves two purposes: it calculates the digit sum __and__ checks the cycle length.
// I haven't found a faster way for the latter.
//
// I cheated a little bit: once I knew the result I reversed the ''for''-loop to start at 750000000 and go backwards instead of starting at 1 and going forward.
// It reduced the total time from 13 to 9.6 seconds. Now the program spends most of the time in ''cyclicDigitSum()''.
//
// # Alternative
// All primes satisfying ''endsWith56789()'' end with ...09891. This reduces the search space considerably.
// All primes satisfying ''startsWith137()'' are between 724637681 and 729927007.
// Only three primes satisfy both conditions - that means you don't really need my prime sieve and could replace it by a much faster Miller-Rabin test (saving lots of memory, too).

#include <iostream>
#include <vector>

// ---------- code from my toolbox ----------

// odd prime numbers are marked as "true" in a bitvector
std::vector<bool> sieve;

// return true, if x is a prime number
bool isPrime(unsigned int x)
{
  // handle even numbers
  if ((x & 1) == 0)
    return x == 2;

  // lookup for odd numbers
  return sieve[x >> 1];
}

// find all prime numbers from 2 to size
void fillSieve(unsigned int size)
{
  // store only odd numbers
  const unsigned int half = (size >> 1) + 1;

  // allocate memory
  sieve.resize(half, true);
  // 1 is not a prime number
  sieve[0] = false;

  // process all relevant prime factors
  for (unsigned int i = 1; 2*i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3*i+1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2*i+1;
      }
    }
}

// ---------- problem-specific code ----------

// return true if prime * 56789 ends with ...99999
bool endsWith56789(unsigned int prime)
{
  // cyclic = (10^(p-1) - 1) / p
  // cyclic * p = 10^(p-1) - 1
  // the last 5 digits of cyclic are ...56789
  // => cyclic * p must end with 5 nines (99999)
  // => add 1 to both sides => 99999 + 1 mod 100000 = 0
  auto cyclic  =  56789;
  auto modulo  = 100000;
  auto product = cyclic * (unsigned long long)prime;
  return (product + 1) % modulo == 0;
}

// return true if 9999...9999 / p = 137....
bool startsWith137(unsigned int prime)
{
  double cyclic = 1.0 / prime;
  return cyclic > 0.00000000137 &&
         cyclic < 0.00000000138;
}

// return the digit sum if the prime produces a cyclic number with a cycle length of prime - 1, else return 0
unsigned int cyclicDigitSum(unsigned int prime)
{
  // compute all digits of the cycle:
  // compute 1/prime, 10/prime, 100/prime, etc.
  // whenever the
  unsigned long long result = 0;
  unsigned long long numerator   = 1;
  unsigned int cycleLength = 0;
  while (true)
  {
    cycleLength++;
    // produce next leading digit
    numerator *= 10;
    result    += numerator / prime;
    numerator %= prime;

    // cycle found (or cycle too long)
    if (numerator <= 1 || cycleLength == prime)
      break;
  }

  // wrong cycle length
  if (cycleLength != prime - 1)
    return 0;

  // return digit sum
  return result;
}

int main()
{
  unsigned int limit = 750000000;
  fillSieve(limit);

  // check each prime
  //for (unsigned int prime = 1; ; prime += 2)
  for (unsigned int prime = limit-1; prime > 1; prime -= 2)
  {
    if (!isPrime(prime))
      continue;

    // verify digits
    if (startsWith137(prime) && endsWith56789(prime))
    {
      // compute digit sum
      auto digitSum = cyclicDigitSum(prime);
      if (digitSum > 0) // accept only if the prime actually produces a cycle number with correct cycle length
      {
        std::cout << digitSum << std::endl;
        break;
      }
    }
  }
  return 0;
}

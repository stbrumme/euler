// ////////////////////////////////////////////////////////
// # Title
// Digital root clocks
//
// # URL
// https://projecteuler.net/problem=315
// http://euler.stephan-brumme.com/315/
//
// # Problem
// ![clocks](p315_clocks.gif)
//
// Sam and Max are asked to transform two digital clocks into two "digital root" clocks.
// A digital root clock is a digital clock that calculates digital roots step by step.
//
// When a clock is fed a number, it will show it and then it will start the calculation, showing all the intermediate values until it gets to the result.
// For example, if the clock is fed the number 137, it will show: "137" ==> "11" ==> "2" and then it will go black, waiting for the next number.
//
// Every digital number consists of some light segments: three horizontal (top, middle, bottom) and four vertical (top-left, top-right, bottom-left, bottom-right).
// Number "1" is made of vertical top-right and bottom-right, number "4" is made by middle horizontal and vertical top-left, top-right and bottom-right. Number "8" lights them all.
//
// The clocks consume energy only when segments are turned on/off.
// To turn on a "2" will cost 5 transitions, while a "7" will cost only 4 transitions.
//
// Sam and Max built two different clocks.
//
// Sam's clock is fed e.g. number 137: the clock shows "137", then the panel is turned off, then the next number ("11") is turned on, then the panel is turned off again and finally the last number ("2") is turned on and, after some time, off.
// For the example, with number 137, Sam's clock requires:
// "137": (2 + 5 + 4) * 2 = 22 transitions ("137" on/off).
// "11": (2 + 2) * 2 = 8 transitions ("11" on/off).
// "2": (5) * 2 = 10 transitions ("2" on/off).
// For a grand total of 40 transitions.
//
// Max's clock works differently. Instead of turning off the whole panel, it is smart enough to turn off only those segments that won't be needed for the next number.
// For number 137, Max's clock requires:
// "137": 2 + 5 + 4 = 11 transitions ("137" on)
// 7 transitions (to turn off the segments that are not needed for number "11").
// "11": 0 transitions (number "11" is already turned on correctly)
// 3 transitions (to turn off the first "1" and the bottom part of the second "1"; the top part is common with number "2").
// "2": 4 transitions (to turn on the remaining segments in order to get a "2")
// 5 transitions (to turn off number "2").
// For a grand total of 30 transitions.
//
// Of course, Max's clock consumes less power than Sam's one. The two clocks are fed all the prime numbers between `A = 10^7` and `B = 2 * 10^7`.
//
// Find the difference between the total number of transitions needed by Sam's clock and that needed by Max's one.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// I encode all lit segments as bitmasks in ''Segments''. Counting the set bits (''popcnt'') is equivalent to counting the lit segments.
// There are at most 8 digits, hence a 64 bit integer represents all segments of a number.
//
// Sam's algorithm can be found in ''sam'': switching on and off all segments "costs" 2x ''popcnt(getSegments(x))''.
//
// Max's algorithm (see ''max'') is a bit more complicated: I have to compare all lit segments of the previous number to the lit segments of the current number.
// When I XOR both bitmasks then I get a new bitmasks with all segments that change (either have to be turned off or turnd on).
// Counting the set bits in the XORed value tells me the number of transitions.
// Before the first and after the last number all lit segments transition from and to the "empty" state.
//
// Both routines work recursively because I hoped to improve the execution time by caching / memoizing results:
// After the first step the ''digitSum'' is below 70. However, the simple memoization in ''sam'' didn't help much
// and therefore I didn't even attempt to code something similiar in ''max''.
//
// # Note
// ''popcnt'' was named after the super-fast CPU instruction which counts bits.
// My G++ compiler shows the result after 0.12 seconds when that intrinsic can be used but needs 0.17 seconds with my own alternative code
// (which still relies on tricky bit manipulation, see http://bits.stephan-brumme.com ).

#include <iostream>
#include <vector>

// represent digits as bitmasks, lit segments are 1
const unsigned char Segments[10] =
{
  // assume the following numbering scheme for the seven segments:
  //  000
  // 1   2
  // 1   2
  //  333
  // 4   5
  // 4   5
  //  666
  (1 << 0) | (1 << 1) | (1 << 2) | (0 << 3) | (1 << 4) | (1 << 5) | (1 << 6), // digit 0 = 0x77
  (0 << 0) | (0 << 1) | (1 << 2) | (0 << 3) | (0 << 4) | (1 << 5) | (0 << 6), // digit 1 = 0x24
  (1 << 0) | (0 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (0 << 5) | (1 << 6), // digit 2 = 0x5d
  (1 << 0) | (0 << 1) | (1 << 2) | (1 << 3) | (0 << 4) | (1 << 5) | (1 << 6), // digit 3 = 0x6d
  (0 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (0 << 4) | (1 << 5) | (0 << 6), // digit 4 = 0x2e
  (1 << 0) | (1 << 1) | (0 << 2) | (1 << 3) | (0 << 4) | (1 << 5) | (1 << 6), // digit 5 = 0x6b
  (1 << 0) | (1 << 1) | (0 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6), // digit 6 = 0x3b
  (1 << 0) | (1 << 1) | (1 << 2) | (0 << 3) | (0 << 4) | (1 << 5) | (0 << 6), // digit 7 = 0x27
  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6), // digit 8 = 0x7f
  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (0 << 4) | (1 << 5) | (1 << 6)  // digit 9 = 0x6f
};


// ---------- standard prime sieve from my toolbox ----------

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
  for (unsigned int i = 1; 2 * i*i < half; i++)
    // do we have a prime factor ?
    if (sieve[i])
    {
      // mark all its multiples as false
      unsigned int current = 3 * i + 1;
      while (current < half)
      {
        sieve[current] = false;
        current += 2 * i + 1;
      }
    }
}

// ---------- problem solution ----------

// return sum of all digits
unsigned int digitSum(unsigned int x)
{
  unsigned int result = 0;
  while (x > 0)
  {
    result += x % 10;
    x      /= 10;
  }
  return result;
}

// convert number to its bitmasks, 8 bits per digit => x must not exceed 99999999
unsigned long long getSegments(unsigned int x)
{
  unsigned long long result = 0;
  unsigned int shift = 0;
  while (x > 0)
  {
    unsigned long long current = Segments[x % 10];
    result |= current << shift;
    x      /= 10;
    shift  += 8;
  }
  return result;
}

// count bits which are set to 1
unsigned int popcnt(unsigned long long x)
{
#ifdef __GNUC__
  return __builtin_popcount(x);
#endif

  unsigned int result = 0;
  while (x > 0)
  {
    // clear right-most bit
    x &= x - 1;
    result++;
  }
  return result;
}

// process all steps with strategy of Sam, return number of transitions
unsigned int sam(unsigned int x)
{
  // memoize (runs quite fast without, too)
  static std::vector<unsigned int> cache(100, 0);
  if (x < cache.size() && cache[x] > 0)
    return cache[x];

  // display was clear
  auto segments = getSegments(x);
  // switch (intermediate) number on and off
  auto result   = 2 * popcnt(segments);

  // need further iterations ?
  if (x > 9)
    result += sam(digitSum(x));

  if (x < cache.size())
    cache[x] = result;
  return result;
}

// process all steps with strategy of Max, return number of transitions
unsigned int max(unsigned int x, unsigned long long previousSegments = 0)
{
  auto segments    = getSegments(x);
  auto transitions = segments ^ previousSegments;
  auto result      = popcnt(transitions);

  if (x > 9)
    // morph to next number
    result += max(digitSum(x), segments);
  else
    // fade out
    result += popcnt(segments);

  return result;
}

int main()
{
  // search range
  unsigned int from = 10000000;
  unsigned int to   = 20000000;
  std::cin >> from >> to;

  // for live input only
  if (from > to)
    return 1;

  // generate prime sieve
  fillSieve(to);

  // count all digits of Sam and Max
  unsigned long long sumSam = 0;
  unsigned long long sumMax = 0;
  for (auto i = (from | 1); i <= to; i += 2) // odd numbers only
  {
    if (!isPrime(i))
      continue;

    sumSam += sam(i);
    sumMax += max(i);
  }

  // Sam >= Max, display the difference
  std::cout << sumSam - sumMax << std::endl;
  return 0;
}

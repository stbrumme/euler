// ////////////////////////////////////////////////////////
// # Title
// Double pandigital number divisible by 11
//
// # URL
// https://projecteuler.net/problem=491
// http://euler.stephan-brumme.com/491/
//
// # Problem
// We call a positive integer double pandigital if it uses all the digits 0 to 9 exactly twice (with no leading zero).
// For example, 40561817703823564929 is one such number.
//
// How many double pandigital numbers are divisible by 11?
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// ''bruteForce'' counts all permutations which are divisible by 11.
// It finds the correct result in a reasonable amount of time when the highest digit is 6.
// And of course that simple approach is way to slow when the highest digit is 9.
//
// A number is divisible by 11 if the difference of the sum of the digits at odd and the even positions is divisible by 11.
// 40561817703823564929 is __not__ divisible by 11 because 4+5+1+1+7+0+2+5+4+2 = 31 and 0+6+8+7+0+8+3+6+9+9 = 56 and 56 - 31= 25 is not divisible by 11.
// There is actually an astonishing variety of rules for divisibility by 11: https://en.wikipedia.org/wiki/Divisibility_rule
//
// All double pandigital numbers are permutations of the string ''"00112233445566778899"'' (excluding leading zeros).
// Ten of these digits will be at odd positions and ten at even positions.
// If I create a bitmask which digits will be at odd positions then I need a 20-bit integer where exactly 10 bits are set.
//
// All bitmasks with exactly 10 bits set are between 00000000001111111111b (''minBitmask'') and 11111111110000000000b (''maxBitmask'').
// The nice function ''nextNumberWithSameBits(x)'' creates the next number with the same number of bits set as ''x'' and was first published in HAKMEM 175 (often called "snoob").
//
// However, there are a few bordercases:
// I can choose a digit once, twice or not at all to appear at an odd position. A bitmask might be:
// || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 || 1 ||
// || 9 || 9 || 8 || 8 || 7 || 7 || 6 || 6 || 5 || 5 || 4 || 4 || 3 || 3 || 2 || 2 || 1 || 1 || 0 || 0 ||
// || 0 || 1 || 0 || 0 || 1 || 1 || 0 || 1 || 0 || 1 || 0 || 1 || 1 || 1 || 0 || 0 || 0 || 1 || 0 || 1 ||
// ==> exactly 10 bits set
// For each digit I must only accept the bit patterns:
// 01 (=> use that digit once, e.g. for 9,6,...),
// 11 (twice, e.g. for 7 and 3) and
// 00 (none, e.g. for 8 and 2)
// I __must not__ accept bit pattern 10 because it's the same as 01 (see my loop with the variable ''reduce'').
//
// The sum of all digits is `2 * T(9)` where `T` is the triangular number:
// `digitSum = 2 * dfrac{9 * (9 + 1)}{2} = 90`
//
// The sum of all digits at even positions can be computed when I know the sum of all digits at odd positions:
// `sumEven = digitSum - sumOdd`
//
// And the difference of `sumEven` and `sumOdd` has to be divisible by 11:
// `(sumEven - sumOdd) == 0 mod 11`
// `((digitSum - sumOdd) - sumOdd) == 0 mod 11`
// `(digitSum - 2 * sumOdd) == 0 mod 11`
//
// Whenever I know that the bitmasks generate matching numbers I have to figure out how many different numbers are possible.
// If `x_i` represents how often `x` appears at odd positions then `x_i` can be either 0, 1 or 2.
// The total number of permutations will be:
// `p(x) = dfrac{10!}{x_0!x_1!x_2!...x_9!}`
//
// If `y_i` represents how often `y` appears at even positions then `y_i` can be either 0, 1 or 2. The formula remains the same:
// `p(y) = dfrac{10!}{y_0!y_1!y_2!...y_9!}`
//
// And the sum of all `p(x)p(y)` will be the result.
// Interesting is that the number of digits that appear twice is the same for `p(x)` and `p(y)`. Even though a few `x_i != y_i`, it's still `p(x) = p(y)`.
// There my code simplifies to ''result += permutationsRepeated[repeated] * permutationsRepeated[repeated];''
//
// Excluding leading zeros is simple: just multiply with `frac{9}{10}`.

#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>

// count all numbers, where highest digit is maxDigit
unsigned long long bruteForce(unsigned int maxDigit)
{
  unsigned long long result = 0;

  // keep only a few digits
  std::string digits = "10012233445566778899"; // smallest number without a leading zero
  digits.resize(maxDigit * 2 + 2);

  // count numbers divisible by 11
  do
  {
    if (std::stoll(digits) % 11 == 0)
      result++;
  } while (std::next_permutation(digits.begin(), digits.end()));

  return result;
}

// return next higher number with same number of bits set
unsigned int nextNumberWithSameBits(unsigned int x)
{
  // from HAKMEM 175
  // see http://www.hackersdelight.org/hdcodetxt/snoob.c.txt
  auto smallest = x & -x;
  auto ripple = x + smallest;
  auto ones = ripple ^ x;
  return ((ones >> 2) / smallest) | ripple;
}

// count numbers by using permutations
unsigned long long fast(unsigned int maxDigit)
{
  unsigned long long result = 0;

  int  digitSum  = 2 * (maxDigit + 1) * maxDigit / 2; // = (maxDigit + 1) * maxDigit = 90
  auto numDigits = 2 * (maxDigit + 1);                // = 20

  // precompute number of permutations with repeated elements
  unsigned long long factorial = 1;
  for (unsigned int i = 1; i <= maxDigit + 1; i++)
    factorial *= i; // 10! = 3628800
  // 10!, 10!/2!, 10!/2!2!, 10!/2!2!2!, ...
  unsigned long long permutationsRepeated[10];
  for (unsigned int i = 0; i <= maxDigit; i++)
    permutationsRepeated[i] = factorial >> i;

  // smallest bitmask where maxDigits bits are set
  auto minBitmask = (1 << (maxDigit + 1)) - 1;
  // largest  bitmask where maxDigits bits are set
  auto maxBitmask = minBitmask << (maxDigit + 1);
  // process all bitmasks where maxDigits bits are set
  for (auto bitmask = minBitmask; bitmask <= maxBitmask; bitmask = nextNumberWithSameBits(bitmask))
  {
    // when picking a number for odd positions:
    // choose a digit once, twice or not at all
    // e.g. bits represent             99887766554433221100
    // and the current choice might be 01001101010111000101
    // (exactly 10 bits set)
    // for each digit only accept the bit patterns 01 (once, e.g. for 9,6,...),
    //                                             11 (twice, e.g. for 7 and 3) and
    //                                             00 (none, e.g. for 8 and 2)
    // DO NOT accept 10 because it's the same as 01
    // ok will be false if bit pattern 10 is found
    auto reduce = bitmask;
    bool ok = true;
    while (reduce > 0)
    {
      // lowest two bits are 10b (=2 decimal) ?
      if ((reduce & 3) == 2)
      {
        ok = false;
        break;
      }
      // next two bits
      reduce >>= 2;
    }
    if (!ok)
      continue;

    // add all digits at odd positions and count how many digits are repeatedly present at odd positions
    int sumOdd = 0;
    auto repeated = 0; // it's actually the same value for odd and even positions
    for (unsigned int pos = 0; pos < numDigits; pos++)
    {
      // bit set ? use that digit
      if (bitmask & (1 << pos))
      {
        sumOdd += pos / 2;

        // use that digit twice ? (bit pattern 11b)
        if (pos & 1) // same as repeated += pos & 1;
          repeated++;
      }
    }

    // divisible by 11 ?
    if ((digitSum - 2*sumOdd) % 11 == 0)
      result += permutationsRepeated[repeated] * permutationsRepeated[repeated];
  }

  // exclude leading zeros
  return result * maxDigit / (maxDigit + 1);
}

int main()
{
  unsigned int numDigits = 9;
  std::cin >> numDigits;

  //std::cout << bruteForce(numDigits) << std::endl;
  std::cout << fast(numDigits) << std::endl;
  return 0;
}

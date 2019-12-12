// ////////////////////////////////////////////////////////
// # Title
// Counting Digits
//
// # URL
// https://projecteuler.net/problem=156
// http://euler.stephan-brumme.com/156/
//
// # Problem
// Starting from zero the natural numbers are written down in base 10 like this:
// 0 1 2 3 4 5 6 7 8 9 10 11 12....
//
// Consider the digit `d=1`. After we write down each number `n`, we will update the number of ones that have occurred and call this number `f(n,1)`.
// The first values for `f(n,1)`, then, are as follows:
//
// ||  3  ||     4    ||
// ||!`n` ++ `f(n,1)` ||
// ||  0  ++     0    ||
// ||  1  ++     1    ||
// ||  2  ++     1    ||
// ||  3  ++     1    ||
// ||  4  ++     1    ||
// ||  5  ++     1    ||
// ||  6  ++     1    ||
// ||  7  ++     1    ||
// ||  8  ++     1    ||
// ||  9  ++     1    ||
// || 10  ++     2    ||
// || 11  ++     4    ||
// || 12  ++     5    ||
//
// Note that `f(n,1)` never equals 3.
// So the first two solutions of the equation `f(n,1)=n` are `n=0` and `n=1`. The next solution is `n=199981`.
//
// In the same manner the function `f(n,d)` gives the total number of digits d that have been written down after the number n has been written.
// In fact, for every digit `d != 0`, `0` is the first solution of the equation `f(n,d)=n`.
//
// Let `s(d)` be the sum of all the solutions for which `f(n,d)=n`.
// You are given that `s(1)=22786974071`.
//
// Find `sum{s(d)}` for `1 <= d <= 9`.
//
// Note: if, for some `n`, `f(n,d)=n` for more than one value of `d` this value of `n` is counted again for every value of `d` for which `f(n,d)=n`.
//
// # Solved by
// Stephan Brumme
// October 2017
//
// # Algorithm
// First I wrote a brute-force function ''countSingle'' that counts a digit in a single number.
// Obviously it's not the fastest approach to this problem, since `s(1) = 22786974071` ...
//
// Nevertheless, ''countSingle'' can be used to analyze all numbers and digits up to a million.
// And a certain pattern appeared:
// ||   5   ||  5  ||   5  ||   5   ||    5   ||
// ||!digit ++ <10 ++ <100 ++ <1000 ++ <10000 ||
// ||   1   ++   1 ++   20 ++   300 ++   4000 ||
// ||   2   ++   1 ++   20 ++   300 ++   4000 ||
// ||   3   ++   1 ++   20 ++   300 ++   4000 ||
// ||   4   ++   1 ++   20 ++   300 ++   4000 ||
// ||   5   ++   1 ++   20 ++   300 ++   4000 ||
// ||   6   ++   1 ++   20 ++   300 ++   4000 ||
// ||   7   ++   1 ++   20 ++   300 ++   4000 ||
// ||   8   ++   1 ++   20 ++   300 ++   4000 ||
// ||   9   ++   1 ++   20 ++   300 ++   4000 ||
//
// Each number `x` can be split into its parts (I don't want to use the word "digits" here ...), e.g. `5684 = 5 * 10^3 + 6 * 10^2 + 8 * 10^1 + 4 * 10^0`.
// And the same can be done with the digit-counting function `f(5684, i) = f(5000, i) + f(600, i) + f(80, i) + f(4, i)`
//
// My function ''count'' basically does exactly that: it split a number into its parts and counts how often each digit occurs:
// - find the left-most part by slowly multiplying ''shift'' by 10
// - increment ''multiplier'' as well, it will be the number shown in the table above
//
// A single number is trivial: if it's not small than ''digit'', then the count is 1.
// A number between 10 and 99 (inclusive) requires some work:
// - if the leading part is smaller than ''digit'', then ''digit'' doesn't appear there
// - if the leading part is larger  than ''digit'', then ''digit'' appeared 10x at that position
// - if the leading part is equal   to   ''digit'', then ''digit'' appeared as often as the right-most part plus 1
// - add the ''multiplier'' multiplied by the leading part
// - recursively call the function with the right-most part only, add the count to the current ''result''
//
// For example, `f(84, 9) =         8 * 1 =  8` because 9 can only be found in the right-most digit, multiply ''multiplier'' by the left-most digit.
// And          `f(84, 8) = 4 + 1 + 8 * 1 = 13` because 8 can be found 4+1 in the left-most digit (80, 81, 82, 83, 84) and 8x in the right-most digit.
// Finally      `f(84, 7) = 10 +    8 * 1 = 19` because 7 can be found 10x in the left-most digit and 8x in the right-most digit.
//
// Now I have a pretty fast function to count a single digit in a huge range of numbers, but where does that range end ?
// I create some test code and looked at the results of `f(n, d) / n` for `n = 10^i` (==> all powers of ten) and observed that the ratio exceeds 1 for `10^10`.
// Since my code is fast, I decided to add some safety margin and set ''Limit = 10^12'' (actually the highest number for any `f(n, d) = n` is `n = 80000000001`).
//
// Binary search is my tool to find all numbers `n` such that `f(n, d) = n`:
// - `n` is strictly monotonically increasing
// - `f(n, d)` is monotonically increasing
// - find the center of the current range `[from, to]` so that `center = dfrac{from + to}{2}`
// - only if `f(center, d) >= from` and `center >= f(from, d)` then there could be a number `n` in that range `[from, center]` where `f(n, d) = n`
// - similar logic applies for `f(to, d) >= center` and `to >= f(center, d)`
// - if the search narrows down the range to a single number then check if `f(n, d) = n`
//
// # Hackerrank
// The base can be between 2 and 10 and the analyzed digits can be modified by the user (instead of always computing `s(1) + s(2) + ... + s(9)`).
// I totally forgot about looking for a proper ''Limit'' depending on ''base'' but `10^12` worked perfectly fine for all test cases. Lucky me !

#include <iostream>

// decimal => 10
unsigned int base = 10; // change only for modified Hackerrank problem

// count digits in a single number
unsigned int countSingle(unsigned int digit, unsigned long long value)
{
  // actually the problem doesn't ask for zeros, so I could skip this special treatment of zeros ...
  if (value == 0 && digit == 0)
    return 1;

  unsigned int result = 0;
  while (value > 0)
  {
    if (value % base == digit)
      result++;
    value /= base;
  }

  return result;
}

// return f(value, digit)
unsigned long long count(unsigned int digit, unsigned long long value)
{
  // handle a single digit
  if (value < base)
    return value < digit ? 0 : 1;

  // find highest digit
  unsigned long long shift      = 1;
  unsigned long long multiplier = 0;
  while (shift * base <= value)
  {
    shift *= base;
    multiplier++;
  }
  multiplier *= shift / base;

  // split number: "first" represents the left-most digit and "remainder" is everything else
  auto first     = value / shift;
  auto remainder = value % shift;

  // count digit in "remainder"
  auto result = first * multiplier;  // full blocks
  result += count(digit, remainder); // partial block

  // count digit in "first"
  if (digit == first)
    result += remainder + 1;
  if (digit <  first && digit > 0) // I don't need to handle zero, but it felt wrong ...
    result += shift;

  return result;
}

// return sum of all numbers from <= x <= to where count(x, digit) = x
unsigned long long findAll(unsigned int digit, unsigned long long from, unsigned long long to)
{
  // narrowed down to a single number ?
  auto center = (from + to) / 2;
  if (from == center)
  {
    auto current = count(digit, from);
    if (current == from)
      return from;
    else
      return 0;
  }

  unsigned long long result = 0;

  auto countFrom   = count(digit, from);

#define FAST
#ifdef  FAST
  // matches often occur in bunches, try to resolve many at once
  while (countFrom == from && from < to)
  {
    result    += from;
    countFrom += countSingle(digit, ++from);
  }
  if (from >= to + 1)
    return result;
#endif

  center = (from + to) / 2;

  // binary subdivision
  auto countCenter = count(digit, center);
  auto countTo     = count(digit, to);

  // recursive search in lower half
  if (countCenter >= from   && center >= countFrom   && center > from)
    result += findAll(digit, from, center);
  // recursive search in upper half
  if (countTo     >= center && to     >= countCenter && center < to)
    result += findAll(digit, center, to);

  return result;
}

int main()
{
  // 10^12
  const auto Limit = 1000000000000ULL; // actually 80000000001 would suffice
  // will store the result
  unsigned long long sum = 0;

//#define ORIGINAL
#ifdef  ORIGINAL
  for (unsigned int digit = 1; digit < base; digit++)
    sum += findAll(digit, 0, Limit);

#else

  unsigned int tests;
  std::cin >> base >> tests;
  // read all digits and count them
  while (tests--)
  {
    unsigned int digit;
    std::cin >> digit;
    sum += findAll(digit, 0, Limit);
  }
#endif

  // done !
  std::cout << sum << std::endl;
  return 0;
}

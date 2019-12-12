// ////////////////////////////////////////////////////////
// # Title
// Find the largest 0 to 9 pandigital that can be formed by concatenating products
//
// # URL
// https://projecteuler.net/problem=170
// http://euler.stephan-brumme.com/170/
//
// # Problem
// Take the number 6 and multiply it by each of 1273 and 9854:
// `6 * 1273 = 7638`
// `6 * 9854 = 59124`
//
// By concatenating these products we get the 1 to 9 pandigital 763859124. We will call 763859124 the "concatenated product of 6 and (1273,9854)". Notice too, that the concatenation of the input numbers, 612739854, is also 1 to 9 pandigital.
//
// The same can be done for 0 to 9 pandigital numbers.
//
// What is the largest 0 to 9 pandigital 10-digit concatenated product of an integer with two or more other integers, such that the concatenation of the input numbers is also a 0 to 9 pandigital 10-digit number?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// I analyze all pandigital numbers, beginning with the largest (''9876543210'') and slowly decrement towards ''1023456789''.
// The easiest way is to store the pandigital number in a ''std::string'' and call ''std::prev_permutation'' repeatedly (see my variable ''current'').
//
// A bold assumption is that the pandigital number only needs to be split into two parts `left` and `right` such that
// `left = factor * one` and `right = factor * two`. If `concat(factor, one, two)` is 10-pandigital then we found the result.
//
// Neither `left` nor `right` can begin with a zero. Any potential `factor` must be a divisor of `left` and `right`: `1 < factor <= gcd(left, right)`
// When looking at all 10-pandigital numbers (there are only `10! = 3628800` such numbers), I observed that all `factor` are multiples of 3.
//
// # Note
// Chances are that splitting the pandigital number into more than two parts produces valid results, too.
// Maybe I just got lucky by finding the correct solution.
//
// # Hackerrank
// The program is forced to use a certain output format (no big problem).
// My program fails to process the massive amount of test cases.

#include <string>
#include <iostream>
#include <algorithm>

// true, if x contains only distinct digits
bool isPandigital(unsigned long long x)
{
  unsigned char used[10] = { 0 };
  while (x > 0)
  {
    auto digit = x % 10;
    // digit already used ?
    if (used[digit] == 1)
      return false;
    used[digit]++;
    x /= 10;
  }

  return true;
}

// greatest common divisor
unsigned int gcd(unsigned int a, unsigned int b)
{
  while (a != 0)
  {
    unsigned int c = a;
    a = b % a;
    b = c;
  }
  return b;
}

int main()
{
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    std::string current = "9876543210";
    std::cin >> current;

    // find next smaller pandigital number (only to avoid malicious input)
    unsigned long long adjusted = std::stoll(current);
    if (adjusted < 1023456789)
      adjusted = 1023456789;
    while (!isPandigital(adjusted))
      adjusted--;
    current = std::to_string(adjusted);

    // start search
    bool found = false;
    do
    {
      // split into two parts and check each common divisor
      for (size_t split = 1; split < current.size() && !found; split++)
      {
        // must not begin with a zero
        if (current[0] == '0' || current[split] == '0')
          continue;

        auto left  = std::stoll(current.substr(0, split));
        auto right = std::stoll(current.substr(split));

        // any common divisors ?
        unsigned int shared = gcd(left, right);
        const unsigned int MultipleOfThree = 3; // I saw that all divisors are always multiples of three
        for (unsigned int factor = MultipleOfThree; factor <= shared; factor += MultipleOfThree)
        {
          // analyze all common divisors
          if (left  % factor == 0 &&
              right % factor == 0)
          {
            // combine all digits
            unsigned int one = left  / factor;
            unsigned int two = right / factor;
            std::string sequence = std::to_string(factor) +
                                   std::to_string(one) +
                                   std::to_string(two);

            // must have exactly 10 pandigital digits
            if (sequence.size() == 10 && isPandigital(std::stoll(sequence)))
            {
              found = true;
              std::cout << factor << "*(" << one << "," << two << ")=" << current << std::endl;
              break;
            }
          }
        }
      }

      // done ?
      if (found)
        break;
    } while (std::prev_permutation(current.begin(), current.end()));
  }

  return 0;
}

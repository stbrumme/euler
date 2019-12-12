// ////////////////////////////////////////////////////////
// # Title
// A Modified Collatz sequence
//
// # URL
// https://projecteuler.net/problem=277
// http://euler.stephan-brumme.com/277/
//
// # Problem
// A modified Collatz sequence of integers is obtained from a starting value `a_1` in the following way:
//
// `a_{n+1} = a_n/3` if an is divisible by 3. We shall denote this as a large downward step, "D".
//
// `a_{n+1} = (4a_n + 2)/3` if an divided by 3 gives a remainder of 1. We shall denote this as an upward step, "U".
//
// `a_{n+1} = (2a_n - 1)/3` if an divided by 3 gives a remainder of 2. We shall denote this as a small downward step, "d".
//
// The sequence terminates when some `a_n = 1`.
//
// Given any integer, we can list out the sequence of steps.
// For instance if `a_1=231`, then the sequence `{a_n}={231,77,51,17,11,7,10,14,9,3,1}` corresponds to the steps "DdDddUUdDD".
//
// Of course, there are other sequences that begin with that same sequence "DdDddUUdDD....".
// For instance, if `a_1=1004064`, then the sequence is DdDddUUdDDDdUDUUUdDdUUDDDUdDD.
// In fact, 1004064 is the smallest possible `a_1 > 10^6` that begins with the sequence DdDddUUdDD.
//
// What is the smallest `a_1 > 10^15` that begins with the sequence "UDDDUdddDDUDDddDdDddDDUDDdUUDd"?
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// The function ''isGood'' returns true if its parameter ''x'' has a Collatz sequence that starts with the characters found in parameter ''sequence''.
//
// The loop in ''main'' finds the first number whose Collatz sequence starts with "U", beginning at `10^15` (and finds a match `10^15`).
// In the next iteration, all potential candidates must be `3^1=3` apart.
// `10^15` doesn't match the two-character prefex "UD", but `10^15+3` does.
// In the third iteration, all potential candidates must be `3^2=9` apart.
// Again, `10^15+3` fails to match "UDD" but `10^15+9` matches.
//
// # Alternative
// It's possible to solve this problem without a computer, too.

#include <iostream>
#include <string>

// return true if x matches the partial Collatz sequence
bool isGood(unsigned long long x, const std::string& sequence)
{
  for (auto s : sequence)
  {
    switch (x % 3)
    {
    case 0: // large downward step
      if (s != 'D')
        return false; // failed

      x /= 3;
      break;

    case 1: // upward step
      if (s != 'U')
        return false; // failed

      x = (4*x + 2) / 3;
      break;

    case 2: // small downward step
      if (s != 'd')
        return false; // failed

      x = (2*x - 1) / 3;
      break;

    default: ; // never reached
    }
  }

  return true;
}

int main()
{
  std::string sequence = "UDDDUdddDDUDDddDdDddDDUDDdUUDd";
  auto current = 1000000000000000ULL;
  std::cin >> current >> sequence;

  // initially search every number (3^0=1), then every third (3^1), every ninth (3^2), ...
  unsigned long long step = 1;

  // look for a match of the first characters of sequence
  for (size_t length = 1; length <= sequence.size(); length++)
  {
    // extract first characters
    auto partial = sequence.substr(0, length);

    // and find first match for those characters
    unsigned int iterations = 0;
    while (!isGood(current, partial))
    {
      current += step;
      if (++iterations > 100) // quick hack: prevent invalid input of live test
        return 1;
    }

    // increase step size for next iteration
    step *= 3;
  }

  // found it
  std::cout << current << std::endl;
  return 0;
}

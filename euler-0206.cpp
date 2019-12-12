// ////////////////////////////////////////////////////////
// # Title
// Concealed Square
//
// # URL
// https://projecteuler.net/problem=206
// http://euler.stephan-brumme.com/206/
//
// # Problem
// Find the unique positive integer whose square has the form 1_2_3_4_5_6_7_8_9_0, where each "_" is a single digit.
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// The square must be between 1020304050607080900 (if all "_" are zeros) and 1929394959697989990 (if all "_" are nines).
// That means I looking for a number `x` such that `sqrt{1020304050607080900} <= x <= sqrt{1929394959697989990}` which is
// `1010101010 <= x <= 1389026623`. I named those constants ''MinNumber'' and ''MaxNumber''.
//
// The square's last digit is zero and that's only possible if the last digit of `x` is zero, too.
// A simple loop iterates over all `x` in the aforementioned range which are multiples of 10 and calls ''match'' until it succeeds.
//
// ''match'' splits the ''square'' of ''x'' into single digits and compares them against a predefined array.
// Since it's done from right-to-left, those digits are in reverse order.
//
// # Notes
// My original code displayed the correct answer in about 250ms.
// When I saw that it is very close to the upper limit of the for-loop I simply reversed it.
// ==> now only 3ms ... and I feel like a cheater !
//
// # Alternatives
// You can probably solve this problem with pencil and paper:
// - I already said that the last digit of ''x'' must be zero
// - the next digit must be either 3 or 9 because only `3^2=9` or `7^2=49` end with a 9
// - and so on ...

#include <iostream>

// return true if root^2 matches 1_2_3_4_5_6_7_8_9_0
bool match(unsigned long long x)
{
  unsigned long long square = x * x;

  // required digits in reverse order
  const unsigned int Right2Left[] = { 0, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
  unsigned int index = 0;

  // check all digits
  do
  {
    // right-most digits matches current element of Right2Left ?
    auto digit = square % 10;
    if (digit != Right2Left[index++])
      return false;

    // remove the digit which passes the test and skip the next digit which is unknown, too
    square /= 100;
  } while (square > 0);

  // all tests passed !
  return true;
}

int main()
{
  // smallest possible number: gaps are zeros => sqrt(1020304050607080900)
  const unsigned int MinNumber = 1010101010;
  // largest  possible number: gaps are nines => sqrt(1929394959697989990)
  const unsigned int MaxNumber = 1389026620;

  //for (unsigned int x = MinNumber; x <= MaxNumber; x += 10)
  for (unsigned int x = MaxNumber; x >= MinNumber; x -= 10)
    if (match(x))
    {
      std::cout << x << std::endl;
      break;
    }

  return 0;
}

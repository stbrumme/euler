// ////////////////////////////////////////////////////////
// # Title
// Square digit chains
//
// # URL
// https://projecteuler.net/problem=92
// http://euler.stephan-brumme.com/92/
//
// # Problem
// A number chain is created by continuously adding the square of the digits in a number to form a new number until it has been seen before.
//
// For example,
//
// 44 ==> 32 ==> 13 ==> 10 ==> 1 ==> 1
// 85 ==> 89 ==> 145 ==> 42 ==> 20 ==> 4 ==> 16 ==> 37 ==> 58 ==> 89
//
// Therefore any chain that arrives at 1 or 89 will become stuck in an endless loop. What is most amazing is that EVERY starting number will eventually arrive at 1 or 89.
//
// How many starting numbers below ten million will arrive at 89?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// Let's start with the simple stuff:
// ''becomes89(x)'' checks whether repeated application of the "digit-square-adding" terminates with 89 (''true'') or 1 (''false'').
//
// The real magic is hidden in ''main'' - it's an iterative approach similar to Dynamic Programming and was inspired by https://rosettacode.org/wiki/Iterated_digits_squaring
//
// I count how many numbers ''sums[x]'' share the same squared digit sum ''x'':
// - initially ''sums[]'' contains a bunch of zeros
// - after that, all single digit numbers 0..9 are processed:
// ==> ''sums[0*0] = 1; sums[1*1] = 1; sums[2*2] = 1; sums[3*3] = 1; ...''
//
// Then all two-digit numbers 10..99 are processed:
// - they can have squared digit sums between 1 and 2*9*9
// - the highest digit will be between 1 and 9 (I call it ''high'')
// - a squared sum ''x'' should become the sum of all one-digit numbers with that squared digit sum and all two-digit number with that squared digit sum
// - the count of one-digit numbers is already stored in ''sum[x]'', it's either 0 or 1
// - the count of two-digit numbers is the count of ''sum[x - high*high]''
// ==> e.g. when ''high'' is 1, then ''sum[5] += sum[5 - 1*1]'' ==> ''1''
// ==> when ''high'' becomes 2, then ''sum[5] += sum[5 - 2*2]'' ==> ''2''
// ==> there are 2 one-digit or two-digit numbers with a squared digit sum of 5
//
// Please note that in-place updates of ''sum[x]'' are in descending order because otherwise we encounter collisions:
// ''sum[x - high*high]'' should not have been updated in the current pass. That means, ''x'' must decrease while ''high'' increases in their respective loops.
//
// That algorithm is repeated for all digits
//
// When I know how many numbers share the same squared digit sum, I can simply count all squared digit sums which lead to 89.
//
// # Note
// You have to enter the number of __digits__ of the upper limit. It's 7 for the original problem because `10^7=10000000`.
//
// # Hackerrank
// They want you to compute the result for values up to `10^{200}`. The final number must be modulo 1000000007, too (otherwise it gets huge).

#include <iostream>

// return true, if x will arrive at 89
bool becomes89(unsigned int x)
{
  do
  {
    // sum of squares of all digits
    unsigned int squareDigitSum = 0;
    auto reduce = x;
    while (reduce > 0)
    {
      auto digit = reduce % 10;
      reduce /= 10;
      squareDigitSum += digit * digit;
    }

    // terminated ?
    if (squareDigitSum == 89)
      return true;
    if (squareDigitSum ==  1)
      return false;

    // not done yet ... next iteration
    x = squareDigitSum;
  } while (true);
}

int main()
{
  unsigned int digits = 7; // 10^7 = 10,000,000
  std::cin >> digits;

  // Hackerrank's result will become quite big, they only want the final number modulo 1000000007
  // (doesn't affect the original problem)
  const unsigned int Modulo = 1000000007;

  // count how many numbers with the same digit sum exist
  // inspired by https://rosettacode.org/wiki/Iterated_digits_squaring

  // initialized with zeros
  unsigned int sums[200*9*9 + 1] = { 0 };

  // single-digit numbers
  for (unsigned int first = 0; first <= 9; first++)
    sums[first * first]++;
  // alternative approach: set sums[0] = 1; and start the new loop with length = 1

  // start with one digit and iteratively add digits
  for (unsigned int length = 2; length <= digits; length++)
    // go through all potential sums (including the most recently added digit)
    for (unsigned int sum = length*9*9; sum > 0; sum--)
      // what sum is it without that most recently added digit ?
      for (unsigned int high = 1; high <= 9; high++)
      {
        // square of the just added digit
        auto square = high * high;
        // this digit can't be part of the current digit sum because it's too big
        if (square > sum)
          break;

        // add count of all numbers without the new digit
        sums[sum] += sums[sum - square];
        // avoid overflows (Hackerrank only)
        sums[sum] %= Modulo;
      }

  // now we know how many numbers sums[x] exist with digit sum x
  // let's check which digit sums will be converted to 89
  unsigned int count89 = 0;
  // check all sums
  for (unsigned int i = 1; i <= digits*9*9; i++)
    if (becomes89(i))
    {
      count89 += sums[i]; // yes, all these numbers turn to 89
      count89 %= Modulo;  // Hackerrank only
    }

  std::cout << count89 << std::endl;
  return 0;
}

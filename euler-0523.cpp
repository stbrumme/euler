// ////////////////////////////////////////////////////////
// # Title
// First Sort I
//
// # URL
// https://projecteuler.net/problem=523
// http://euler.stephan-brumme.com/523/
//
// # Problem
// Consider the following algorithm for sorting a list:
//
// 1. Starting from the beginning of the list, check each pair of adjacent elements in turn.
// 2. If the elements are out of order:
// 2a. Move the smallest element of the pair at the beginning of the list.
// 2b. Restart the process from step 1.
// 3. If all pairs are in order, stop.
//
// For example, the list ''{ 4 1 3 2 }'' is sorted as follows:
//
// ''__4 1__ 3 2'' (4 and 1 are out of order so move 1 to the front of the list)
// ''1 __4 3__ 2'' (4 and 3 are out of order so move 3 to the front of the list)
// ''__3 1__ 4 2'' (3 and 1 are out of order so move 1 to the front of the list)
// ''1 3 __4 2__'' (4 and 2 are out of order so move 2 to the front of the list)
// ''__2 1__ 3 4'' (2 and 1 are out of order so move 1 to the front of the list)
// ''1 2 3 4'' (The list is now sorted)
//
// Let `F(L)` be the number of times step 2a is executed to sort list `L`. For example, `F({ 4 \space 1 \space 3 \space 2 }) = 5`.
//
// Let `E(n)` be the expected value of `F(P)` over all permutations `P` of the integers `{1, 2, ..., n}`.
// You are given `E(4) = 3.25` and `E(10) = 115.725`.
//
// Find `E(30)`. Give your answer rounded to two digits after the decimal point.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// I solved this problem in a very empiric way:
// - write code to solve the problem for small input values (see ''evaluate'')
// - stare at the output far too long
// - finally see some inner structures after two days
//
// The function ''evaluate'' can solve `E(10)` in a few seconds and will produce this output:
//
// ||  3 ||   4    ||     8     ||       8       ||
// ||! x ++  E(x)  ++   #moves  ++ #permutations ||
// ||  1 ++   0    ++         0 ++       1       ||
// ||  2 ++  0.5   ++         1 ++       2       ||
// ||  3 ++  1.5   ++         9 ++       6       ||
// ||  4 ++  3.25  ++        78 ++      24       ||
// ||  5 ++  6.25  ++       750 ++     120       ||
// ||  6 ++ 11.42  ++      8220 ++     720       ||
// ||  7 ++ 20.42  ++    102900 ++    5040       ||
// ||  8 ++ 36.29  ++   1463280 ++   40320       ||
// ||  9 ++ 64.62  ++  23451120 ++  362880       ||
// || 10 ++ 115.73 ++ 419942880 ++ 3628800       ||
//
// These numbers didn't reveal anything at all - but then I computed the difference `E(x) - E(x-1)` (as fractions):
//
// ||  3 ||        8         ||     8           ||
// ||! x ++  `E(x) - E(x-1)` ++ denominator = x ||
// ||  2 ++       1/2        ++     1/2         ||
// ||  3 ++       1          ++     3/3         ||
// ||  4 ++       7/4        ++     7/4         ||
// ||  5 ++       3          ++    15/5         ||
// ||  6 ++      31/6        ++    31/6         ||
// ||  7 ++       9          ++    63/7         ||
// ||  8 ++     127/8        ++   127/8         ||
// ||  9 ++      85/3        ++   255/9         ||
// || 10 ++     511/10       ++   511/10        ||
//
// The second columns already shows a pattern for `E(x) - E(x - 1)` when `x` is even:
// the numerator is always one less than a power of two and the denominator is always `x`.
//
// When enforcing the denominator to be `x` (third column) then the same pattern appears for odd `x` as well and I finally have a formula:
// `E(x) - E(x - 1) = dfrac{2^{x-1} - 1}{x}`
//
// All I need is a simple ''for''-loop iterating over all `x <= 30`.
//
// # Note
// I somehow prefer the way ''evaluate()'' finds the solutions: play around with data structures, STL algorithms, ... ==> let the computer do some hard work.
// The formula used to determine `E(30)` is pretty simple (though a bit tricky to find) and could have been solved with a pencil and paper only.
// That's not why I studied computer science and work as a software engineer ...

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

// execute all moves on all permutations of a given size
// ==> only needed to get an idea what the formula looks like, not needed anymore
double evaluate(unsigned int size)
{
  // data = { 1,2,3,...,size }
  std::vector<unsigned int> data, current;
  for (unsigned int i = 1; i <= size; i++)
    data.push_back(i);

  // count moves and permutations
  unsigned int moves = 0;
  unsigned int permutations = 0;
  do
  {
    // get current permutation
    current = data;
    permutations++;

    // sort container
    unsigned int pos = 1;
    // compare data[pos - 1] and data[pos]
    while (pos < size)
    {
      // pair in wrong order ?
      if (current[pos] < current[pos - 1])
      {
        // rotate all elements from 0 to pos to the right once (last element becomes the first)
        std::rotate(current.begin(), current.begin() + pos, current.begin() + pos + 1);
        moves++;
        // restart
        pos = 1;
      }
      else
        pos++;
    }
  } while (std::next_permutation(data.begin(), data.end()));

  std::cout << "E(" << size << ")=" << moves / double(permutations) // result
            << " =" << moves << "/" << permutations << std::endl;   // and as a fraction
  return moves / double(permutations);
}

int main()
{
  unsigned int limit = 10;
  std::cin >> limit;

  std::cout << std::fixed << std::setprecision(2);

  // solve for small input values
  //for (unsigned int i = 1; i <= limit; i++)
  //  evaluate(i);

  double result = 0;
  for (unsigned int i = 1; i <= limit; i++)
  {
    // 2^(i-1) - 1
    auto numerator = (1 << (i - 1)) - 1; // 0,1,3,7,15,31,...
    // add to result
    result += numerator / double(i);
  }
  std::cout << result << std::endl;
  return 0;
}

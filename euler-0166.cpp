// ////////////////////////////////////////////////////////
// # Title
// Criss Cross
//
// # URL
// https://projecteuler.net/problem=166
// http://euler.stephan-brumme.com/166/
//
// # Problem
// A 4x4 grid is filled with digits `d`, `0 <= d <= 9`.
//
// It can be seen that in the grid
//
// || 1 || 1 || 1 || 1 ||
// || 6 ++ 3 ++ 3 ++ 0 ||
// || 5 ++ 0 ++ 4 ++ 3 ||
// || 0 ++ 7 ++ 1 ++ 4 ||
// || 1 ++ 2 ++ 4 ++ 5 ||
//
// the sum of each row and each column has the value 12. Moreover the sum of each diagonal is also 12.
//
// In how many ways can you fill a 4x4 grid with the digits `d`, `0 <= d <= 9` so that each row, each column, and both diagonals have the same sum?
//
// # Solved by
// Stephan Brumme
// June 2017
//
// # Algorithm
// I brute-force the problem with a few performance tweaks.
// Let's assign a variable to each cell of the grid:
//
// || 1 || 1 || 1 || 1 ||
// ||`a`++`b`++`c`++`d`||
// ||`e`++`f`++`g`++`h`||
// ||`i`++`j`++`k`++`l`||
// ||`m`++`n`++`o`++`p`||
//
// The first step is to find the current sum `s` (called ''sum'' in my code):
// `s = a + b + c + d`
//
// The "fourth" variable of each row/column/diagonal depends on the three previous and `s`:
// `h = s - e - f - g` (second row)
// `l = s - i - j - k` (third  row)
// `p = s - m - n - o` (fourth row)
// `m = s - a - e - i` (first  column)
// `n = s - b - f - j` (second column)
// `o = s - c - g - k` (third  column)
// Now only 10 instead of 16 variables are unknown.
//
// Actually, `j` isn't unknown - it can be computed, too:
// `j = s - d - g - m` (anti-diagonal)
//
// My program iterates over all possible values of the remaining 9 variables `a`, `b`, `c`, `d`, `e`, `f`, `g`, `i` and `k`.
// Any combination where all variables are between ''0'' and ''maxDigit'' (which is 9 for the original problem) is valid - with one exception:
// I have to check the diagonal, too (`s = a + f + k + p`), but I saw in my experiments that the fourth column doesn't need to be checked as well.
//
// These are my performance tweaks:
// 1. If there is an even number of digits (''maxDigit = 9'' ==> ''0..9'' ==> 10 digits), then `a` can be restricted to ''0..(maxDigit - 1)/2'')
//    For every solution found there is another "inverted" solution where each variable `x' = maxDigit - x`.
//    ==> about twice as fast
// 2. The grid can be mirrored along the diagonal such that `b <= e`. For every `b < e` there is a second solution found by mirroring along the diagonal.
//    ==> about twice as fast
// 3. All variables are unsigned: if a variable become negative it will be a huge positive number in C's two-complement representation.
//    The check ''if (x < 0 || x > maxDigit)'' simplifies to ''if (x > maxDigit)'' which is about 10% faster.
// These three optimizations make my program about four times faster.
//
// # Alternative
// Nayuki's solution is based on `f = b + c + 2d - e - i - k`. Look at his code for an explanation - it makes the program about five times faster.
// I didn't spend much time on a thorough mathematical analysis of the problem and didn't see that relationship.
//
// # Hackerrank
// The maximum digit can be changed via STDIN. The original problem has ''maxDigit = 9''.

#include <iostream>

int main()
{
  // maximum digit
  unsigned int maxDigit = 9;
  std::cin >> maxDigit;

  // restrict variable "a"
  bool even = (maxDigit + 1) % 2 == 0;
  auto maxA = even ? (maxDigit - 1) / 2 : maxDigit;

  unsigned int result   = 0;
  for (unsigned int a = 0; a <= maxA; a++) // limit a to 0..4 => for every solution found there is a "inverted" version where x = 9 - x and x = a,b,c,d,...,p
    for (unsigned int b = 0; b <= maxDigit; b++)
      for (unsigned int c = 0; c <= maxDigit; c++)
        for (unsigned int d = 0; d <= maxDigit; d++)
        {
          // all rows, columns, diagonals must share the same sum
          auto sum = a + b + c + d;

          for (unsigned int e = b; e <= maxDigit; e++) // !!! start at b instead of zero
            for (unsigned int f = 0; f <= maxDigit; f++)
              for (unsigned int g = 0; g <= maxDigit; g++)
              {
                // sum of second row must be identical to first row
                auto h = sum - e - f - g;
                if (h > maxDigit)
                  continue;

                for (unsigned int i = 0; i <= maxDigit; i++)
                {
                  // sum of first column must be identical to first row
                  auto m = sum - a - e - i;
                  if (m > maxDigit)
                    continue;

                  // sum of anti-diagonal must be identical to first row
                  auto j = sum - d - g - m;
                  if (j > maxDigit)
                    continue;

                  // sum of second column must be identical to first row
                  auto n = sum - b - f - j;
                  if (n > maxDigit)
                    continue;

                  for (unsigned int k = 0; k <= maxDigit; k++)
                  {
                    // sum of third column must be identical to first row
                    auto o = sum - c - g - k;
                    if (o > maxDigit)
                      continue;

                    // sum of third row must be identical to first row
                    auto l = sum - i - j - k;
                    if (l > maxDigit)
                      continue;

                    // sum of fourth row must be identical to first row
                    auto p = sum - m - n - o;
                    if (p > maxDigit)
                      continue;

                    // check diagonal, too
                    if (sum != a + f + k + p)
                      continue;

                    // yes, found a solution
                    result++;
                    // mirror grid along diagonal, too
                    if (b < e)
                      result++;
                  }
                }
              }
        }

  // add "inverted" solutions
  if (even)
    result *= 2;

  std::cout << result << std::endl;
  return 0;
}

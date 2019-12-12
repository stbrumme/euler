// ////////////////////////////////////////////////////////
// # Title
// Matrix Sum
//
// # URL
// https://projecteuler.net/problem=345
// http://euler.stephan-brumme.com/345/
//
// # Problem
// We define the Matrix Sum of a matrix as the maximum sum of matrix elements with each element being the only one in his row and column. For example, the Matrix Sum of the matrix below equals 3315 ( = 863 + 383 + 343 + 959 + 767):
//
// ''  7  53 183 439 __863__''
// ''497 __383__ 563  79 973''
// ''287  63 __343__ 169 583''
// ''627 343 773 __959__ 943''
// ''__767__ 473 103 699 303''
//
// Find the Matrix Sum of:
//
// ''  7  53 183 439 863 497 383 563  79 973 287  63 343 169 583''
// ''627 343 773 959 943 767 473 103 699 303 957 703 583 639 913''
// ''447 283 463  29  23 487 463 993 119 883 327 493 423 159 743''
// ''217 623   3 399 853 407 103 983  89 463 290 516 212 462 350''
// ''960 376 682 962 300 780 486 502 912 800 250 346 172 812 350''
// ''870 456 192 162 593 473 915  45 989 873 823 965 425 329 803''
// ''973 965 905 919 133 673 665 235 509 613 673 815 165 992 326''
// ''322 148 972 962 286 255 941 541 265 323 925 281 601  95 973''
// ''445 721  11 525 473  65 511 164 138 672  18 428 154 448 848''
// ''414 456 310 312 798 104 566 520 302 248 694 976 430 392 198''
// ''184 829 373 181 631 101 969 613 840 740 778 458 284 760 390''
// ''821 461 843 513  17 901 711 993 293 157 274  94 192 156 574''
// '' 34 124   4 878 450 476 712 914 838 669 875 299 823 329 699''
// ''815 559 813 459 522 788 168 586 966 232 308 833 251 631 107''
// ''813 883 451 509 615  77 281 613 459 205 380 274 302  35 805''
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// I wrote a simple Dynamic Programming solution:
// to avoid processing all 15! combinations (about `1.3 * 10^12`) it keeps track of the best solution so far and aborts a search if the current
// path in the search tree can't exceed that solution.
// Thus only 2100 path are fully evaluated (make it to ''if (row == Size)'') and the correct solution is printed in about 0.02 seconds.
//
// # Note
// Pretty sure you can create a "bad" matrix where my algorithm needs ages to find the correct solution.
// Even flipping the matrix along the main diagonal (''swap(matrix[row][column], matrix[column][row])'') make it about 70% slower.
//
// # Alternative
// I learnt that a so-called Hungarian algorithm (see https://en.wikipedia.org/wiki/Hungarian_algorithm ) can solve this problem even faster.

#include <iostream>
#include <vector>

// 15x15 grid
const unsigned int Size = 15;
// matrix from problem statement
unsigned short matrix[Size][Size] =
  {
    {   7,  53, 183, 439, 863, 497, 383, 563,  79, 973, 287,  63, 343, 169, 583 },
    { 627, 343, 773, 959, 943, 767, 473, 103, 699, 303, 957, 703, 583, 639, 913 },
    { 447, 283, 463,  29,  23, 487, 463, 993, 119, 883, 327, 493, 423, 159, 743 },
    { 217, 623,   3, 399, 853, 407, 103, 983,  89, 463, 290, 516, 212, 462, 350 },
    { 960, 376, 682, 962, 300, 780, 486, 502, 912, 800, 250, 346, 172, 812, 350 },
    { 870, 456, 192, 162, 593, 473, 915,  45, 989, 873, 823, 965, 425, 329, 803 },
    { 973, 965, 905, 919, 133, 673, 665, 235, 509, 613, 673, 815, 165, 992, 326 },
    { 322, 148, 972, 962, 286, 255, 941, 541, 265, 323, 925, 281, 601,  95, 973 },
    { 445, 721,  11, 525, 473,  65, 511, 164, 138, 672,  18, 428, 154, 448, 848 },
    { 414, 456, 310, 312, 798, 104, 566, 520, 302, 248, 694, 976, 430, 392, 198 },
    { 184, 829, 373, 181, 631, 101, 969, 613, 840, 740, 778, 458, 284, 760, 390 },
    { 821, 461, 843, 513,  17, 901, 711, 993, 293, 157, 274,  94, 192, 156, 574 },
    {  34, 124,   4, 878, 450, 476, 712, 914, 838, 669, 875, 299, 823, 329, 699 },
    { 815, 559, 813, 459, 522, 788, 168, 586, 966, 232, 308, 833, 251, 631, 107 },
    { 813, 883, 451, 509, 615,  77, 281, 613, 459, 205, 380, 274, 302,  35, 805 }
  };

// sum of the highest value of the current row and all further rows (sum[current..15])
unsigned int maxRemaining[Size];

// try to find the highest value for "optimum"
// row        - current row, start at top-most row 0
// columnMask - bitmask of all used columns (1 - used, 0 - available)
// sum        - current sum
// atLeast    - best solution so far
unsigned int search(unsigned int row = 0, unsigned int columnMask = 0, unsigned int sum = 0, unsigned int atLeast = 0)
{
  // done ?
  if (row == Size)
    return sum;

  // even if choosing the highest value of each of the next rows:
  // is it possible hat this combination exceeds the previously highest sum ?
  if (sum + maxRemaining[row] <= atLeast)
    return 0;

  // look at all values of the current row
  for (unsigned int column = 0; column < Size; column++)
  {
    // column already used ?
    auto mask = 1 << column;
    if ((columnMask & mask) != 0)
      continue;

    // "occupy" column and continue with next row
    auto current = search(row + 1, columnMask | mask, sum + matrix[row][column], atLeast);
    if (atLeast < current)
      atLeast = current;
  }

  return atLeast;
}

int main()
{
  // find highest element of each row
  unsigned int maxValuePerRow[Size];
  for (unsigned int row = 0; row < Size; row++)
  {
    maxValuePerRow[row] = matrix[0][row];
    for (unsigned int column = 1; column < Size; column++)
      if (maxValuePerRow[row] < matrix[column][row])
        maxValuePerRow[row] = matrix[column][row];
  }
  // compute the maximum sum of the last rows, ignoring collisions (invalid choices)
  maxRemaining[Size - 1] = maxValuePerRow[Size - 1];
  for (auto row = Size - 1; row > 0; row--)
    maxRemaining[row - 1] = maxRemaining[row] + maxValuePerRow[row - 1];

  // let's go !
  std::cout << search() << std::endl;
  return 0;
}

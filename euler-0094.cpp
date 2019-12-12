// ////////////////////////////////////////////////////////
// # Title
// Almost equilateral triangles
//
// # URL
// https://projecteuler.net/problem=94
// http://euler.stephan-brumme.com/94/
//
// # Problem
// It is easily proved that no equilateral triangle exists with integral length sides and integral area.
// However, the almost equilateral triangle 5-5-6 has an area of 12 square units.
//
// We shall define an almost equilateral triangle to be a triangle for which two sides are equal and the third differs by no more than one unit.
//
// Find the sum of the perimeters of all almost equilateral triangles with integral side lengths and area and whose perimeters do not exceed one billion (1,000,000,000).
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// I came up with two solutions:
// 1. a brute-force solution  based on geometry which needs about 5 seconds to solve the problem (see ''findMore'')
// 2. a super-simple sequence based on "is there a pattern in the numbers of my brute-force approach ?" (see ''sequence'')
//
// Let's start with algorithm 1:
// The height in such a triangle with two equal sides is
// `h = sqrt{twoSides^2 - frac{oneSide^2}{4}}`
// Thus its area can be computed as
//  `A = (oneSide/2) * h`
//    `= (oneSide/2) * sqrt{  twoSides^2 - frac{oneSide^2}{4}}`
//    `= (oneSide/2) * sqrt{4 twoSides^2 -  oneSide^2}/2`
// `2A =  oneSide    * sqrt{4 twoSides^2 -  oneSide^2}`
//
// `oneSide` is always integral and `2A` will be integral if the square root is integral, too,
// that means that `4 * twoSides^2 - oneSide^2` must be a perfect square.
// That's what my function ''isValidTriangle'' is for.
//
// ''findMore'' looks for triangles with a perimeter between ''perimeter'' and ''limit''.
// It checks every possible triangle which makes it pretty slow.
//
// I had a prototype that printed the lengths of all sides and their perimeter:
//
// ||    7    ||    7    ||    7    ||      7       ||     7     ||
// ||    a    ||    b    ||    c    || c - b (or a) || perimeter ||
// ||       5 ||       5 ||       6 ||     +1       ||        16 ||
// ||      17 ||      17 ||      16 ||     -1       ||        50 ||
// ||      65 ||      65 ||      66 ||     +1       ||       196 ||
// ||     241 ||     241 ||     240 ||     -1       ||       722 ||
// ||     901 ||     901 ||     902 ||     +1       ||      2704 ||
// ||    3361 ||    3361 ||    3360 ||     -1       ||     10082 ||
// ||   12545 ||   12545 ||   12546 ||     +1       ||     37636 ||
// ||   46817 ||   46817 ||   46816 ||     -1       ||    140450 ||
// ||  174725 ||  174725 ||  174724 ||     +1       ||    524176 ||
// ||  ...    ||  ...    ||  ...    ||    ...       ||    ...    ||
//
// Obviously there is an alternating pattern in the difference between the length of the single side c and the other two sides a or b.
// In the most unscientific way - plotting the numbers in Excel - I found that
// `a_n = 14a_{n-1} - a_{n-2} - 4` for all triangles where the single side is 1 unit shorter
// `a_n = 14a_{n-1} - a_{n-2} + 4` for all triangles where the single side is 1 unit longer
//
// That's by no means something I can proof (and I don't intend to) but it gives the correct answers pretty much instantly. I can live with that ...
//
// # Hackerrank
// The second approach solves all problems in less than 10 milliseconds, even for perimeters of `10^18` while
// the brute-force approach fails for 2 out 7 test cases.

#include <cmath>
#include <vector>
#include <iostream>

// valid perimeters
std::vector<unsigned long long> solutions;

// return true if area is integral
bool isValidTriangle(unsigned long long oneSide, unsigned long long twoSides)
{
  unsigned long long check = 4 * twoSides * twoSides - oneSide * oneSide;
  unsigned long long root  = sqrt(check);
  return root * root == check;
}

// brute-force approach
unsigned long long findMore(unsigned long long perimeter, unsigned long long limit)
{
  // check all perimeters
  while (perimeter <= limit + 3)
  {
    // length of the two equal sides
    auto twoSides = perimeter / 3;

    // assume single side is one unit smaller than the other two sides
    auto oneSide = twoSides - 1;
    if (isValidTriangle(oneSide, twoSides))
      solutions.push_back(perimeter - 1);

    // assume single side is one unit bigger than the other two sides
    oneSide = twoSides + 1;
    if (isValidTriangle(oneSide, twoSides))
      solutions.push_back(perimeter + 1);

    // next group of triangles
    perimeter += 3;
  }

  return perimeter;
}

// just compute sequence
unsigned long long sequence(unsigned long long limit)
{
  // initial values of the equal sides
  unsigned long long plusOne [] = { 1,  5 };
  unsigned long long minusOne[] = { 1, 17 };

  solutions.clear();
  // smallest solutions where:
  solutions.push_back(3 * plusOne [1] + 1); // single side is 1 unit longer  than the equal sides
  solutions.push_back(3 * minusOne[1] - 1); // single side is 1 unit shorter than the equal sides


  while (solutions.back() <= limit + 3)
  {
    // compute next length of equal sides
    unsigned long long nextPlusOne  = 14 * plusOne [1] - plusOne [0] - 4;
    unsigned long long nextMinusOne = 14 * minusOne[1] - minusOne[0] + 4;

    // store it, shift off oldest values
    plusOne [0] = plusOne [1];
    plusOne [1] = nextPlusOne;
    minusOne[0] = minusOne[1];
    minusOne[1] = nextMinusOne;

    // we are interested in the perimeter
    solutions.push_back(3 * nextPlusOne  + 1);
    solutions.push_back(3 * nextMinusOne - 1);
  }

  // largest perimeter found
  return solutions.back();
}


int main()
{
  solutions.push_back(16); // perimeter of smallest triangle
  unsigned long long perimeter = 18; // check 18-1 and 18+1 in next step

  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned long long limit = 1000000000;
    std::cin >> limit;

    // check all perimeters
    while (perimeter <= limit + 3)
      //perimeter = findMore(perimeter, limit);
      perimeter = sequence(limit);

    // sum of all relevant triangles
    unsigned long long sum = 0;
    for (auto x : solutions)
      if (x <= limit)
        sum += x;

    std::cout << sum << std::endl;
  }

  return 0;
}

// ////////////////////////////////////////////////////////
// # Title
// Rectangles in cross-hatched grids
//
// # URL
// https://projecteuler.net/problem=147
// http://euler.stephan-brumme.com/147/
//
// # Problem
// In a 3x2 cross-hatched grid, a total of 37 different rectangles could be situated within that grid as indicated in the sketch.
//
// ![grid](p147.gif)
//
// There are 5 grids smaller than 3x2, vertical and horizontal dimensions being important, i.e. 1x1, 2x1, 3x1, 1x2 and 2x2.
// If each of them is cross-hatched, the following number of different rectangles could be situated within those smaller grids:
//
// 1x1: 1
// 2x1: 4
// 3x1: 8
// 1x2: 4
// 2x2: 18
//
// Adding those to the 37 of the 3x2 grid, a total of 72 different rectangles could be situated within 3x2 and smaller grids.
//
// How many different rectangles could be situated within 47x43 and smaller grids?
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// The number of upright rectangles can be computed using the formulas found in problem 85:
// `n = T(width) * T(height)` where `T` is the triangle number `T(x) = dfrac{x(x+1)}{2}`
//
// A 3x2 grid contains `dfrac{3(3+1)}{2} * dfrac{2(2+1)}{2} = dfrac{12}{2} * dfrac{6}{2} = 18` upright rectangles.
//
// I didn't find such a nice formula for the diagonal rectangles. Fortunately brute-force counting can solve this problem in less than a second.
// There are two kinds of diagonal squares:
// - some have a center `(x,y)` that is crosses by a vertical line ==> `x` is an integer and `y` is an integer plus 0.5
// - some have a center `(x,y)` that is crosses by a horizontal line ==> `y` is an integer and `x` is an integer plus 0.5
//
// If I look at a square then it has a neighbor to the upper-right whose center is at `(x + 0.5, y + 0.5)`.
// Its lower-right neighbor's center is at `(x + 0.5, y - 0.5)`.
// All centers have the properties `0 < x < width` and `0 < y < height`.
// Those floating-point computations tend to be a bit slow therefore I multiplied all values by 2 and returned to integer arithmetic.
//
// I can count all diagonal rectangles if I start at every diagonal square and find all its upper-right neighbors. This gives me all rectangles that are one unit high.
// Then I go one step to the lower-right and again find all upper-right neighbors. This gives me all rectangles that are two units high.
// This algorithm is repeated until there is no valid lower-right neighbor.
//
// The variable ''parity''determines whether the first squares center is crossed by a vertical or horizontal line.
//
// # Hackerrank
// Several grids have to be analyzed at once. Since all have to process smaller grids like (1,1), (1,2), (2,1), etc. I added a simple cache.
// I fail two test cases because they apparantly require you to find the closed form - my simple brute-force strategy times out.
//
// # Note
// Swapping the values for ''width'' and ''height'' doesn't affect the result. My algorithm is slightly faster when ''width > height''.

#include <iostream>
#include <map>

#define ORIGINAL

// Hackerrank only
const unsigned int Modulo = 1000000007;

// return number of different rectangles in a x*y grid (without diagonal rectangles)
unsigned int grid(unsigned int width, unsigned int height)
{
  // triangular number, see https://en.wikipedia.org/wiki/Triangular_number
  // return T(width) * T(height)
  return (width * (width + 1) / 2) * (height * (height + 1) / 2);
}

// return number of different diagonal rectangles in a x*y grid
unsigned int diagonal(unsigned int width, unsigned int height)
{
  // memoize
  static std::map<std::pair<unsigned int, unsigned int>, unsigned int> cache;
  auto id = std::make_pair(width, height);
  auto lookup = cache.find(id);
  if (lookup != cache.end())
    return lookup->second;

  // a is the length of the longer side and b the shorter side
  auto a = width;
  auto b = height;
  if (a < b)
    std::swap(a, b);

  // no clever formulas, just stupid counting ...
  unsigned int count = 0;

  // for each lattice point (x,y) I can find two nearby square centers:
  // at (x + 0.5, y) and at (x, y + 0.5)
  // floating-point arithmetic was a bit slow therefore I switched to integer arithmetic:
  // all coordinates are multiplied by 2
  for (unsigned int i = 0; i < a; i++)
    for (unsigned int j = 0; j < b; j++)
      for (unsigned int parity = 0; parity <= 1; parity++)
      {
        // left-most corner (if rotated by 45 degrees it would be the lower-left corner)
        auto startX = 2*i + 1 + parity;
        auto startY = 2*j + 2 - parity;
        // now try to find all possible upper-right corners of that triangle

        // found all rectangles started at (startX, startY) ?
        bool stop = false;
        // limit height of each rectangle
        auto maxHeight = 999999999;

        // scan along the (rotated) x-axis
        for (auto currentWidth = 0; !stop; currentWidth++)
        {
          // generate coordinates in a non-rotated coordinate system (still multiplied by 2)
          auto currentX = startX + currentWidth;
          auto currentY = startY - currentWidth;
          // out of bounds ?
          if (currentY <= 0)
            break;

          // scan along the (rotated) y-axis
          // note: not higher than any rectangle before (with the same starting point)
          for (auto currentHeight = 0; currentHeight < maxHeight; currentHeight++)
          {
            // upper-right corner of the current rectangle
            auto endX = currentX + currentHeight;
            auto endY = currentY + currentHeight;
            if (endX >= 2*a || endY >= 2*b)
            {
              // no other rectangle may be higher
              if (maxHeight > currentHeight)
                maxHeight = currentHeight;

              // reached right-most part ? => done
              stop = (currentHeight == 0);
              break;
            }

            // found one more rectangle
            count++;
          }
        }
      }

  // Hackerrank only
#ifndef ORIGINAL
  count %= Modulo;
#endif

  cache[id] = count;
  return count;
}

int main()
{
  unsigned int tests = 1;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int maxWidth  = 47;
    unsigned int maxHeight = 43;
    std::cin >> maxWidth >> maxHeight;

    // count all upright and all diagonal rectangles
    unsigned long long sumUpright  = 0;
    unsigned long long sumDiagonal = 0;
    for (unsigned int width = 1; width <= maxWidth; width++)
      for (unsigned int height = 1; height <= maxHeight; height++)
      {
        sumUpright  += grid    (width, height);
        sumDiagonal += diagonal(width, height);
      }

    // display result
#ifdef ORIGINAL
    std::cout << sumUpright + sumDiagonal << std::endl;
#else
    std::cout << sumUpright % Modulo << " " << sumDiagonal % Modulo << std::endl;
#endif
  }

  return 0;
}

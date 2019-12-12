// ////////////////////////////////////////////////////////
// # Title
// Square on the Inside
//
// # URL
// https://projecteuler.net/problem=504
// http://euler.stephan-brumme.com/504/
//
// # Problem
// Let `ABCD` be a quadrilateral whose vertices are lattice points lying on the coordinate axes as follows:
//
// `A(a, 0)`, `B(0, b)`, `C(-c, 0)`, `D(0, -d)`, where `1 <= a, b, c, d <= m` and `a`, `b`, `c`, `d`, `m` are integers.
//
// It can be shown that for `m = 4` there are exactly 256 valid ways to construct `ABCD`.
// Of these 256 quadrilaterals, 42 of them strictly contain a square number of lattice points.
//
// How many quadrilaterals `ABCD` strictly contain a square number of lattice points for `m = 100`?
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// Pick's theorem states `A = i + b/2 - 1` (see https://en.wikipedia.org/wiki/Pick%27s_theorem ):
// if a simple polygon has `i` interior integer points and `b` boundary integer points, then `A` must be a perfect square to match the conditions of the problem.
// So all I have to do is finding `i` and `b` - that's what my function ''countLatticePoints'' is for.
//
// The bounding box that enclosed the quadrilateral and is parallel to the x- and y-axis has an area of
// `ab + bc + cd + ad = a(b + d) + c(b + d) = (a + c)(b + d)`
// The four triangles cover only half the area of the bounding box. My variable ''inside'' equals `i` of the formula:
// `i = dfrac{1}{2} (a + c)(b + d)`
//
// The boundary integer points can be subdivided into two groups:
// - 4 corners `A(a, 0)`, `B(0, b)`, `C(-c, 0)`, `D(0, -d)`
// - the edges between `A` and `B`, `B` and `C`, `C` and `D`, `D` and `A`
// An edge between between two points `X(x, 0)` and `Y(0, y)` covers the same number of integer lattice points as an edge between `X_2(0,0)` and `Y_(x,y)`.
// And there are `gcd(x,y) + 1` such integer lattice points: the origin plus all multiples of `k * ( dfrac{x}{gcd(x,y)}, dfrac{y}{gcd(x,y)} )`.
// However, each corner appears twice: `A` in `AB` and `DA`, `B` in `AB` and `BC`, ...
// Using just `gcd(x,y)` (without minus one) solves this problem.
//
// Unfortunately, my results of ''countLatticePoints'' were off by 4:
// the corners `A`, `B`, `C` and `D` are not part of the original bounding-box and must be subtracted, too.
//
// # Note
// I set up a few caches to speed up the computations considerably:
// - the same GCDs have to by computed quite many times, therefore I store them in ''pointsOnEdge''
// - ''sqrt'' is pretty slow and a simple container ''isSquare'' stores a ''true'' flag for each square up to the largest area of an quadrilateral (100,100,100,100) ==> about 20000.
//
// # Alternative
// There are many symmetries. For example I could assume `a <= b` and reduce the search space accordingly.

#include <iostream>
#include <vector>

// cache gcd(a,b)
std::vector<std::vector<unsigned int>> pointsOnEdge;

// greatest common divisor
template <typename T>
T gcd(T a, T b)
{
  while (a != 0)
  {
    T c = a;
    a = b % a;
    b = c;
  }
  return b;
}

// count lattice points with Pick's theorem
// A(a, 0), B(b, 0), C(-c, 0), D(-d, 0)
unsigned int countLatticePoints(unsigned int a, unsigned int b, unsigned int c, unsigned int d)
{
  // Pick's theorem: A = i + b/2 - 1
  // where i is the area inside the quadrilateral
  // and b is the number of lattice points on the boundary
  // https://en.wikipedia.org/wiki/Pick%27s_theorem

  // total area of the bounding box
  auto rectangle = (a + c) * (b + d);
  // only half of the area belongs to the quadrilateral
  auto inside = rectangle / 2;

  // points on the edges of the quadrilateral
  auto boundary = pointsOnEdge[a][b] + pointsOnEdge[b][c] + pointsOnEdge[c][d] + pointsOnEdge[a][d];
  // the four corners are not inside the bounding box (they are on its edges)
  boundary -= 4;

  // Pick's formula
  return inside - (boundary / 2) - 1;
}

int main()
{
  unsigned int limit = 100;
  std::cin >> limit;

  // precompute number of lattice points lying exactly on the sides/edges
  pointsOnEdge.resize(limit + 1);
  for (auto& x : pointsOnEdge)
    x.resize(limit + 1);
  for (unsigned int a = 1; a <= limit; a++)
    for (unsigned int b = 1; b <= limit; b++)
      // note: only one endpoint is included for each edge
      //       e.g. a=4 b=2 gcd(4,2)=2
      //       => has actually 3 lattice points:
      //          both endpoints plus point (3,1)
      //       but each endpoint is part of two edges therefore it would be counted twice
      //       that means that keeping only one endpoint is okay and much simpler
      pointsOnEdge[a][b] = pointsOnEdge[b][a] = gcd(a, b);

  // precompute square numbers
  // note: std::vector<bool> would be sufficient but std::vector<char> is about 20% faster
  std::vector<unsigned char> isSquare(countLatticePoints(limit, limit, limit, limit) + 1, false);
  for (size_t i = 1; i*i < isSquare.size(); i++)
    isSquare[i*i] = true;

  // plain enumeration of all combinations, 100^4 = 10^8 iterations
  auto count = 0;
  for (unsigned int a = 1; a <= limit; a++)
    for (unsigned int b = 1; b <= limit; b++)
      for (unsigned int c = 1; c <= limit; c++)
        for (unsigned int d = 1; d <= limit; d++)
          // count only perfect squares
          if (isSquare[countLatticePoints(a, b, c, d)])
            count++;

  // display result
  std::cout << count << std::endl;
  return 0;
}

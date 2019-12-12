// ////////////////////////////////////////////////////////
// # Title
// Squares under a hyperbola
//
// # URL
// https://projecteuler.net/problem=247
// http://euler.stephan-brumme.com/247/
//
// # Problem
// Consider the region constrained by `1 <= x` and `0 <= y <= 1/x`.
//
// Let `S_1` be the largest square that can fit under the curve.
// Let `S_2` be the largest square that fits in the remaining area, and so on.
// Let the index of Sn be the pair (left, below) indicating the number of squares to the left of `S_n` and the number of squares below `S_n`.
//
// ![hyperbola](p247_hypersquares.gif)
//
// The diagram shows some such squares labelled by number.
// `S_2` has one square to its left and none below, so the index of `S_2` is (1,0).
// It can be seen that the index of `S_32` is (1,1) as is the index of `S_50`.
// 50 is the largest `n` for which the index of `S_n` is (1,1).
//
// What is the largest `n` for which the index of `S_n` is (3,3)?
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// Each square has a lower-left corner `(x_0,y_0)` and an upper-right corner `(x_1,y_1)` such that:
// (1) `x_1 - x_0 = y_1 - y_0 = side` (length of each side of that square)
//
// The hyperbola `y = 1/x` must go through the upper-right corner:
// (2) `y_1 = dfrac{1}{x_1}`
//
// Substituting (2) in (1):
// (3) `x_1 - x_0 = dfrac{1}{x_1} - y_0`
//
// And solve for `x_1`:
// (4) `x_1 - dfrac{1}{x_1} = x_0 - y_0`
// (5) `dfrac{x^2_1 - 1}{x_1} = x_0 - y_0`
// (6) `x^2_1 - 1 = x_1 (x_0 - y_0)`
// (7) `0 = x^2_1 - x_1 (x_0 - y_0) - 1`
// (8) `0 = x^2_1 + x_1 (y_0 - x_0) - 1`
//
// That's a quadratic equation with parameters:
// (9) `p = y_0 - x_0`, `q   = -1`
// (10) `x = -frac{p}{2} \pm sqrt{(frac{p}{2})^2 - q}`
//
// Omitting the second solution:
// (11) `x_1 = -frac{y_0 - x_0}{2} + sqrt{(frac{y_0 - x_0}{2})^2 + 1}`
// (12) `x_1 = frac{x_0 - y_0}{2} + frac{1}{2} sqrt{(x_0 - y_0)^2 + 4}`
// (13) `x_1 = 0.5 * (x_0 - y_0 + sqrt{(x_0 - y_0)^2 + 4})`
//
// Therefore the length of a side in (1) becomes:
// (14) `side = x_1 - x_0`
// (15) `side = 0.5 * (x_0 - y_0 + sqrt{(x_0 - y_0)^2 + 4}) - x_0`
//
// And a bit simplified:
// (16) `side = 0.5 * (-x_0 - y_0 + sqrt{(x_0 - y_0)^2 + 4})`
// (17) `side = 0.5 * (sqrt{(x_0 - y_0)^2 + 4} - x_0 - y_0)`
//
// That formula can be found in ''Square::setSideLength''.
//
// My program starts with a single square whose lower-left corner (x, y) is at (1, 0).
// All squares are stored in an ''std::set'' named ''todo'' which is sorted descendingly by the square's size
// (the overloaded ''Square::operator<'' returns the "wrong" result on purpose because an ''std::set'' is sorted ascendingly by default).
//
// I always pick the largest (==> the first) square from ''todo'' and replace it by its upper and right neighbor.
// Each square's position (x, y) and index (left, below) are tracked.
//
// In order to have a square at index (3, 3) I have to have a square at (2, 3) or (3, 2).
// Generalized, only if some squares are in ''todo'' where `left <= 3` and `below <= 3` and `left + below < 3+3` then it's
// still possible to generate a square at index (3, 3).
// ''candidates'' counts how many squares fulfil that condition. If ''candidates'' becomes zero then no more squares can be at (3,3).
// The most recent will be printed.

#include <iostream>
#include <set>
#include <cmath>

// a square is described by its lower left corner (x,y) and its index along x and y axis
struct Square
{
  Square(double x_, double y_, unsigned int left_, unsigned int below_)
  : x(x_), y(y_), left(left_), below(below_)
  {
    setSideLength();
  }

  // lower-left corner
  double x;
  double y;
  // number of squares on the left side
  unsigned int left;
  // number of squares below
  unsigned int below;

  // length of a side
  double side;

  // note: sort in reverse, therefore return true if it's the larger (!) value
  bool operator<(const Square& other) const
  {
    return side > other.side;
  }

private:
  // find length of edges of the largest square whose lower-left corner is at x,y
  void setSideLength()
  {
    // see my explanations above
    side = 0.5 * (sqrt((x - y) * (x - y) + 4) - x - y);
  }
};

int main()
{
  unsigned int indexLeft  = 3;
  unsigned int indexBelow = 3;
  std::cin >> indexLeft >> indexBelow;

  // square's number
  unsigned int result     = 0;

  // create first square with lower-left corner at (1,0) and no other squares on its left or bottom side
  std::set<Square> todo = { Square(1, 0, 0, 0) };
  unsigned int candidates = 1; // => the first square is left of and below all other squares
  while (candidates > 0)
  {
    result++;

    // pick first (= largest) square
    auto current = *(todo.begin());
    // and remove it
    todo.erase(todo.begin());

    // create a new square on top of the current square
    Square top  (current.x,    current.y + current.side,
                 current.left, current.below + 1);
    todo.insert(top);
    // and a square on the right side
    Square right(current.x + current.side, current.y,
                 current.left + 1, current.below);
    todo.insert(right);

    // count how many squares could be on the left or bottom side of a square with index (3, 3)
    if (top    .left <= indexLeft && top    .below <= indexBelow)
      candidates++;
    if (right  .left <= indexLeft && right  .below <= indexBelow)
      candidates++;
    if (current.left <= indexLeft && current.below <= indexBelow)
      candidates--;
  }

  // show ID of last square with index (3, 3)
  std::cout << result << std::endl;
  return 0;
}

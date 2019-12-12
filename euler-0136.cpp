// ////////////////////////////////////////////////////////
// # Title
// Singleton differences
//
// # URL
// https://projecteuler.net/problem=136
// http://euler.stephan-brumme.com/136/
//
// # Problem
// The positive integers, `x`, `y`, and `z`, are consecutive terms of an arithmetic progression.
// Given that `n` is a positive integer, the equation, `x^2 - y^2 - z^2 = n`, has exactly one solution when `n = 20`:
//
// `13^2 - 10^2 - 7^2 = 20`
//
// In fact there are twenty-five values of `n` below one hundred for which the equation has a unique solution.
//
// How many values of `n` less than fifty million have exactly one solution?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// My solution is almost identical to problem 135. See there for an explanation of the algorithm.
// I actually submitted the correct solution to this problem just four minutes after.
//
// Then I thought: let's reduce the memory consumption ! The code from problem 135 required about 200 MByte RAM.
// I replaced the ''std::vector<unsigned int> solutions'' by two bitfields ''atLeastOne'' and ''moreThanOne'':
// - both have one bit per possible solution
// - both are initialized with false
// - if ''atLeastOne[current]'' is ''false'', then there was no solution so far ==> set it to ''true''
// - if ''atLeastOne[current]'' is ''true'',  then there was at least one solution ==> set ''moreThanOne[current]'' to ''true''
//
// The correct result is the number of all ''true'' entries in ''atLeastOne'' minus all ''true'' in ''moreThanOne''.
//
// Memory consumption is reduced to 1/16th ... or 14 MByte including I/O overhead etc. Execution time remained unchanged.

#include <iostream>
#include <vector>

int main()
{
  const unsigned int limit = 50000000;
  // all bits set to zero
  std::vector<bool> atLeastOne (limit, false);
  std::vector<bool> moreThanOne(limit, false);

  for (unsigned int a = 1; a < limit; a++)
    for (auto b = (a + 3) / 4; b < a; b++)
    {
      auto current = a * (4*b - a);
      if (current >= limit)
        break;

      // already had a solution (or more) ?
      if (atLeastOne[current])
        moreThanOne[current] = true;
      else
        // nope, first solution
        atLeastOne [current] = true;
    }

  // count all with exactly 1 solution
  unsigned int count = 0;
  for (unsigned int i = 0; i < limit; i++)
    if (atLeastOne[i] && !moreThanOne[i])
      count++;

  std::cout << count << std::endl;

  return 0;
}

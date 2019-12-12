// ////////////////////////////////////////////////////////
// # Title
// Sum of a square and a cube
//
// # URL
// https://projecteuler.net/problem=348
// http://euler.stephan-brumme.com/348/
//
// # Problem
// Many numbers can be expressed as the sum of a square and a cube. Some of them in more than one way.
//
// Consider the palindromic numbers that can be expressed as the sum of a square and a cube, both greater than 1, in exactly 4 different ways.
// For example, 5229225 is a palindromic number and it can be expressed in exactly 4 different ways:
//
// `2285^2 + 20^3`
// `2223^2 + 66^3`
// `1810^2 + 125^3`
// `1197^2 + 156^3`
//
// Find the sum of the five smallest such palindromic numbers.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// My ''SquareCube'' objects are simple ''struct''s that store the ''value'' of a ''square'' and a ''cube''.
// It contains two comparison functions: ''operator=='' returns true when two objects are equal (obviously ...).
// However, ''operator<'' returns the "wrong" result: I flipped true, if the current object is __bigger__ than the parameter.
// The reason behind my weird logic is that I use the ''std::priority_queue'' is returns always the largest object.
// By flipping ''operator<'' I can modifiy it to returns the smallest object.
//
// I create all numbers which can be expressed as the sum of square and a cube in ascending order.
// Initially an object ''SquareCube'' which represents `2^2 + 3^2` is stored in a ''std::priority_queue''.
// Whenever I take an object from the ''top'' of the queue, I add two objects `(square+1)^2 + cube^3` and `square^2 + (cube+1)^3`.
// And whenever I can pick exactly four objects in a row with the same value, then I check whether they are palindromes.
//
// # Note
// Many numbers will occur twice in the queue, e.g. when I pick `35 = 3^2 + 3^3` from the queue then it was inserted after
// picking `16 = 3^2 + 2^3` and after picking `31 = 2^2 + 3^3`. I can easily spot those duplicates with my overloaded ''operator==''.
//
// # Alternative
// A much simpler alternative is to create a huge array (about `10^9` entries) which contains only zeros.
// Then generate all pairs `(square,cube)` and increment ''myarray[square*square + cube*cube*cube]''.
// Look for the 5 smallest indices where ''myerray[] = 5''.
//
// Remember that this approach needs about 1 GByte of memory. My solution needs less than 5 MByte.

#include <iostream>
#include <vector>
#include <queue>
#include <string>

// represent a number where "value = square^2 + cube^3"
struct SquareCube
{
  // create a new object made of a square and a cube
  SquareCube(unsigned int square_, unsigned int cube_)
  : square(square_), cube(cube_),
    value(cube_*(unsigned long long)cube_*cube_ + square_*(unsigned long long)square_)
  { }

  // compare two objects (for std::priority_queue)
  bool operator<(const SquareCube& other) const
  {
    if (value != other.value)
      return value > other.value; // deliberately switched the sign => min-heap

    return cube > other.cube;
  }
  bool operator==(const SquareCube& other) const
  {
    return value == other.value && cube == other.cube;
  }

  unsigned int square;
  unsigned int cube;
  // equal to square^2 + cube^3
  unsigned int value;
};

int main()
{
  // find the first 5 numbers
  unsigned int maxFound = 5;
  std::cin >> maxFound;

  // all cubes and squares must be greater than 1
  std::priority_queue<SquareCube> todo;
  todo.push(SquareCube(2, 2));

  // find first 5
  unsigned int numFound = 0;
  // and their sum
  unsigned int sum = 0;

  while (numFound < maxFound)
  {
    // start a new value
    auto current = todo.top();
    // add its "successors"
    todo.emplace(current.square + 1, current.cube);
    todo.emplace(current.square, current.cube + 1);

    while (todo.top() == current)
      todo.pop();

    // count all combinations of squares and cube with the same value
    unsigned int numSame = 1;
    while (todo.top().value == current.value)
    {
      numSame++;
      auto same = todo.top();

      // same value, remove it and add its successors
      while (todo.top() == same)
        todo.pop();
      todo.emplace(same.square + 1, same.cube);
      todo.emplace(same.square, same.cube + 1);
    }

    // exactly four combinations ?
    if (numSame == 4)
    {
      // palindrome ?
      unsigned int reverse = 0;
      auto reduce = current.value;
      while (reduce > 0)
      {
        reverse *= 10;
        reverse += reduce % 10;
        reduce  /= 10;
      }

      // yes, a match
      if (current.value == reverse)
      {
        numFound++;
        sum += current.value;
      }
    }
  }

  std::cout << sum << std::endl;
  return 0;
}

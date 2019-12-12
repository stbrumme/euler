// ////////////////////////////////////////////////////////
// # Title
// Cube digit pairs
//
// # URL
// https://projecteuler.net/problem=90
// http://euler.stephan-brumme.com/90/
//
// # Problem
// Each of the six faces on a cube has a different digit (0 to 9) written on it; the same is done to a second cube.
// By placing the two cubes side-by-side in different positions we can form a variety of 2-digit numbers.
//
// For example, the square number 64 could be formed:
// ![cubes](p090.gif)
//
// In fact, by carefully choosing the digits on both cubes it is possible to display all of the square numbers below one-hundred:
// 01, 04, 09, 16, 25, 36, 49, 64, and 81.
//
// For example, one way this can be achieved is by placing {0, 5, 6, 7, 8, 9} on one cube and {1, 2, 3, 4, 8, 9} on the other cube.
//
// However, for this problem we shall allow the 6 or 9 to be turned upside-down so that an arrangement like {0, 5, 6, 7, 8, 9} and {1, 2, 3, 4, 6, 7}
// allows for all nine square numbers to be displayed; otherwise it would be impossible to obtain 09.
//
// In determining a distinct arrangement we are interested in the digits on each cube, not the order.
//
// {1, 2, 3, 4, 5, 6} is equivalent to {3, 6, 4, 1, 2, 5}
// {1, 2, 3, 4, 5, 6} is distinct from {1, 2, 3, 4, 5, 9}
//
// But because we are allowing 6 and 9 to be reversed, the two distinct sets in the last example both represent the extended set {1, 2, 3, 4, 5, 6, 9}
// for the purpose of forming 2-digit numbers.
//
// How many distinct arrangements of the two cubes allow for all of the square numbers to be displayed?
//
// # Solved by
// Stephan Brumme
// March 2017
//
// # Algorithm
// [TODO] write down a few words

#include <algorithm>
#include <vector>
#include <iostream>

// all sides of a dice
typedef std::vector<unsigned int> Dice;

// each cube has 6 different sides, we can choose any 6 out of 10
const Dice Sides = { 0,1,2,3,4,5,6,7,8,9 };

const unsigned int Skip = 0;
const unsigned int Take = 1;
// if container[x] is Take, then Sides[x] is part of the dice
const std::vector<unsigned int> Initial = { Skip,Skip,Skip,Skip, Take,Take,Take,Take,Take,Take };
const std::vector<unsigned int> Unused  = { Take };

int main()
{
  unsigned int dices = 2;
  unsigned int limit = 9; // up to the 9th square (9^2 = 81)
  std::cin >> limit >> dices;

  // Hackerrank extended the problem to three dices
  const unsigned int AllDices = 3;
  unsigned int maxSquare = 0;

  // generate all square numbers
  std::vector<unsigned short> squares;
  for (unsigned int i = 1; i <= limit; i++)
  {
    auto reduce = i*i;
    maxSquare = reduce;

    std::vector<unsigned int> digits;
    // no matter what, always generate a three-digit square (maybe with some leading zeros)
    for (unsigned int j = 0; j < AllDices; j++)
    {
      auto digit = reduce % 10;
      reduce   /= 10;
      // convert all 9s to 6s
      if (digit == 9)
        digit = 6;

      digits.push_back(digit);
    }

    // digits in ascending order
    std::sort(digits.begin(), digits.end());
    // convert to a fingerprint
    // e.g. 9^2 = 081 (with leading zero)
    // => 018 (sorted)
    // => 18  (sorted, converted to an integer)
    auto sortedSquare = digits[0] * 100 + digits[1] * 10 + digits[2];
    if (std::find(squares.begin(), squares.end(), sortedSquare) == squares.end())
      squares.push_back(sortedSquare);
  }

  // will contain all solutions
  unsigned int valid = 0;
  // all possible label combinations for first dice
  Dice dice1, dice2, dice3;
  auto open = squares;

  auto permutationDice1 = Initial;
  auto permutationDice2 = Initial;
  auto permutationDice3 = Initial;

  do
  {
    dice1.clear();
    for (size_t i = 0; i < permutationDice1.size(); i++)
      if (permutationDice1[i] == Take)
        dice1.push_back(Sides[i]);

    // if both 6 and 9 are contained, then they behave identical
    //if (permutationDice1[6] == Take && permutationDice1[9] == Take)
      //dice1.pop_back();

    // second dice is "lexicographically" bigger than or equal to first dice
    permutationDice2 = (dices >= 2 ? permutationDice1 : Unused);
    do
    {
      dice2.clear();
      for (size_t i = 0; i < permutationDice2.size(); i++)
        if (permutationDice2[i] == Take)
          dice2.push_back(Sides[i]);

      // some digits need to occur at least twice
      if (maxSquare >= 100)
      {
        // 100 requires two zeros, so we must already have at least one
        if (std::count(dice1.begin(), dice1.end(), 0) +
            std::count(dice2.begin(), dice2.end(), 0) < 1)
          continue;
      }
      if (maxSquare >= 144)
      {
        // 144 requires two 4s, so we must have at least one by now
        if (std::count(dice1.begin(), dice1.end(), 4) +
            std::count(dice2.begin(), dice2.end(), 4) < 1)
          continue;
      }

      // if less than three dices are requested then add a dummy dice with one side
      permutationDice3 = (dices == 3 ? permutationDice2 : Unused);
      do
      {
        dice3.clear();
        for (size_t i = 0; i < permutationDice3.size(); i++)
          if (permutationDice3[i] == Take)
            dice3.push_back(Sides[i]);

        // simple pre-check
        unsigned int frequency[10] = { 0,0,0,0,0, 0,0,0,0,0 };
        for (auto x : dice1)
          frequency[x]++;
        for (auto x : dice2)
          frequency[x]++;
        for (auto x : dice3)
          frequency[x]++;

        // for performance optimization only: reject impossible combinations
        if (frequency[1] < 1)
          continue;
        if (maxSquare >=   4 && frequency[4] < 1)
          continue;
        if (maxSquare >=  25 && frequency[2] < 1)
          continue;
        if (maxSquare >=  25 && frequency[5] < 1)
          continue;
        if (maxSquare >=  36 && frequency[3] < 1)
          continue;
        if (maxSquare >=  81 && frequency[8] < 1)
          continue;
        if (maxSquare >= 100 && frequency[0] < 2)
          continue;
        if (maxSquare >= 144 && frequency[4] < 2)
          continue;

        std::vector<unsigned int> matches;
        // build all combinations and remove any squares we encounter along the way
        for (auto one : dice1)
        {
          // 6 is 9 (upside down)
          if (one == 9)
            one = 6;
          for (auto two : dice2)
          {
            // 6 is 9 (upside down)
            if (two == 9)
              two = 6;

            for (auto three : dice3)
            {
              // 6 is 9 (upside down)
              if (three == 9)
                three = 6;

              unsigned int current[AllDices] = { one, two, three };

              // std::sort is much slower for such small containers
              if (current[0] > current[1])
                std::swap(current[0], current[1]);
              if (current[1] > current[2])
                std::swap(current[1], current[2]);
              if (current[0] > current[1])
                std::swap(current[0], current[1]);

              auto sortedSquare = 100 * current[0] + 10 * current[1] + current[2];
              // if successful then another square number was matched
              auto match = std::find(squares.begin(), squares.end(), sortedSquare);
              // remove it from the list
              if (match != squares.end())
                matches.push_back(sortedSquare);
            }
          }
        }

        if (matches.size() < squares.size())
          continue;

        std::sort(matches.begin(), matches.end());
        auto last = std::unique(matches.begin(), matches.end());

        open = squares;
        for (auto m = matches.begin(); m != last; m++)
        {
          auto match = std::find(open.begin(), open.end(), *m);
          open.erase(match);
        }

        // all squares matched ?
        if (open.empty())
          valid++;
      } while (std::next_permutation(permutationDice3.begin(), permutationDice3.end()));
    } while (std::next_permutation(permutationDice2.begin(), permutationDice2.end()));
  } while (std::next_permutation(permutationDice1.begin(), permutationDice1.end()));

  std::cout << valid;
  return 0;
}

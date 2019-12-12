// ////////////////////////////////////////////////////////
// # Title
// Darts
//
// # URL
// https://projecteuler.net/problem=109
// http://euler.stephan-brumme.com/109/
//
// # Problem
// In the game of darts a player throws three darts at a target board which is split into twenty equal sized sections numbered one to twenty.
//
// ![dart board](p109.gif)
//
// The score of a dart is determined by the number of the region that the dart lands in. A dart landing outside the red/green outer ring scores zero.
// The black and cream regions inside this ring represent single scores. However, the red/green outer ring and middle ring score double and treble scores respectively.
//
// At the centre of the board are two concentric circles called the bull region, or bulls-eye. The outer bull is worth 25 points and the inner bull is a double, worth 50 points.
//
// There are many variations of rules but in the most popular game the players will begin with a score 301 or 501 and the first player to reduce their running total to zero is a winner.
// However, it is normal to play a "doubles out" system, which means that the player must land a double (including the double bulls-eye at the centre of the board) on their final dart to win;
// any other dart that would reduce their running total to one or lower means the score for that set of three darts is "bust".
//
// When a player is able to finish on their current score it is called a "checkout" and the highest checkout is 170: T20 T20 D25 (two treble 20s and double bull).
// There are exactly eleven distinct ways to checkout on a score of 6:
//
// ||  3 ||  3 ||  3 ||
// || D3 ++    ++    ||
// || D1 ++ D2 ++    ||
// || S2 ++ D2 ++    ||
// || D2 ++ D1 ++    ||
// || S4 ++ D1 ++    ||
// || S1 ++ S1 ++ D2 ||
// || S1 ++ T1 ++ D1 ||
// || S1 ++ S3 ++ D1 ||
// || D1 ++ D1 ++ D1 ||
// || D1 ++ S2 ++ D1 ||
// || S2 ++ S2 ++ D1 ||
//
// Note that D1 D2 is considered __different__ to D2 D1 as they finish on different doubles. However, the combination S1 T1 D1 is considered the __same__ as T1 S1 D1.
//
// In addition we shall not include misses in considering combinations; for example, D3 is the __same__ as 0 D3 and 0 0 D3.
//
// Incredibly there are 42336 distinct ways of checking out in total.
//
// How many distinct ways can a player checkout with a score less than 100?
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// Each field of the board is represented by a simple struct ''Field''.
// It carries information about the base value (e.g. 20), the factor (single, double, triple) and the total score of this field (e.g. 20 with factor 3 ==> 60).
//
// All fields are added to a container ''fields''. I even include a "zero" field which indicates a dart that missed the board. It has value = factor = zero.
//
// The outer loop iterates over all total scores 1 to 99.
// Three inner loops iterate over all fields: I start with the third dart which must be a double (I only did this make the code run faster).
// The first and second dart are interchangeable, therefore I define that the field ID of the second dart can't be lower than the first dart.
// If the sum of all three dart matches the total score then a valid checkout was achieved.
//
// The misses can only occur on the first dart or the first plus the second dart. Therefore the "zero" field must be the first in my list of fields.
//
// # Alternative
// I'm pretty sure you can find some closed formula. But my brute-force approach finishes in less than 0.01 seconds, even for the full board (''lessPointsThan = 181'') ...
//
// # Note
// You can optimize the loops, too: if you sort ascendingly the fields by their total value then some ''continue'' statements can be replaced by ''break''.

#include <iostream>
#include <vector>

// a single field
struct Field
{
  Field(unsigned int value_, unsigned int factor_)
  : value(value_), factor(factor_), total(value_ * factor_)
  {}
  unsigned int value;
  unsigned int factor;
  unsigned int total;
};

int main()
{
  unsigned int lessPointsThan = 100;
  std::cin >> lessPointsThan;

  // set up board
  std::vector<Field> fields;
  // dummy element: dart missed the board
  fields.push_back({ 0, 0 });

  // "normal" fields incl. double and triple ring
  for (unsigned int i = 1; i <= 20; i++)
  {
    fields.push_back({ i, 1 });
    fields.push_back({ i, 2 });
    fields.push_back({ i, 3 });
  }

  // bull's eye
  fields.push_back({ 25, 1 });
  fields.push_back({ 25, 2 });

  // final result
  unsigned int combinations = 0;
  // count combinations when 1..99 points left on the board
  for (unsigned int total = 1; total < lessPointsThan; total++)
  {
    // start with the final dart because its restrictions eliminate many iterations ...
    for (auto third : fields)
    {
      // must be double
      if (third.factor != 2 || third.total > total) // simple tweak: if this dart scores too many points then skip it
        continue;

      // throw first and second dart
      for (size_t dart1 = 0; dart1 < fields.size(); dart1++)
        for (size_t dart2 = dart1; dart2 < fields.size(); dart2++)
        {
          auto first  = fields[dart1];
          auto second = fields[dart2];

          // correct number of points ?
          auto sum = first.total + second.total + third.total;
          if (sum != total)
            continue;

          // yes, one more solution
          combinations++;
        }
    }
  }

  std::cout << combinations << std::endl;
  return 0;
}

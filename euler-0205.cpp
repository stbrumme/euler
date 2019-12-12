// ////////////////////////////////////////////////////////
// # Title
// Dice Game
//
// # URL
// https://projecteuler.net/problem=205
// http://euler.stephan-brumme.com/205/
//
// # Problem
// Peter has nine four-sided (pyramidal) dice, each with faces numbered 1, 2, 3, 4.
// Colin has six six-sided (cubic) dice, each with faces numbered 1, 2, 3, 4, 5, 6.
//
// Peter and Colin roll their dice and compare totals: the highest total wins. The result is a draw if the totals are equal.
//
// What is the probability that Pyramidal Pete beats Cubic Colin? Give your answer rounded to seven decimal places in the form ''0.abcdefg''
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// The function ''roll'' counts all possible outcomes when rolling a number of dices.
// In ''main'' I iterate over all possible totals and count how often Peter wins against Colin:
//
// `beats_i = dfrac{ sum_{1..i-1}{six_i}}{ sum_{1..36}{six_i}}`
//
// `beats_i` has to be multiplied by the probability that Peter rolls `i`, which is:
//
// `roll_i = dfrac{four_i}{ sum_{1..36}{four_i}}`
//
// # Note
// I wasn't sure how to submit my solution: whether I should type in the leading zero and the decimal dot - or not.
// Turns out you should enter your result the way it is printed by the program, including both zero and dot.
//
// An interactive [live test](#live) is available for this problem.
//
// # Alternatives
// My gut tells me there is a closed form to compute the result instantly.

#include <iostream>
#include <iomanip>
#include <vector>

// roll dices, store at count[x] how often the sum of all dices was exactly x
void roll(unsigned int dices, unsigned int sides, std::vector<unsigned int>& count, unsigned int sum = 0)
{
  // rolled all dices, increment the sum's counter
  if (dices == 0)
  {
    count[sum]++;
    return;
  }

  // all combinations of a dice ...
  for (unsigned int i = 1; i <= sides; i++)
    roll(dices - 1, sides, count, sum + i);
}

int main()
{
  unsigned int dicesPeter, sidesPeter;
  std::cin >> dicesPeter >> sidesPeter; // 6 6
  unsigned int dicesColin, sidesColin;
  std::cin >> dicesColin >> sidesColin; // 9 4

  // "high score", by default 9*4 = 6*6 = 36
  unsigned int maxTotal = std::max(dicesPeter * sidesPeter, dicesColin * sidesColin);

  // roll all 6x6 dices (Colin)
  std::vector<unsigned int> colin(maxTotal + 1, 0);
  roll(dicesPeter, sidesPeter, colin);
  unsigned int sumColin = 0; // will be  46656 for 6,6
  for (auto x : colin)
    sumColin += x;

  // roll all 9x4 dices (Peter)
  std::vector<unsigned int> peter(maxTotal + 1, 0);
  roll(dicesColin, sidesColin, peter);
  unsigned int sumPeter = 0; // will be 262144 for 9,4
  for (auto x : peter)
    sumPeter += x;

  // for each of Peter's potential dice sums, find how often it wins against Colin
  double winPeter = 0;
  for (unsigned int sum = 1; sum <= maxTotal; sum++)
  {
    // win if total is higher
    unsigned int numWins = 0;
    // => add all combinations of Colin that are smaller
    for (unsigned int j = 1; j < sum; j++)
      numWins += colin[j];

    // compute percentage that current sum (Peter) wins against Colin
    double beats = numWins / double(sumColin);
    // add all of them ...
    winPeter += beats * peter[sum] / sumPeter;
  }

  // done !
  std::cout << std::fixed << std::setprecision(7) << winPeter << std::endl;
  return 0;
}

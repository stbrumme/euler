// ////////////////////////////////////////////////////////
// # Title
// Coin sums
//
// # URL
// https://projecteuler.net/problem=31
// http://euler.stephan-brumme.com/31/
//
// # Problem
// In England the currency is made up of pound, &pound;, and pence, p, and there are eight coins in general circulation:
//
// 1p, 2p, 5p, 10p, 20p, 50p, &pound;1 (100p) and &pound;2 (200p).
//
// It is possible to make &pound;2 in the following way:
//
// 1x &pound;1 + 1x 50p + 2x 20p + 1x 5p + 1x 2p + 3x 1p
//
// How many different ways can &pound;2 be made using any number of coins?
//
// # Solved by
// Stephan Brumme
// February 2017
//
// # Algorithm
// My program creates a table ''history'' that contains the number of combinations for a given sum of money:
// - its entry ''history[0] '' refers to &pound;0
// - its entry ''history[1]'' refers to &pound;0.01
// - its entry ''history[2]'' refers to &pound;0.02
// - its entry ''history[3]'' refers to &pound;0.03
// - ...
// - its entry ''history[200]'' refers to &pound;2.00
//
// There are 8 different coins and therefore each entry of ''history'' is a ''std::vector'' itself with 8 elements:
// it tells how many combinations exist if only the __current coin or smaller coins__ are used.
//
// For example, there is always one way/combination to pay a certain amout if you only have single pennies.
// That means, the first element is always ''1''.
//
// Moreover, each of the next element is at least 1, too, because I said: "current coin __or smaller coins__".
// If we would like to pay &pound;0.01 then ''history[1] = { 1,1,1,1,1,1,1,1 }''.
//
// Now comes the only part that isn't obvious: there is one combination of paying __zero pounds__, too:
// ''history[0] = { 1,1,1,1,1,1,1,1 }''. From now on, everything comes natural, trust me, ...
//
// If we would like to pay &pound;0.02 then there are two ways: pay with two single pennies or a 2p coin.
// What we do is:
// 1. try __not__ to use the current coin (2p in our case), only smaller coins ==> there is one combination
// 2. try to use the current coin (2p in our case) ==> then there are 0.00 &pound; left which is possible in one way
// 3. add 1.+2.
//
// So far we had ''history[2] = { 1,?,?,?,?,?,?,? }''
// Step 1 is the same as ''history[2][currentCoinId - 1] = history[2][0] = 1''.
// Step 2 is the same as ''history[2 - currentCoinValue][currentCoinId] = history[0][1] = 1''.
// Therefore we have ''1+1=2'' combinations (as expected:) ''history[2] = { 1,2,?,?,?,?,?,? }''.
//
// The next coin, it's the 5p coin, can't be used because it's bigger than the total of &pound;0.02. In software terms ''currentCoinValue > total''.
// Only step 1 applies to all remaining elements: ''history[2] = { 1,2,2,2,2,2,2,2 }''.
//
// What does it mean ? There are 2 ways to pay 0.02 &pound; with 1p and 2p. And there are still only two ways if you use all coins up to &pound;2.
// When the program computes ''history[200]'' then the result of the problem is stored in the last element (''history[200][7]'').
//
// # Hackerrank
// There are multiple test cases. My program computes all combinations up to the input values and stores them in ''history''.
// If a test case's input is smaller than something we had before then no computation is required at all, it will become a basic table lookup.
//
// The results may exceed 32 bits and thus I compute `mod 10^9+7` whenever possible (as requested by their modified problem statement).

#include <iostream>
#include <vector>

const unsigned int NumCoins = 8;
// face value of all coins in cents
const unsigned int Coins[NumCoins] = { 1,2,5,10,20,50,100,200 };

// store number of combinations in [x] if coin[x] is allowed:
// [0] => combinations if only pennies are allowed
// [1] => 1 cent and 2 cents are allowed, nothing more
// [2] => 1 cent, 2 cents and 5 cents are allowed, nothing more
// ...
// [6] => all but 2 pounds (= 200 cents) are allowed
// [7] => using all coins if possible
typedef std::vector<unsigned long long> Combinations;

int main()
{
  // remember combinations for all prices from 1 cent up to 200 cents (2 pounds)
  std::vector<Combinations> history;

  unsigned int tests;
  std::cin >> tests;
  while (tests--)
  {
    unsigned int total;
    std::cin >> total;

    // initially we start at zero
    // but if there are previous test cases then we can re-use the old results
    for (unsigned int cents = history.size(); cents <= total; cents++)
    {
      // count all combinations of those 8 coins
      Combinations ways(NumCoins);

      // one combination if using only 1p coins (single pennys)
      ways[0] = 1;

      // use larger coins, too
      for (size_t i = 1; i < ways.size(); i++)
      {
        // first, pretend not to use that coin (only smaller coins)
        ways[i] = ways[i - 1];

        // now use that coin once (if possible)
        auto currentCoin = Coins[i];
        if (cents >= currentCoin)
        {
          auto remaining = cents - currentCoin;
          ways[i] += history[remaining][i];
        }

        // not needed for the original problem, only for Hackerrank's modified problem
        ways[i] %= 1000000007;
      }

      // store information for future use
      history.push_back(ways);
    }

    // look up combinations
    auto result = history[total];
    // the last column (allow all coins) contains the desired value
    auto combinations = result.back();
    combinations %= 1000000007; // for Hackerrank only
    std::cout << combinations << std::endl;
  }

  return 0;
}

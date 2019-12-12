// ////////////////////////////////////////////////////////
// # Title
// Licence plates
//
// # URL
// https://projecteuler.net/problem=371
// http://euler.stephan-brumme.com/371/
//
// # Problem
// Oregon licence plates consist of three letters followed by a three digit number (each digit can be from [0..9]).
// While driving to work Seth plays the following game:
// Whenever the numbers of two licence plates seen on his trip add to 1000 that's a win.
//
// E.g. MIC-012 and HAN-988 is a win and RYU-500 and SET-500 too. (as long as he sees them in the same trip).
//
// Find the expected number of plates he needs to see for a win.
// Give your answer rounded to 8 decimal places behind the decimal point.
//
// Note: We assume that each licence plate seen is equally likely to have any three digit number on it.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// The problem description tries hard to confuse the reader ... the plates' letters (e.g. RYU or SET) don't matter at all.
// The only relevant part of a plate is its number. And I've never seen a leading zero on a plate - though it's possible according to the text.
// In the end it doesn't matter as well.
//
// A simple Monte-Carlo simulation can find the first digits (see ''monteCarlo()''):
// - generate a random number ''plate''
// - if ''1000 - plate'' was seen before then the game is over
// - add ''plate'' to the set of seen plates
//
// There are three groups of numbers:
// (a) 500 can only be paired with a second 500
// (b) 0 doesn't pair with any number
// (c) all other numbers
//
// The whole problem can be mapped to states (not the meaning "countries" but "character"):
// - there are 499 pairs in group c ==> 499 states
// - for each of those state there are two options: 500 wasn't seen or it was seen once
// ==> multiplying both results in 998 states
// I decided to create two arrays ''no500'' and ''with500'', each with 499 elements (for a total of 998 values).
//
// And their state transitions:
// - the "zero" plate (group b) doesn't change the current state
//
// The third group (1 - 499 and 501 - 999) has an interesting property:
// for each pair `(x, y)` where `x + y = 1000` is doesn't matter whether `x < y` or `x > y`.
// Therefore I assume that the first number of any pair in group c is always smaller than 500.
// The exact numbers on the plates don't affect the result as long as they belong to group c: observing (12,430,299) is identical to (1,2,3).
// In the end I am only concerned whether a plate's number is new (==> choose the next free number 1,2,3,...), is a duplicate or pairs with an existing number.
// The probabilities are (assuming ''have'' denoted the number of unpaired plates in group c):
// (1) `p_{new}  = (998 - 2 * have) / 1000`
// (2) `p_{finished}  = have / 1000`
// (3) `p_{duplicate} = have / 1000`
// (4) `p_{zero} = 1 / 1000`
// (5) `p_{500}  = 1 / 1000`
//
// Stupid me needs to verify that I got everything right: `1 = p_{new} + p_{finished} + p_{duplicate} + p_{zero} + p_{500}`  ==> it's okay !
//
// If I progress from one state to another then the expected value is
// (6) `E = 1 + p_{E_1} * E_1 + p_{E_2} * E_2 + p_{E_3} * E_3 + ...`
//
// I have to be careful with state transitions that lead back to the same state they started from:
// (7) `E = 1 + p_E * E`
// (8) `E - p_E * E = 1`
// (9) `E * (1 - p_E) = 1`
// (10) `E = 1 / (1 - p_E)`
// ==> e.g. assume I throw a fair coin `p_E = 0.5` => `E = 1 / (1 - 0.5) = 1 / 0.5 = 2` so I expect to need two attempts to see the other side
// ==> e.g. assume I throw a biased coin `p_E = 0.6` that has a higher probability to show the last side => `E = 1 / (1 - 0.6) = 1 / 0.4 = 2.5` attempts to see the other side
//
// And the generalized equation is `E = (1 + p_{E_1} * E_1 + p_{E_2} * E_2 + ...) / (1 - p_E)`
//
// I found it much easier to reverse the problem:
// if I have seen all plates 1 to 499 (and there was no pair yet), then
// 1. there is a chance `1 / 1000` to see "zero"
// 2. there is a chance `499 / 1000` to see an old plate
// 3. there is a chance `499 / 1000` to finally see a matching plate
// 4. there is a chance `1 / 1000` to see "500"
//
// The first two cases keep the player in its current state.
// The game is won if case 3 happens-
// If the current state includes that "500" was seen before, then the game is over, too.
// If the current state includes that "500" was __NOT__ seen before, then the state changes to "have500 with 499 plates".
//
// Now the expected number for ''have500[499]'' is:
// (11) `E_{499,have500} = 1 / (1 - (p_{zero} + p_{duplicate})) = 2`
//
// And ''no500[499]'':
// (12) `E_{499,no500} = (1 + 1/1000 * E_{499,have500}) / (1 - (p_{zero} + p_{duplicate})) = 2.4`
//
// Now `E_{498,have500}` can be computed by using the probabilities of staying in the current state and both state transitions (finish or advance to 499 plates).
// With similar argument `E_{498,no500}` depends only on `E_{498,have500}` and `E_{499,no500}`.
// A ''for''-loop computes all values.
//
// Initially, the player hasn't seen a plate at all. That means `E_{0,no500}` is the result.
//
// # Note
// ''search()'' computes bad results if the number of plates is odd because then there is no equivalent to the unique features of the "500" plates.
//
// It took me a few minutes to realize that "zero" is special: first, I didn't think it's a valid plate number at all and second, I'm dumb. Sad but true ...
//
// # Alternative
// A Dynamic Programming approach sounds viable but I highly doubt it can match the efficiency of the plain ''for''-loop.

#include <iostream>
#include <iomanip>
#include <vector>

// a simple pseudo-random number generator
// (produces the same result no matter what compiler you have - unlike rand() from math.h)
unsigned int myrand()
{
  // copied from problem 227
  static unsigned long long seed = 0;
  seed = 6364136223846793005ULL * seed + 1;
  return (unsigned int)(seed >> 30);
}

// simulate random journeys ...
double monteCarlo(unsigned int numPlates, unsigned int iterations)
{
  unsigned int numPlatesSeen = 0;
  for (unsigned int i = 0; i < iterations; i++)
  {
    // true if plate was seen
    std::vector<bool> seen(numPlates, false);

    while (true)
    {
      // generate a new plate
      auto plate = myrand() % numPlates;
      numPlatesSeen++;

      // look for matching plate (so that sum is 1000)
      if (plate != 0) // 0 isn't matched by any other plate
      {
        auto other = numPlates - plate;
        if (seen[other])
          break;
      }

      // mark current plate as "seen"
      seen[plate] = true;
    }
  }

  return numPlatesSeen / double(iterations);
}

// and now smarter, faster, more precise ...
double search(unsigned int numPlates)
{
  // how I compute the expected number:
  // E              = 1 + fail * E
  // E - fail * E   = 1
  // E * (1 - fail) = 1
  // E              = 1 / (1 - fail)
  // if there is another state transition E2 then with similar argument
  // E = 1 + fail * E + good * E2
  // E = (1 + good * E2) / (1 - fail)

  // Seth may observe at most 499 "ordinary" plates
  // => zero and 500 are "special"
  const auto maxHave = numPlates / 2 - 1;

  // several times I have to divide by numPlates and have to make sure the result is a double,
  // so let's convert it to double once and keep using plates instead of numPlates from here on
  const double plates = numPlates; // actually I thought about invPlates = 1 / double(numPlates)

  // track expected number of plates you still have to see if n unpaired plates were already observed
  // there is a significant difference whether the "500" plate was seen or not !
  std::vector<double> have500(maxHave + 1, 0);
  std::vector<double> no500  (maxHave + 1, 0);

  // probabilities:
  // observe an "old" plate, nothing changes (but the expected number)
  auto probDuplicate = maxHave / plates;
  // observe a  "000" plate, nothing changes (but the expected number)
  auto probZero      =    1    / plates;
  // observe a  "500" plate, stop if already saw a "500", otherwise store it
  auto prob500       =    1    / plates;

  // solve the base case: all 499 plates already seen
  // compute probability that the next plate still doesn't finish the game
  auto probUnchanged = probDuplicate + probZero;
  // now find expected number in case Seth 499 plates plus the "500" plate
  have500[maxHave]   =  1                               / (1 - probUnchanged);
  // and find expected number in case Seth 499 plates BUT NOT the "500" plate
  no500  [maxHave]   = (1 + prob500 * have500[maxHave]) / (1 - probUnchanged);

  // continue the computation for 498, 497, ..., 1, 0 observed plates
  for (int have = maxHave - 1; have >= 0; have--)
  {
    // determine number of unpaired plates (exclude zero and 500)
    auto numNew  = plates - 2*have - 2;
    // probability that the plate wasn't seen before but pairs with no other plate yet
    auto probNew = numNew / plates;

    // it becomes less probable to see an "old" plate ...
    probDuplicate = have / plates;            // same as probDuplicate -= 1 / plates
    probUnchanged = probDuplicate + probZero; // same as probUnchanged += 1 / plates

    // same as above: E = (1 + good * E2) / (1 - fail)
    have500[have] = (1                           + probNew * have500[have + 1]) / (1 - probUnchanged);
    no500  [have] = (1 + prob500 * have500[have] + probNew * no500  [have + 1]) / (1 - probUnchanged);
  }

  // no plate seen yet, not even "500"
  return no500[0];
}

int main()
{
  unsigned int numPlates = 1000;
  std::cin >> numPlates;

  std::cout << std::fixed << std::setprecision(8);
  std::cout << search(numPlates) << std::endl;

  // run simulation
  //while (true)
    //std::cout << monteCarlo(numPlates, 1000000) << std::endl;

  return 0;
}

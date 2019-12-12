// ////////////////////////////////////////////////////////
// # Title
// An amazing Prime-generating Automaton
//
// # URL
// https://projecteuler.net/problem=308
// http://euler.stephan-brumme.com/308/
//
// # Problem
// A program written in the programming language Fractran consists of a list of fractions.
//
// The internal state of the Fractran Virtual Machine is a positive integer, which is initially set to a seed value.
// Each iteration of a Fractran program multiplies the state integer by the first fraction in the list which will leave it an integer.
//
// For example, one of the Fractran programs that John Horton Conway wrote for prime-generation consists of the following 14 fractions:
//
// `dfrac{17}{91}`, `dfrac{78}{85}`, `dfrac{19}{51}`, `dfrac{23}{38}`, `dfrac{29}{33}`, `dfrac{77}{29}`, `dfrac{95}{23}`, `dfrac{77}{19}`, `dfrac{1}{17}`, `dfrac{11}{13}`, `dfrac{13}{11}`, `dfrac{15}{2}`, `dfrac{1}{7}`, `dfrac{55}{1}`
//
// Starting with the seed integer 2, successive iterations of the program produce the sequence:
// 15, 825, 725, 1925, 2275, 425, ..., 68, __4__, 30, ..., 136, __8__, 60, ..., 544, __32__, 240, ...
//
// The powers of 2 that appear in this sequence are `2^2`, `2^3`, `2^5`, ...
// It can be shown that all the powers of 2 in this sequence have prime exponents and that all the primes appear as exponents of powers of 2, in proper order!
//
// If someone uses the above Fractran program to solve Project Euler Problem 7 (find the 10001st prime), how many iterations would be needed until the program produces `2^{10001st \space prime}` ?
//
// # Solved by
// Stephan Brumme
// December 2017
//
// # Algorithm
// The crucial discovery is that all fractions can be factorized such that the largest prime factor is 29 and no prime appears as a square (or cube).
// The sequence becomes:
//
// `dfrac{17}{7 * 13}`, `dfrac{2 * 3 * 13}{5 * 17}`, `dfrac{19}{3 * 17}`, `dfrac{23}{2 * 19}`, `dfrac{29}{3 * 11}`, `dfrac{7 * 11}{29}`, `dfrac{5 * 19}{23}`, `dfrac{7 * 11}{19}`, `dfrac{1}{17}`, `dfrac{11}{13}`, `dfrac{13}{11}`, `dfrac{3 * 5}{2}`, `dfrac{1}{7}`, `dfrac{5 * 11}{1}`
//
// The 10001st prime needs 10001 bits to be represented - way beyond the 64 bits of ''unsigned long long''.
// However, I can factorize the current number as well and it's initial value is `2 = 2^1`.
// I could include all the other (non-existing) prime factors, too, so that
// `2 = 2^1 * 3^0 * 5^0 * 7^0 * 11^0 * 13^0 * 17^0 * 19^0 * 23^0 * 29^0`
//
// Each fraction increments or decrements some exponents. No exponent must become negative because then it wouldn't be an integer anymore.
// Whenever all exponents except the exponent of 2 are zero, then I found another prime and it's the exponent of 2.
//
// I wrote a brute-force function ''enumerate()'' that finds the 100th prime in a few seconds (matching the values of OEIS A007547).
// But more important, I included a "visualization" of each step which revealed:
// - the exponents of 2, 3, 5, 7 change a lot, often they are incremented or decremented
// - the exponents of 11, 13, 17, 19, 23, 29 never exceed 1
// - actually at most one of the exponents of 11, 13, 17, 19, 23, 29 is 1 and all the others are zero
//
// These are the 19 steps for the first prime (I replaced zeros by an underline to emphasize the patterns):
//
// ||     6    || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 || 2 ||
// ||!         ++ 2 ++ 3 ++ 5 ++ 7 ++ 11++ 13++ 17++ 19++ 23++ 29||
// || step  0: ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ||
// || step  1: ++ _ ++ 1 ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ||
// || step  2: ++ _ ++ 1 ++ 2 ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ||
// || step  3: ++ _ ++ _ ++ 2 ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ 1 ||
// || step  4: ++ _ ++ _ ++ 2 ++ 1 ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ||
// || step  5: ++ _ ++ _ ++ 2 ++ 1 ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ||
// || step  6: ++ _ ++ _ ++ 2 ++ _ ++ _ ++ _ ++ 1 ++ _ ++ _ ++ _ ||
// || step  7: ++ 1 ++ 1 ++ 1 ++ _ ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ||
// || step  8: ++ 1 ++ 1 ++ 1 ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ||
// || step  9: ++ 1 ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ 1 ||
// || step 10: ++ 1 ++ _ ++ 1 ++ 1 ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ||
// || step 11: ++ 1 ++ _ ++ 1 ++ 1 ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ||
// || step 12: ++ 1 ++ _ ++ 1 ++ _ ++ _ ++ _ ++ 1 ++ _ ++ _ ++ _ ||
// || step 13: ++ 2 ++ 1 ++ _ ++ _ ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ||
// || step 14: ++ 2 ++ 1 ++ _ ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ||
// || step 15: ++ 2 ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ 1 ||
// || step 16: ++ 2 ++ _ ++ _ ++ 1 ++ 1 ++ _ ++ _ ++ _ ++ _ ++ _ ||
// || step 17: ++ 2 ++ _ ++ _ ++ 1 ++ _ ++ 1 ++ _ ++ _ ++ _ ++ _ ||
// || step 18: ++ 2 ++ _ ++ _ ++ _ ++ _ ++ _ ++ 1 ++ _ ++ _ ++ _ ||
// || step 19: ++ 2 ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ++ _ ||
//
// My function ''search()'' is build on these properties. It's a state machine which has 7 states ''S_'', ''S11'', ''S13'', ''S17'', ''S19'', ''S23'' and ''S29''
// (where the number after ''S'' indicates the exponent which is currently 1 or ''S_'' if all are zero, which is the default state, too).
//
// The 14 fractions can be interpreted as:
//
// `dfrac{17}{91} = dfrac{17}{7 * 13}` ==> when in state ''S13'' then try to decrement the exponent of 7 and continue in state ''S17''
//
// `dfrac{78}{85} = dfrac{2 * 3 * 13}{5 * 17}` ==> when in state ''S17'' then try to decrement the exponent of 5, increment the exponents of 2 and 3 and continue in state ''S13''
//
// `dfrac{19}{51} = dfrac{19}{3 * 17}` ==> when in state ''S17'' then try to decrement the exponent of 3 and continue in state ''S19''
//
// ... and so on. The current state is in the denominator and the follow-up state in the numerator.
// Any prime factors 2, 3, 5, 7 found in the numerator are incremented but if found in the denominator then they are decremented.
//
// That state machine is substantially faster than the previously mentioned ''enumerate()'' function.
// It needs about 10 seconds to find the first 500 prime numbers but then becomes slower and slower (because the number of steps seems to grow exponentially).
//
// However, I saw a few patterns in the output of ''enumerate()'' that allow me to "merge" states:
// depending on the current state `s_i` and the exponents of 2, 3, 5, 7, the follow-up state `s_{i+1}` might return to the previous state `s_i` (while modifying the exponents of 2, 3, 5, 7).
// Look for the ''#ifdef OPTIMIZE'' pragmas to locate my optimizations.
//
// The first optimization can be found in state ''S11'':
// - if the exponent of 3 is positive (''three > 0''), then it's decremented and followed by state ''S29''
// - state ''S29'' increments the exponent of 7 and returns to state ''S11''
// ==> that's a simple loop which simplifies to ''seven += three; three = 0;''
// ==> the total number of steps would be ''2 * three'' (visit ''S11'' and ''S29'' once per iteration)
// ==> this optimization saves about 1/3 of all steps (143,165,562 iterations to count the 213,945,763 steps for the first 100 primes)
//
// The second optimization can be found in state ''S13'':
// - if the exponents of 5 and 7 are both positive (''seven > 0 && five > 0''), then both are decremented and followed by state ''S17''
// - state ''S17'' increments the exponents of 2 and 3, decrements the exponent of 5 and returns to state ''S13''
// ==> that's another loop which stops whenever ''five'' or ''seven'' becomes zero
// ==> there are ''std::min(five, seven)'' iterations of that loop and each requires 2 steps
// ==> saves about 1/3 (143,263,171 iterations to process the 213,945,763 steps for the first 100 primes)
// ==> saves about 2/3 together with the first optimization (72,482,969 iterations to count the 213,945,763 steps for the first 100 primes)
//
// I was surprised that these two optimization are sufficient to solve the problem (10001th prime):
// the result is a number with 16 digits and found in about a minute but according to my logging the program processed 513,273,040,838,264 iterations.
// That's impossible - the code runs on a modest Core i7 CPU which is clocked somewhere between 3 and 4 GHz (depending on overall load).
// Even the best-case scenario would mean that roughly `250 * 10^9` CPU cycles were "burnt" while processing `513 * 10^12` iterations - that's 2000 iterations per CPU cycle !!!
//
// Clearly the GCC compiler found some optimization that I missed. And after a few minutes I stumbled across it (and it's so simple I wonder why I didn't spot it earlier).
// The third optimization was hidden in state ''S19'' and its structure is similar to the first optimization:
// - if the exponent of 2 is positive (''two > 0''), then it's decremented and followed by state ''S23''
// - state ''S23'' increments the exponent of 5 and returns to state ''S19''
// ==> that's a simple loop which simplifies to ''five += two; two = 0;''
// ==> the total number of steps would be ''2 * two'' (visit ''S19'' and ''S23'' once per iteration)
// ==> saves about 1/3 (143,313,579 iterations to process the 213,945,763 steps for the first 100 primes)
//
// And now comes the incredible part:
// if all three optimzations are enabled simultaneously then the total number of iterations needed to figure out the number of steps quickly drops to __less than 1%__:
// 1,850,784 iterations are sufficient to process the 213,945,763 steps for the first 100 primes (that's 0.865%, or put in another way, I saved 99.135%).
// And `76 * 10^9` iterations to know that it takes `1.5 * 10^15` steps (i.e. saved __99.9950278%__) to find the 10001th prime.
// By the way: that's only 2 CPU cycles per iteration, so there's still "something going on" in the GCC optimizer ...
//
// The execution time didn't drop when I added that third optimization (because GCC discovered it anyway)
// but other C++ compilers with less smart optimizer should now be able to emit reasonably fast code, too.
//
// # Note
// Another tough lesson in how modern software can almost outsmart a human being ...
//
// The source code doesn't need anything from my [toolbox](/toolbox/) and is one of the longest with "original" code written specificly to solve only one problem.
//
// # Alternative
// The Project Euler forum is full of other optimizations which aren't as simple as mine.
// The fastest programs solve the problem in less than a second.

#include <iostream>
#include <iomanip>
#include <algorithm>

// slowly step through all iterations until enough primes are found; display the exponents in each step
unsigned long long enumerate(unsigned int numPrimes, bool displaySteps = true)
{
  const auto NumFractions = 14;
  const auto NumExponents = 10;
  const char Fractions[NumFractions][NumExponents] =
  {
    // represent fractions as exponents of these primes:
    //          2,  3,  5,  7, 11, 13, 17, 19, 23, 29
    // 17/91 =             1/7   *1/13*17
             {  0,  0,  0, -1,  0, -1, +1,  0,  0,  0 },
    // 78/85 =  2 * 3 *1/5        *13*1/17
             { +1, +1, -1,  0,  0, +1, -1,  0,  0,  0 },
    // 19/51 =     1/3               *1/17*19
             {  0, -1,  0,  0,  0,  0, -1, +1,  0,  0 },
    // 23/38 = 1/2                       *1/19*23
             { -1,  0,  0,  0,  0,  0,  0, -1, +1,  0 },
    // 29/33 =     1/3       *1/11                *29
             {  0, -1,  0,  0, -1,  0,  0,  0,  0, +1 },
    // 77/29 =              7* 11                *1/29
             {  0,  0,  0, +1, +1,  0,  0,  0,  0, -1 },
    // 95/23 =          5                 *19*1/23
             {  0,  0, +1,  0,  0,  0,  0, +1, -1,  0 },
    // 77/19 =              7* 11        *1/19
             {  0,  0,  0, +1, +1,  0,  0, -1,  0,  0 },
    //  1/17 =                       *1/17
             {  0,  0,  0,  0,  0,  0, -1,  0,  0,  0 },
    // 11/13 =                 11*1/13
             {  0,  0,  0,  0, +1, -1,  0,  0,  0,  0 },
    // 13/11 =                1/11*13
             {  0,  0,  0,  0, -1, +1,  0,  0,  0,  0 },
    // 15/2  = 1/2* 3 * 5
             { -1, +1, +1,  0,  0,  0,  0,  0,  0,  0 },
    //  1/7  =             1/7
             {  0,  0,  0, -1,  0,  0,  0,  0,  0,  0 },
    // 55/1  =          5     *11
             {  0,  0, +1,  0, +1,  0,  0,  0,  0,  0 }
  };

  // seed = 2 = 2^1 => set first exponent to 1
  int current[NumExponents] = { +1, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  // count number of steps required to find that prime
  unsigned long long steps = 0;

  // number of primes encountered along the way
  unsigned int numFound = 0;
  while (numFound < numPrimes)
  {
    // print current step
    if (displaySteps)
    {
      std::cout << "step " << std::setw(3) << steps << ": ";
      for (auto x : current)
        // replace zeros by dashes to ease "pattern detection"
        std::cout << std::setw(2) << (x == 0 ? "-" : std::to_string(x)) << " ";
      std::cout << std::endl;
    }

    // apply first rule that produces an integer
    for (const auto& fraction : Fractions)
    {
      bool matchingFraction = true;
      // no exponent should become negative after applying a rule
      for (auto exponent = 0; exponent < NumExponents; exponent++)
        matchingFraction &= (current[exponent] + fraction[exponent]) >= 0;

      // yes, that rule rules !
      if (matchingFraction)
      {
        // just add exponents
        for (auto exponent = 0; exponent < NumExponents; exponent++)
          current[exponent] += fraction[exponent];
        // no further rules
        break;
      }
    }

    steps++;

    // one more prime ?
    bool isPrime = true;
    // all but the first exponent are zero
    for (auto exponent = 1; exponent < NumExponents; exponent++)
      isPrime &= current[exponent] == 0;
    if (isPrime) // only 2s, ignore seed value (2^1)
    {
      numFound++;
      if (displaySteps)
        std::cout << "prime " << current[0] << " @ step " << steps << std::endl;
    }
  }

  return steps;
}

// treat the FRACTRAN sequence as a state machine
unsigned long long search(unsigned int numPrimes)
{
  // and manually optimize a few states (GCC discovers a few optimizations on its own)
#define OPTIMIZED

  // at most one exponent a,b,c,d,e,f of 11^a, 13^b, 17^c, 19^d, 23^e, 29^f can be one
  // => seven states
  enum State { S_, S11, S13, S17, S19, S23, S29 };
  State state = S_; // default state

  // the exponents of 2,3,5,7 can be any non-negative number
  auto two   = 1;
  auto three = 0;
  auto five  = 0;
  auto seven = 0;

  unsigned long long steps = 0;
  unsigned long long iterations = 0; // to measure how efficient my state optimizations are
  unsigned int    numFound = 0;
  while (true) // exit-condition in state S_
  {
    iterations++;

    switch (state)
    {
      case S_:
        if (three == 0 && five == 0 && seven == 0 && steps > 0)
        {
          numFound++;
          //std::cout << "prime " << two << "/" << numFound << " @ " << steps
          //          << " (" << iterations << " iterations => " << 100.0 * iterations / steps << "%)" << std::endl;
          if (numFound == numPrimes)
            return steps;
        }

        // fraction L: 15/2 = 3*5 / 2
        if (two > 0)
        {
          two--; three++; five++;
          // could be optimized, too:
          //steps += two; three += two; five += two; two = 0; continue:
          break;
        }

        // fraction M: 1/7
        if (seven > 0)
        {
          // could be optimized, too
          //steps += seven; seven = 0; continue;
          seven--;
          break;
        }

        // fraction N: 55/1 = 5 * 11
        five++;
        state = S11;
        break;

      case S11:
        // fraction E: 29/33 = 29 / (3*11)
        if (three > 0)
        {
#ifdef OPTIMIZED
          // S11 <=> S29
          // S29 performs seven++ and immediately returns to the current state S11
          steps += 2 * three;
          seven += three;
          three  = 0;
          continue; // to avoid step++ after the switch-block
#endif
          three--;
          state = S29;
          break;
        }

        // fraction K: 13/11
        state = S13;
        break;

      case S13:
        // fraction A: 17/91 = 17 / (7*13)
        if (seven > 0)
        {
#ifdef OPTIMIZED
          // S13 <=> S17:
          // as long as five and seven are not zero both are decremented by one
          // whereas two and three are incremented by one
          if (five > 0)
          {
            auto smallest = std::min(five, seven);
            steps += 2 * smallest;
            two   += smallest;
            three += smallest;
            five  -= smallest;
            seven -= smallest;
            continue; // to avoid step++ after the switch-block
          }
#endif
          seven--;
          state = S17;
          break;
        }

        // fraction J: 11/13
        state = S11;
        break;

      case S17:
        // fraction B: 78/85 = 2*3*13 / (5*17)
        if (five > 0)
        {
          five--; two++; three++;
          state = S13;
          break;
        }
        // fraction C: 19/51 = 19 / (3*17)
        if (three > 0)
        {
          three--;
          state = S19;
          break;
        }

        // fraction I: 1/17
        state = S_;
        break;

      case S19:
        // fraction D: 23/38 = 23 / (2*19)
        if (two > 0)
        {
#ifdef OPTIMIZED
          // S19 <=> S23 (apparently automatically detected by GCC at optimization level -O3, too)
          // two is decremented until it's zero while five is incremented
          steps += 2 * two;
          five  += two;
          two    = 0;
          continue; // to avoid step++ after the switch-block
#endif
          two--;
          state = S23;
          break;
        }

        // fraction H: 77/19 = 7*11 / 19
        seven++;
        state = S11;
        break;

      case S23:
        // fraction G: 95/23 = 5*19 / 23
        five++;
        state = S19;
        break;

      case S29:
        // fraction F: 77/29 = 7*11 / 29
        seven++;
        state = S11;
        break;
    }

    // next iteration
    steps++;
  }
}

int main()
{
  unsigned int numPrimes = 10001;
  std::cin >> numPrimes;

  // visualize steps
  //enumerate(numPrimes, true);

  // fast algorithm to solve the problem
  std::cout << search(numPrimes) << std::endl;
  return 0;
}

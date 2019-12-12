// ////////////////////////////////////////////////////////
// # Title
// Paper sheets of standard sizes: an expected-value problem
//
// # URL
// https://projecteuler.net/problem=151
// http://euler.stephan-brumme.com/151/
//
// # Problem
// A printing shop runs 16 batches (jobs) every week and each batch requires a sheet of special colour-proofing paper of size A5.
//
// Every Monday morning, the foreman opens a new envelope, containing a large sheet of the special paper with size A1.
//
// He proceeds to cut it in half, thus getting two sheets of size A2. Then he cuts one of them in half to get two sheets of size A3
// and so on until he obtains the A5-size sheet needed for the first batch of the week.
//
// All the unused sheets are placed back in the envelope.
//
// ![sheet sizes](p151.gif)
//
// At the beginning of each subsequent batch, he takes from the envelope one sheet of paper at random.
// If it is of size A5, he uses it. If it is larger, he repeats the 'cut-in-half' procedure until he has what he needs and any remaining sheets are always placed back in the envelope.
//
// Excluding the first and last batch of the week, find the expected number of times (during each week) that the foreman finds a single sheet of paper in the envelope.
//
// Give your answer rounded to six decimal places using the format x.xxxxxx .
//
// # Solved by
// Stephan Brumme
// May 2017
//
// # Algorithm
// I solved this problem twice. My first attempt was running a Monte-Carlo simulation - and never achieved enough precision. That code is still present below.
//
// My second (and successful) attempt ''evaluate''s the exact probabilities of each possible combination.
// Its parameter is a simple container with 5 elements, where the first represents the number of A1 sheets, the second element stands for the number of A2 sheets, etc.
// The initial parameter is ''{ 1, 0, 0, 0, 0 }''. If at some point we have no A5, no A4, 2x A3, no A4 and 1x A5 then it would be ''{ 0, 0, 2, 0, 1 }''.
//
// Each sheet size is picked and the function calls itself recursively. The probability for picking a single sheet is ''1 / numSheets''.
// If there are multiple sheets of the same sheet size ''i'' then the probability increases to ''sheets[i] / numSheets''.

#include <iostream>
#include <vector>
#include <iomanip>

// return number of batch where a single sheet is in the envelope
// parameter is a list of the number of available sheets, starting with A1
// e.g. 0x A5, 0x A4, 2x A3, 0x A4, 1x A5 is encoded as { 0, 0, 2, 0, 1 }
double evaluate(std::vector<unsigned int> sheets)
{
  // count sheets
  unsigned int numSheets = 0;
  for (auto s : sheets)
    numSheets += s;

  // a single sheet ?
  double single = 0;
  if (numSheets == 1)
  {
    // only one sheet of A5 left => last batch
    if (sheets.back() == 1)
      return 0;

    // except if the single sheet is A1 (first batch)
    if (sheets.front() == 0)
      single = 1;
  }

  // process all sheet sizes
  for (size_t i = 0; i < sheets.size(); i++)
  {
    if (sheets[i] == 0)
      continue;

    auto next = sheets;
    // use one piece of the sheet size
    next[i]--;
    // cut it into smaller pieces
    for (size_t j = i + 1; j < next.size(); j++)
      next[j]++;

    // how likely do we pick this sheet size ?
    double probability = sheets[i] / (double)numSheets;
    // analyze next batch
    single += evaluate(next) * probability;
  }

  return single;
}

// I kept this code for historical reasons ... it's not called in main()
double montecarlo()
{
  // more rounds improve precision
  const unsigned int NumRounds = 1000000;
  // different seeds yields different results ...
  srand(111);

  // how often a single sheet is observed
  unsigned int singleSheet = 0;
  for (unsigned int round = 0; round < NumRounds; round++)
  {
    // 1 => DIN A1, 2 => A2, ... 5 => A5
    // this stack of sheets may contain some sizes multiple times
    const unsigned int SheetSizes = 5;
    unsigned int sheets[SheetSizes] = { 1,0,0,0,0 }; // one sheet A1 on Monday morning
    unsigned int numSheets = 1;

    // until all sheets are used (on Friday afternoon)
    while (numSheets > 0)
    {
      // a single sheet ?
      if (numSheets == 1)
        singleSheet++;

      // pick a random sheet
      unsigned int pick = rand() % numSheets;
      unsigned int current = 0;
      // select sheet size
      while (pick >= sheets[current])
        pick -= sheets[current++];
      // and remove one sheet
      sheets[current]--;

      // reduce total number of sheets, too
      numSheets--;

      // if the current sheet is larger than A5 then cut it into smaller sheets
      while (++current < SheetSizes)
      {
        sheets[current]++;
        numSheets++;
      }
    }

    // don't count the first and last batch (always one sheet)
    singleSheet -= 2;
  }

  return singleSheet / (double)NumRounds;
}

int main()
{
  std::cout << std::fixed << std::setprecision(6);

  // start with one A1 sheet
  std::cout << evaluate({ 1,0,0,0,0 }) << std::endl;

  // my first approach was using a Monte-Carlo simulation but it converges too slowly
  //std::cout << montecarlo() << std::endl;

  return 0;
}

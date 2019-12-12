// ////////////////////////////////////////////////////////
// # Title
// Protein folding
//
// # URL
// https://projecteuler.net/problem=300
// http://euler.stephan-brumme.com/300/
//
// # Problem
// In a very simplified form, we can consider proteins as strings consisting of hydrophobic (H) and polar (P) elements, e.g. HHPPHHHPHHPH.
// For this problem, the orientation of a protein is important; e.g. HPP is considered distinct from PPH. Thus, there are 2n distinct proteins consisting of n elements.
//
// When one encounters these strings in nature, they are always folded in such a way that the number of H-H contact points is as large as possible, since this is energetically advantageous.
// As a result, the H-elements tend to accumulate in the inner part, with the P-elements on the outside.
// Natural proteins are folded in three dimensions of course, but we will only consider protein folding in two dimensions.
//
// The figure below shows two possible ways that our example protein could be folded (H-H contact points are shown with red dots).
// ![protein folding](protein.gif)
// The folding on the left has only six H-H contact points, thus it would never occur naturally.
// On the other hand, the folding on the right has nine H-H contact points, which is optimal for this string.
//
// Assuming that H and P elements are equally likely to occur in any position along the string, the average number of H-H contact points in an optimal folding of a random protein string of length 8 turns out to be 850 / 28=3.3203125.
//
// What is the average number of H-H contact points in an optimal folding of a random protein string of length 15?
// Give your answer using as many decimal places as necessary for an exact result.
//
// # Solved by
// Stephan Brumme
// August 2017
//
// # Algorithm
// The core idea is:
// - generate all possible layouts (see ''search()'')
// - for each layout: "fill" it with all possible proteins and count contact points (see ''optimize'')
//
// ''search()'' uses a 30x30 grid where the first element is placed at (15,15).
// Since all rotated and mirrored proteins yield the same number of contact points I also place the second element at (16,15).
// In each subsequent step all possible continuations are tried until 15 elements are placed on the grid.
// I decided to label those elements ''a'', ''b'', ..., ''o'' (equivalent to ''First+0'', ''First+1'', ..., ''First+14''). ''Empty'' cells are with a zero.
//
// When 15 elements are stored in ''grid'', then the function ''optimize'' finds all connections between cells.
// E.g. in the example image there is a contact between 0 and 14 (the first and the last element of the protein).
// It deliberately ignores immediate neighbors, such as 0 and 1 because they always have a contact independent of the protein's layout.
// This accelerates the analysis because those contacts of immediate neighbors can be pre-computed efficiently in ''main()'' and reduce the time spent in ''optimize'' considerably.
//
// Several proteins share the same layout and thus have the same number of contact points.
// Even more, several protein are not identical but "similar": their layout varies but they have contact points at the same locations.
// My "cache" named ''visited'' detects those identical and those similar proteins to speed up the process.
//
// The contact points are based on bit masks: if element 3 and element 7 have a contact, then the bitmask will be ''(1 << 3) | (1 << 7)''.
// When I iterate over all proteins then there will be ''1 << length'' combinations. I decided that a zero bit refers to a hydrophobic element.
// Thus only if ''(protein & contactMask) == 0'' then both elements are hydrophobic.
//
// # Note
// The grid is 30x30 because when I start in the middle of the grid then a protein which is a straight line could extend to (29,15).
// Obviously this layout has only contact points between immediate neighboring elements and is not an optimal choice for most proteins.
// On the other hand, proteins with many contact points are often very compact.
// If I don't search the whole grid for potential contact points then I could miss a few.
// As it turns out, missing contacts doesn't matter for sub-optimal layout - as long as I correctly process all optimal layouts.
// And it's even okay to get a few optimal layouts wrong - as long as there as multiple optimal layout and I don't mess up all of them.
// I played around with the value ''ignoreStraight'': reducing the search window to the area (9,9) - (21,21) still produces the correct result.
// This is only true for proteins with 15 elements, therefore I skip this optimization for other protein lengths.
//
// Placing the second element always at (16,15) is possible because of the massive amount of rotated and mirrored proteins.
// A similar optimization is to call ''optimize'' only if the last element's ''y >= Center'' (which is 15):
// For each protein which the last element at (x,y) there will be a mirrored protein ended at (x, 2*Center - y).
//
// I tried to implement another optimization:
// for each protein with a sequence abc..mno there is a flipped protein onm...bca. Thus I could skip about half of all protein sequences.
// In the end I couldn't achieve a speed-up and removed the code.
//
// I liked this problem !

#include <iostream>
#include <iomanip>
#include <vector>
#include <set>

// operate on a 30x30 grid, each protein starts at (Center, Center)
const unsigned int GridSize = 30;
const unsigned int Center   = GridSize / 2;
typedef char Grid[GridSize][GridSize];

// indicator for an empty cell
const char Empty = 0;
// protein's first element is 'a', then 'b', ..., 'e'
const char First = 'a';

// optimized number of hydrophobic contacts per protein
std::vector<unsigned int> best;
// number of direct contacts between immediate neighbors (minimum value, independent of folding)
std::vector<unsigned int> direct;
// keep track of past contact points sequences (detect duplicates / strong similiarities)
std::set<std::vector<unsigned int>> visited;

// check each protein whether the current grid is better than anything seen before
void optimize(unsigned int length, const Grid& grid)
{
  // list of neighbors, each pair is stored as a bitmask
  std::vector<unsigned int> contacts;

  // count how many occupied cells I've seen
  unsigned int seen = 0;

  // extremely straight foldings can't be optimal because they have only contacts between directs neighbors
  unsigned int ignoreStraight = 9; // empirical value
  if (length != 15)
    ignoreStraight = 0;

  // walk through the grid
  for (unsigned int i = ignoreStraight; i + 1 + ignoreStraight < GridSize; i++)
    for (unsigned int j = ignoreStraight; j + 1 + ignoreStraight < GridSize; j++)
    {
      // already visited the entire protein ?
      if (seen + 1 >= length)
        break;

      // ignore empty cells
      if (grid[i][j] == Empty)
        continue;

      // one more element visited
      seen++;
      // get its index (0..length-1)
      auto from = grid[i][j] - First;

      // check right neighbor
      if (grid[i + 1][j] != Empty)
      {
        auto to = grid[i + 1][j] - First;
        if (from != to + 1 && from + 1 != to)
          contacts.push_back((1 << from) | (1 << to));
      }
      // check bottom neighbor
      if (grid[i][j + 1] != Empty)
      {
        auto to = grid[i][j + 1] - First;
        if (from != to + 1 && from + 1 != to)
          contacts.push_back((1 << from) | (1 << to));
      }
    }

  // no contacts at all ? (besides direct contact)
  if (contacts.empty())
    return;

  // already had a similar folding ? (same contact points)
  if (visited.count(contacts) != 0)
    return;
  visited.insert(contacts);

  // for each protein check whether more hydrophobic contacts exist
  auto numProteins = 1 << length;
  for (auto protein = 0; protein < numProteins; protein++)
  {
    // at least all direct contacts
    auto found = direct[protein];

    // can't beat the previous record ?
    if (found + contacts.size() <= best[protein])
      continue;

    // count other contact points
    for (auto contactMask : contacts)
    {
      // are both neighbors hydrophobic ?
      if ((protein & contactMask) == 0)
        found++;
    }

    // better than before ?
    if (best[protein] < found)
      best[protein] = found;
  }
}

// generate recursive all possible layouts
void search(unsigned int current, unsigned int length, Grid& grid, unsigned int x, unsigned int y)
{
  // protein completely placed on grid ?
  if (current == length)
  {
    // don't check layouts mirrored along y-axis
    if (y >= Center)
      optimize(length, grid);

    // done
    return;
  }

  // try all possible continuations
  // go left
  if (grid[x - 1][y] == Empty)
  {
    grid[x - 1][y] = current + First;
    search(current + 1, length, grid, x - 1, y);
    grid[x - 1][y] = Empty;
  }

  // go right
  if (grid[x + 1][y] == Empty)
  {
    grid[x + 1][y] = current + First;
    search(current + 1, length, grid, x + 1, y);
    grid[x + 1][y] = Empty;
  }

  // go down
  if (grid[x][y - 1] == Empty)
  {
    grid[x][y - 1] = current + First;
    search(current + 1, length, grid, x, y - 1);
    grid[x][y - 1] = Empty;
  }

  // go up
  if (grid[x][y + 1] == Empty)
  {
    grid[x][y + 1] = current + First;
    search(current + 1, length, grid, x, y + 1);
    grid[x][y + 1] = Empty;
  }
}

int main()
{
  unsigned int length = 15;
  std::cin >> length;

  // allocate memory
  auto numProteins = 1 << length;
  direct.resize(numProteins, 0);
  best  .resize(numProteins, 0);

  // count direct neighboring hydrophobic elements in each protein
  for (auto protein = 0; protein < numProteins; protein++)
  {
    for (unsigned int i = 0; i < length - 1; i++)
      if ((protein & (3 << i)) == 0) // 3 => bitmask of two bits
        direct[protein]++;

    best[protein] = direct[protein];
  }

  // create an empty grid
  Grid grid;
  for (unsigned int x = 0; x < GridSize; x++)
    for (unsigned int y = 0; y < GridSize; y++)
      grid[x][y] = Empty;

  // always start in the middle
  unsigned int x = Center;
  unsigned int y = Center;
  grid[x][y] = First;
  // due to symmetries I can also place the second element
  x++;
  grid[x][y] = First + 1;

  // generate all layouts and then call optimize()
  search(2, length, grid, x, y);

  // count optimized contact points
  unsigned int sum = 0;
  for (auto x : best)
    sum += x;
  // display average
  std::cout << std::setprecision(14)
            << sum / double(best.size()) << std::endl;
  return 0;
}

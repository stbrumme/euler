// ////////////////////////////////////////////////////////
// # Title
// Connectedness of a network
//
// # URL
// https://projecteuler.net/problem=186
// http://euler.stephan-brumme.com/186/
//
// # Problem
// Here are the records from a busy telephone system with one million users:
//
// ||   5   ||    5   ||    5   ||
// ||!RecNr ++ Caller ++ Called ||
// ||   1   ++ 200007 ++ 100053 ||
// ||   2   ++ 600183 ++ 500439 ||
// ||   3   ++ 600863 ++ 701497 ||
// ||  ...  ++   ...  ++   ...  ||
//
// The telephone number of the caller and the called number in record `n` are `Caller(n) = S_{2n-1}` and `Called(n) = S_{2n}`
// where `S_{1,2,3,...}` come from the "Lagged Fibonacci Generator":
//
// For `1 <= k <= 55`, `S_k = [100003 - 200003k + 300007k^3] mod 1000000`
// For `56 <= k`, `S_k = [S_{k-24} + S_{k-55}] mod 1000000`
//
// If `Caller(n) = Called(n)` then the user is assumed to have misdialled and the call fails; otherwise the call is successful.
//
// From the start of the records, we say that any pair of users `X` and `Y` are friends if `X` calls `Y` or vice-versa.
// Similarly, `X` is a friend of a friend of `Z` if `X` is a friend of `Y` and `Y` is a friend of `Z`; and so on for longer chains.
//
// The Prime Minister's phone number is 524287. After how many successful calls, not counting misdials,
// will 99% of the users (including the PM) be a friend, or a friend of a friend etc., of the Prime Minister?
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// There is a pretty descent description of the disjoint-set algorithm on Wikipedia ( https://en.wikipedia.org/wiki/Disjoint-set_data_structure ).
//
// For each phone number (called ''node'' in my source code) I track its "leader" and the number of friends.
// A "leader" is an arbitrary person who represents the root of the current subtree. He/she doesn't have to be the prime minister.
// In fact, the prime minister doesn't have to be a leader in hi/her subtree.
// All persons in the same subtree share the same leader. And only the leader has the correct count of persons in that subtree.
//
// Whenever person A calls person B then I track down their respective leaders.
// If they share the same leader then nothing has to changed.
// If they have different leaders then I merge both disjoint subtrees. The leader of the larger subtree will be the new leader of the joint subtree.
//
// Whenever the subtree of the prime minister contains more than 990000 members (99% of one million) the algorithm has to finish.
//
// My ''find'' routine was optimized for speed:
// - Wikipedia recommends a recursive solution but I wrote an iterative
// - I "flatten" each tree: whenever ''find'' looks for the leader of ''x'' (which might take a few iterations) then I update x's node to directly point at that leader
// ==> subsequent calls to ''find'' will terminate after one iteration
//
// The lagged Fibonacci generator needs to keep at least the last 55 values.
// I keep a few more to avoid too frequent memory allocations and/or data moves.
//
// # Note
// Even though it might be obvious to others, I was surprised when I saw that the actually phone number of the prime minister
// doesn't really matter: the algorithm will produce the exactly same result for 99% of all phone numbers.

#include <iostream>
#include <vector>

// a single phone number
struct Node
{
  // choose one person to be the leader of a subtree (not necessarily the prime minister)
  unsigned int leader;
  // number of persons in the current subtree, value only valid for the leader
  unsigned int count;
};

// all persons / phone numbers
std::vector<Node> nodes;

// return leader of current subtree
unsigned int find(unsigned int id)
{
  auto current = id;
  while (true)
  {
    auto leader = nodes[current].leader;
    // stop if current node is its own leader
    if (leader == current)
    {
      // "flatten" tree, replace former leader by actual leader
      if (nodes[id].leader != leader)
        nodes[id].leader = leader;

      return leader;
    }

    // keep going, leader not found yet
    current = leader;
  }
}

// merge two subtrees
void merge(unsigned int x, unsigned int y)
{
  // find leaders
  auto rootX = find(x);
  auto rootY = find(y);

  // same tree ?
  if (rootX == rootY)
    return; // nothing left to do

  // merge smaller tree into larger tree
  if (nodes[rootX].count >= nodes[rootY].count)
  {
    nodes[rootX].count += nodes[rootY].count;
    nodes[rootY].leader = rootX;
  }
  else
  {
    nodes[rootY].count += nodes[rootX].count;
    nodes[rootX].leader = rootY;
  }
}

// lagged Fibonacci generator
unsigned int generate()
{
  // store recent output
  static std::vector<unsigned int> history;

  unsigned int current;
  // one of the first 55 values
  if (history.size() < 55)
  {
    auto k = history.size() + 1;
    current = (300007ULL * k*k*k - 200003 * k + 100003) % 1000000;
  }
  else
  {
    current = (history[history.size() - 55] + history[history.size() - 24]) % 1000000;
    // throw away history if too old
    const size_t RemoveAtOnce = 100;
    if (history.size() > 55 + RemoveAtOnce)
      history.erase(history.begin(), history.begin() + RemoveAtOnce);
  }

  history.push_back(current);
  return current;
}

int main()
{
  // prime minister's phone 6-digit phone number
  unsigned int phone      = 524287;
  // 99% should be friends or friends-of-friends of the prime minister
  unsigned int percentage =     99;
  std::cin >> phone >> percentage;

  // create one million independent nodes
  for (unsigned int i = 0; i < 1000000; i++)
    nodes.push_back({ i, 1 });

  // connect nodes
  unsigned int calls = 0;
  while (nodes[find(phone)].count < 1000000 * percentage / 100)
  {
    // randomly generate a call
    auto from = generate();
    auto to   = generate();
    // misdialled ?
    if (from == to)
      continue;

    // valid call, connect subtrees
    calls++;
    merge(from, to);
  }

  // display result
  std::cout << calls << std::endl;
  return 0;
}

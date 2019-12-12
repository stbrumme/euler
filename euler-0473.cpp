// ////////////////////////////////////////////////////////
// # Title
// Phigital number base
//
// # URL
// https://projecteuler.net/problem=473
// http://euler.stephan-brumme.com/473/
//
// # Problem
// Let `phi` be the golden ratio: `phi = 1 + 5 sqrt{2}`.
// Remarkably it is possible to write every positive integer as a sum of powers of `phi`
// even if we require that every power of `phi` is used at most once in this sum.
// Even then this representation is not unique.
// We can make it unique by requiring that no powers with consecutive exponents are used and that the representation is finite.
// E.g: `2 = phi + phi^-2` and `3 = phi^2 + phi^-2`
//
// To represent this sum of powers of `phi` we use a string of 0's and 1's with a point to indicate where the negative exponents start.
// We call this the representation in the phigital numberbase.
// So `1=1_{phi}`, `2=10.01_{phi}`, `3=100.01_{phi}` and `14=100100.001001_{phi}`
//
// The strings representing 1, 2 and 14 in the phigital number base are palindromic, while the string representing 3 is not.
// (the phigital point is not the middle character).
//
// The sum of the positive integers not exceeding 1000 whose phigital representation is palindromic is 4345.
//
// Find the sum of the positive integers not exceeding `10^10` whose phigital representation is palindromic.
//
// # Solved by
// Stephan Brumme
// September 2017
//
// # Algorithm
// According to the problem statement, two 1s must be separated by at least one zero.
// When I looked at the found numbers, there were always at least __TWO__ zeros between two 1s.
//
// In ''search()'' I generate only one half of the palindrome: the right-hand side. Obviously the left-hand side must be the mirrored version.
// If ''current'' is the sum of a few `phi^n` then I add `phi^{n+2}`, then `phi^{n+3}`, `phi^{n+4}`, ... until the sum gets too large.
//
// Unfortunately, I can't work around precision issues and need GCC's ''__float128'' extension.
// Computing `phi^n` on-the-fly caused even more precision problems - so I precomputed more accurate values with Wolfram Alpha (see large table in ''phipow()''.
//
// # Alternative
// I didn't realize that the golden ratio can be found in Fibonacci numbers, too (see https://en.wikipedia.org/wiki/Fibonacci_number ):
// `F_n = dfrac{phi^n - (-phi)^{-n}}{2 phi - 1}`
// This way all my precision problem can be avoided - and even more important, a solution can be found in a few milliseconds.
//
// # Note
// Working with GCC's ''__float128'' extension requires the ''std=gnu++11'' compiler switch.

#include <iostream>
#include <vector>
#include <cmath>

typedef __float128 Number; // G++ only !
// acceptable error
const Number Epsilon = 1e-15Q;

// ln(10^10)/ln((1+sqrt(5))/2) is about 47.85, so at most 48 exponents
unsigned int maxExponent = 48;

// return phi^exponent
Number phipow(int exponent)
{
  // golden ratio is (1 + sqrt(5)) / 2
  //const Number Phi = 1.618033988749894848204586834365638117720309179805762862135Q;

  // precompute values Phi^0 ... Phi^48
  static const Number precomputed[] =
  {
    1,
    1.618033988749894848204586834365638117720309179805762862135Q,
    2.618033988749894848204586834365638117720309179805762862135Q,
    4.236067977499789696409173668731276235440618359611525724270Q,
    6.854101966249684544613760503096914353160927539417288586406Q,
    11.09016994374947424102293417182819058860154589902881431067Q,
    17.94427190999915878563669467492510494176247343844610289708Q,
    29.03444185374863302665962884675329553036401933747491720776Q,
    46.97871376374779181229632352167840047212649277592102010484Q,
    76.01315561749642483895595236843169600249051211339593731260Q,
    122.9918693812442166512522758901100964746170048893169574174Q,
    199.0050249987406414902082282585417924771075170027128947300Q,
    321.9968943799848581414605041486518889517245218920298521475Q,
    521.0019193787254996316687324071936814288320388947427468775Q,
    842.9988137587103577731292365558455703805565607867725990250Q,
    1364.000733137435857404797968963039251809388599681515345902Q,
    2206.999546896146215177927205518884822189945160468287944927Q,
    3571.000280033582072582725174481924073999333760149803290830Q,
    5777.999826929728287760652380000808896189278920618091235757Q,
    9349.000106963310360343377554482732970188612680767894526588Q,
    15126.99993389303864810402993448354186637789160138598576234Q,
    24476.00004085634900844740748896627483656650428215388028893Q,
    39602.99997474938765655143742344981670294439588353986605128Q,
    64079.00001560573666499884491241609153951090016569374634021Q,
    103681.9999903551243215502823358659082424552960492336123914Q,
    167761.0000059608609865491272482819997819661962149273587317Q,
    271442.9999963159853080994095841479080244214922641609711232Q,
    439204.0000022768462946485368324299078063876884790883298549Q,
    710646.9999985928316027479464165778158308091807432493009781Q,
    1149851.00000086967789739648324900772363719686922233763Q,
    1860497.99999946250950014442966558553946800604996558693Q,
    3010349.00000033218739754091291459326310520291918792456Q,
    4870846.99999979469689768534258017880257320896915351149Q,
    7881196.00000012688429522625549477206567841188834143605Q,
    12752042.9999999215811929115980749508682516208574949475Q,
    20633239.0000000484654881378535697229339300327458363836Q,
    33385281.9999999700466810494516446738021816536033313311Q,
    54018521.0000000185121691873052143967361116863491677147Q,
    87403802.9999999885588502367568590705382933399524990459Q,
    141422324.000000007071019424062073467274405026301666760Q,
    228826126.999999995629869660818932537812698366254165806Q,
    370248451.000000002700889084881006005087103392555832567Q,
    599074577.999999998330758745699938542899801758809998373Q,
    969323029.000000001031647830580944547986905151365830941Q,
    1568397606.99999999936240657628088309088670691017582931Q,
    2537720636.00000000039405440686182763887361206154166025Q,
    4106118242.99999999975646098314271072976031897171748957Q,
    6643838879.00000000015051539000453836863393103325914982Q,
    10749957121.999999999906976373147249098394250004976639Q // already too big: > 10^10
  };

  // lookup cached values
  if (exponent >= 0)
    return        precomputed[+exponent];
  else
    return 1.0Q / precomputed[-exponent];
}

// return phi^pos + phi^-(pos+1)
Number phipowBoth(unsigned int pos)
{
  // precompute phi^i + phi^-(i+1)
  // => those are the pairs in the palindromic representation
  static std::vector<Number> cache;
  if (cache.empty())
  {
    cache.push_back(phipow(0));
    for (int i = 1; i < 49; i++)
      cache.push_back(phipow(i) + phipow(-(i+1)));
  }

  return cache[pos];
}

// generate all strings where there are at least two zeros between two ones
unsigned long long search(Number limit, unsigned int exponent = 0, Number current = 0)
{
  // "out of bounds" ?
  if (current > limit)
    return 0;

  unsigned long long result = 0;

  // an integer ?
  auto rounded = round(current); // avoid rounding issues, add a small "buffer"
  auto diff = current - rounded;
  if (diff > -Epsilon && diff < +Epsilon)
    result += rounded;

  // it turns out at least two zeros between a pair of ones
  // go deeper by appending "001", "0001", "00001", etc.
  // => at least two zeros and a single one
  // except next to the dot: a zero on one side generates a zero on the other side, too (palindrome)
  if (exponent == 0)
    exponent++;
  else
    exponent += 2;

  // until phi^48 > 10^10
  while (exponent <= maxExponent)
  {
    // add phi^exponent (and its mirrored "twin" phi^-(exponent+1)
    result += search(limit, exponent+1, current + phipowBoth(exponent));
    exponent++;
  }

  return result;
}

int main()
{
  // 10^10
  unsigned long long limit = 10000000000ULL;
  std::cin >> limit;

  // find maximum length of one side of the palindrome
  maxExponent = 0;
  while (phipow(maxExponent) <= limit)
    maxExponent++;

  // only one phigital number can have a 1 next to the dot => that's the degenerated string "1"
  // all other strings must have zeros next to the dot bceause "no powers with consecutive exponents"
  std::cout << 1 + search(limit) << std::endl;
  return 0;
}

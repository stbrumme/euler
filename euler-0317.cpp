// ////////////////////////////////////////////////////////
// # Title
// Firecracker
//
// # URL
// https://projecteuler.net/problem=317
// http://euler.stephan-brumme.com/317/
//
// # Problem
// A firecracker explodes at a height of 100 m above level ground.
// It breaks into a large number of very small fragments, which move in every direction;
// all of them have the same initial velocity of 20 m/s.
//
// We assume that the fragments move without air resistance, in a uniform gravitational field with `g=9.81 m/s^2`.
//
// Find the volume (in `m^3`) of the region through which the fragments move before reaching the ground.
// Give your answer rounded to four decimal places.
//
// # Solved by
// Stephan Brumme
// July 2017
//
// # Algorithm
// The German Wikipedia page on trajectory ( https://de.wikipedia.org/wiki/Wurfparabel ) says that the formula of the hull is:
// (1) `y = dfrac{v^2}{2g} - dfrac{g x^2}{2v^2} + h_0`
//
// where `g = 9.81`, `h_0 = 100` and `v = 20`
// I couldn't find it on the English page ( https://en.wikipedia.org/wiki/Trajectory_of_a_projectile ), though.
//
// Splitting the formula into constant and variable parts:
// (2) `add = dfrac{v^2}{2g} + h_0`
// (3) `factor = dfrac{g}{2v^2}`
// (4) `y = factor * x^2 + add`
//
// The volume of a rotated body is
// (5) `V = pi * integral{(f(x))^2} dx`
//
// Unfortunately, equation (5) applies only to rotations around the x-axis.
// The fireworks are rotated around the y-axis, thus I have to find its inverse (swap `x` and `y`):
// (6) `x = factor * y^2 + add`
// (7) `factor * y^2 = x - add`
// (8) `y^2 = dfrac{x - add}{factor}`
//
// (9) `y = sqrt{dfrac{x - add}{factor}}`
//
// I replace equation (9) in (5):
// (10) `V = pi * integral{(sqrt{dfrac{x - add}{factor}})^2} dx`
//
// (11) `V = pi * integral{dfrac{x - add}{factor}} dx`
//
// (12) `V = dfrac{pi}{factor} * integral(x - add) dx`
//
// Integrating:
// (13) `integral_x(x - add) dx = dfrac{x^2}{2} - add * x`
//
// Only "points above ground" should be taken into consideration:
// - the integral starts at zero, `x_{low} = 0`
// - the integral ends at the highest point of any firework, which is in the centre, that means `x_{high} = f(0)` in (1) which is `x_{high} = add`
//
// Now all I have to do is computing (12) for both borders `x_{low}` and `x_{high}` of the integral:
// (14) `V = dfrac{pi}{factor} * (( dfrac{x_{high}^2}{2} - add * x_{high}) - (dfrac{x_{low}^2}{2} - add * x_{low} ))`
//
// It turns out that for ``x_{low} = 0`` most of the equation is zero and simplifies to:
// (15) `V = dfrac{pi}{factor} * (dfrac{x_{high}^2}{2} - add * x_{high})`
//
// `x_{high} = add` allows even further simplification:
// (16) `V = dfrac{pi}{factor} * (dfrac{add^2}{2} - add * add)`
// (17) `V = dfrac{pi}{factor} * dfrac{-add^2}{2}`
// (18) `V = -dfrac{pi}{2} * dfrac{add^2}{factor}`
//
// Equation 18 is used in my code. And in case you forgot what `add` and `factor` are:
// (19) `V = -dfrac{pi}{2} * dfrac{(dfrac{v^2}{2g} + h_0)^2}{dfrac{g}{2v^2}}`
//
// The result is basically found instantly without any memory consumption.
//
// # Alternative
// ... I'm a software engineer and felt that something was missing !
//
// Therefore I wrote a simple loop to integrate the initial formula (1) numerically (well, actually I used equation (4) which is the same):
// - slowly step along the x-axis and compute ''y = factor * x * x + add''
// - y will be monotoneously decreasing
// - each step that I take adds a small cylinder/slice to the volume (with ''sliceHeight = lastY - y'' and ''sliceVolume = pi * x * x * sliceHeight'')
//
// It's tricky to find the correct ''step'' which defines the distance between two consecutive x-values:
// - if ''step'' is too large, then rounding artifacts falsify the result
// - if ''step'' is too small, then the algorithm can be very slow and the volume because so tiny, that it can't be properly represented by ''double''
// ==> another type of rounding artifacts
//
// My code contains a dynamic adjustment of ''step'' after each step, depending on the previous slice volume.
// Nevertheless, I can't achieve a sufficient precision: two decimal places are no problem but I haven't found the
// optimal value for ''averageSliceVolume'' such that the correct result is displayed.
//
// To enable the numeric integration enable ''#define NUMERIC_INTEGRATION''.

#include <iostream>
#include <iomanip>

// nore than enough digits of pi ...
const double pi = 3.141592653589793238462;
// gravition (in m/s^2)
const double g  = 9.81;

int main()
{
  std::cout << std::fixed << std::setprecision(4);

  // velocity in m/s
  double velocity =  20;
  // height above ground (in m)
  double height   = 100;

  // user can adjust fireworks
  std::cin >> velocity >> height;

  // split formula into constant summand ...
  auto add = velocity * velocity / (2 * g) + height;
  // ... and factor
  auto factor = - g / (2 * velocity * velocity);

  // integral from 0 to "add" of pi * (0.5 * i * i - add * i) / factor
  // => it's zero at 0, just compute at "add"
  auto result = -0.5 * pi * add * add / factor;
  std::cout << result << std::endl;

//#define NUMERIC_INTEGRATION
#ifdef NUMERIC_INTEGRATION
  // accumulated volume so far
  double volume = 0;
  // distance between two consecutive x
  double step   = 0.00001;
  // current x
  double x      = 0;

  // y of previous iteration
  double lastY  = 0;

  // preferred size of a slice, adjust "step" accordingly
  double averageSliceVolume = 0.0001;

  while (true)
  {
    // evaluate formula (4)
    auto y = x * x * factor + add;
    // hit the ground ? ==> done
    if (y <= 0)
      break;

    // height of cylinder
    auto sliceHeight = lastY - y;
    // volume of cylinder
    auto sliceVolume = pi * x * x * sliceHeight;

    // add to accumulated volume
    volume += sliceVolume;

    // slice was too large / too small ? adjust distance to next x
    if (sliceVolume > averageSliceVolume)
      step /= 2;
    else
      step *= 2;

    // next iteration
    x      += step;
    lastY   = y;
  }

  std::cout << volume << std::endl;
#endif

  return 0;
}

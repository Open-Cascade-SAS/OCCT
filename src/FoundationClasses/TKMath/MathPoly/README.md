# MathPoly Package

Polynomial root finding algorithms for OCCT.

## Overview

The MathPoly package provides efficient, numerically stable algorithms for finding
real roots of polynomial equations, with dedicated closed-form solvers for degree <= 4
and a general Laguerre-based solver for higher degrees.

## Contents

- `MathPoly_Quadratic.hxx` - Linear and quadratic equation solvers
- `MathPoly_Cubic.hxx` - Cubic equation solver (Cardano's method)
- `MathPoly_Quartic.hxx` - Quartic equation solver (Ferrari's method)
- `MathPoly_Laguerre.hxx` - General polynomial solver (Laguerre + deflation), plus quintic/sextic/octic helpers

## Usage

All functions are in the `MathPoly` namespace:

```cpp
#include <MathPoly_Quadratic.hxx>
#include <MathPoly_Cubic.hxx>
#include <MathPoly_Quartic.hxx>
#include <MathPoly_Laguerre.hxx>

// Solve x^2 - 2 = 0 (find sqrt(2))
auto result1 = MathPoly::Quadratic(1.0, 0.0, -2.0);

// Solve x^3 - 6x^2 + 11x - 6 = 0 (roots: 1, 2, 3)
auto result2 = MathPoly::Cubic(1.0, -6.0, 11.0, -6.0);

// Solve x^4 - 10x^2 + 9 = 0 (roots: -3, -1, 1, 3)
auto result3 = MathPoly::Quartic(1.0, 0.0, -10.0, 0.0, 9.0);

// Solve x^5 - 15x^4 + 85x^3 - 225x^2 + 274x - 120 = 0
auto result4 = MathPoly::Quintic(1.0, -15.0, 85.0, -225.0, 274.0, -120.0);

// Solve degree-8 polynomial with general Laguerre solver
double coeffs[9] = {40320.0, -109584.0, 118124.0, -67284.0, 22449.0, -4536.0, 546.0, -36.0, 1.0};
auto result5 = MathPoly::Laguerre(coeffs, 8);

// Check result
if (result1.IsDone())
{
  for (size_t i = 0; i < result1.NbRoots; ++i)
  {
    std::cout << "Root " << i << ": " << result1.Roots[i] << std::endl;
  }
}
```

## Algorithms

### Linear (ax + b = 0)
- Direct solution: x = -b/a
- Handles degenerate cases (a = 0)

### Quadratic (ax^2 + bx + c = 0)
- Uses numerically stable formula to avoid catastrophic cancellation
- Handles all discriminant cases

### Cubic (ax^3 + bx^2 + cx + d = 0)
- Cardano's method with Vieta substitution
- Trigonometric solution for three real roots (casus irreducibilis)
- Newton-Raphson refinement

### Quartic (ax^4 + bx^3 + cx^2 + dx + e = 0)
- Ferrari's method
- Special handling for biquadratic case
- Newton-Raphson refinement

### General Degree N (Laguerre + Deflation)
- Works up to degree 20 (`MathPoly::Laguerre()`)
- Finds real and complex roots (complex conjugate pairs)
- Uses root deflation and Newton refinement of real roots
- Helper wrappers available for quintic, sextic, and octic cases

## Result Type

Dedicated quadratic/cubic/quartic/quintic/sextic helpers return `MathUtils::PolyResult`:

```cpp
struct PolyResult
{
  Status Status = Status::OK;    // Solution status
  size_t NbRoots = 0;            // Number of real roots found
  std::array<double, 4> Roots;   // Root values (sorted ascending)

  bool IsDone() const;           // True if Status is OK
};
```

General Laguerre solver returns `MathPoly::GeneralPolyResult`:

```cpp
struct GeneralPolyResult
{
  Status Status = Status::NotConverged;
  std::array<double, 20> Roots;                    // Real roots
  std::array<std::complex<double>, 20> ComplexRoots;
  size_t NbRoots = 0;
  size_t NbComplexRoots = 0;
};
```

## Dependencies

- MathUtils (core utilities and types)

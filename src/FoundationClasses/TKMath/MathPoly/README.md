# MathPoly Package

Polynomial root finding algorithms for OCCT.

## Overview

The MathPoly package provides efficient, numerically stable algorithms for finding
real roots of polynomial equations, with dedicated direct solvers for degree <= 4
and a general Laguerre-based solver for higher degrees.

## Contents

- `MathPoly_Types.hxx` - package-local result type with root multiplicities
- `MathPoly_Utils.hxx` - power-of-two scaling, residual checks, and direct-solver utilities
- `MathPoly_Quadratic.hxx` - Linear and quadratic equation solvers
- `MathPoly_Cubic.hxx` - Cubic real-root solver using derivative-recursive isolation
- `MathPoly_Quartic.hxx` - Quartic real-root solver using derivative-recursive isolation
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
- Uses derivative roots to partition the real line into monotone intervals
- Detects tangential roots at critical points and bisects sign-changing intervals
- Reports algebraic multiplicity from successive derivatives

### Quartic (ax^4 + bx^3 + cx^2 + dx + e = 0)
- Uses the same scaled derivative-recursive isolation as the cubic solver
- Returns all distinct real roots in sorted order
- Reports algebraic multiplicity explicitly

### General Degree N (Laguerre + Deflation)
- Works up to degree 20 (`MathPoly::Laguerre()`)
- Finds real and complex roots (complex conjugate pairs)
- Uses power-of-two coefficient/variable scaling and scale-aware residual checks
- Retries Laguerre iteration from deterministic starts and reports failed convergence
- Verifies roots and synthetic-division remainders before accepting deflation
- Uses root deflation and Newton refinement of real roots
- Helper wrappers available for quintic, sextic, and octic cases

## Result Type

All solvers return `MathPoly::PolyResult`:

```cpp
struct PolyResult
{
  Status Status = Status::NotConverged;
  std::array<double, 20> Roots;                 // Distinct sorted real roots
  std::array<size_t, 20> Multiplicities;        // Algebraic multiplicity per real root
  std::array<std::complex<double>, 20> ComplexRoots;
  std::array<size_t, 20> ComplexMultiplicities;
  size_t NbRoots = 0;
  size_t NbComplexRoots = 0;

  bool IsDone() const;
};
```

Repeated roots occur once in `Roots`; their algebraic multiplicity is reported by the matching
`Multiplicities` entry. A finite-coefficient equation whose root is not representable as a finite
`double` reports `NumericalError` instead of returning a non-finite root. `GeneralPolyResult` is an
alias of `PolyResult`.

## Dependencies

- MathUtils (core utilities and types)

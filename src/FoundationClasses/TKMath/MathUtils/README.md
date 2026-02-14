# MathUtils Package

Core utilities and helper functions for modern math solvers in OCCT.

## Overview

The MathUtils package provides foundational utilities used by all other modern math packages:
- **MathPoly** - Polynomial solvers
- **MathLin** - Linear algebra
- **MathOpt** - Optimization/Minimization
- **MathRoot** - Root finding
- **MathInteg** - Integration
- **MathSys** - Nonlinear systems

## Contents

### Types and Configuration
- `MathUtils_Types.hxx` - Common result types and status enums
- `MathUtils_Config.hxx` - Solver configuration structures and shared Newton/line-search constants

### Core Utilities
- `MathUtils_Core.hxx` - Mathematical constants and helper functions
- `MathUtils_Convergence.hxx` - Convergence testing utilities
- `MathUtils_Poly.hxx` - Polynomial evaluation and manipulation
- `MathUtils_Domain.hxx` - 1D/2D parameter domain helpers (contains/clamp/normalize/equality checks)
- `MathUtils_Bracket.hxx` - Root and minimum bracketing algorithms
- `MathUtils_Gauss.hxx` - Gauss-Legendre quadrature points and weights
- `MathUtils_Deriv.hxx` - Numerical differentiation utilities
- `MathUtils_LineSearch.hxx` - Line search algorithms for optimization

### Gauss-Kronrod Weights
- `MathUtils_GaussKronrodWeights.hxx/.cxx` - Access to Gauss-Kronrod quadrature weights

### Functors
- `MathUtils_FunctorScalar.hxx` - Scalar (1D) function functors
- `MathUtils_FunctorVector.hxx` - Vector (N-D) function functors

## Usage

All utilities are in the `MathUtils` namespace:

```cpp
#include <MathUtils_Core.hxx>
#include <MathUtils_Convergence.hxx>

// Use constants
double x = MathUtils::THE_PI;

// Check convergence
bool converged = MathUtils::IsXConverged(xOld, xNew, 1e-8);
```

### Functor Usage

```cpp
#include <MathUtils_FunctorScalar.hxx>
#include <MathRoot_Brent.hxx>

// Using polynomial functor
MathUtils::Polynomial poly({-2.0, 0.0, 1.0});  // x^2 - 2
auto result = MathRoot::Brent(poly, 0.0, 2.0);  // Find sqrt(2)

// Using lambda wrapper
auto func = MathUtils::MakeScalar([](double x, double& y) {
  y = x * x - 2.0;
  return true;
});
auto result2 = MathRoot::Brent(func, 0.0, 2.0);
```

## Key Types

### Status Enum
```cpp
enum class Status {
  OK,
  NotConverged,
  MaxIterations,
  NumericalError,
  InvalidInput,
  InfiniteSolutions,
  NoSolution,
  NotPositiveDefinite,
  Singular,
  NonDescentDirection
};
```

Note: specialized `MathSys` 2D/3D/4D Newton solvers also use `MathUtils::Status`
via `MathSys_NewtonTypes.hxx`.

### Result Types
- `ScalarResult` - For 1D root finding results
- `PolyResult` - For polynomial root results (up to 4 roots)
- `VectorResult` - For N-D optimization results
- `IntegResult` - For integration results with error estimate

### Configuration
```cpp
struct Config {
  double XTolerance = 1e-10;   // Tolerance on X
  double FTolerance = 1e-10;   // Tolerance on F
  int    MaxIterations = 100;  // Maximum iterations
};
```

## Design Principles

1. **Template-based** - No virtual dispatch overhead
2. **Header-only** - Most utilities are header-only templates
3. **Constexpr** - Constants are compile-time evaluated
4. **Non-throwing** - Functions return status codes, not exceptions

# MathRoot Package

The MathRoot package provides algorithms for solving `f(x) = 0` for scalar functions.

All methods require finite inputs, valid non-negative function tolerances, positive X tolerances,
and positive iteration or sample counts. Bracketed methods require strictly ordered bounds;
`NewtonBounded` normalizes reversed finite bounds for compatibility with the legacy solver. For
methods accepting callbacks, `false` produces `CallbackError`, while a non-finite value or
derivative produces `NumericalError`.

## Overview

This package contains header-only template implementations of classical root-finding methods.

## Available Algorithms

### Single Root Finders

| Algorithm | Header | Requirements | Iteration model |
|-----------|--------|--------------|-----------------|
| Newton-Raphson | `MathRoot_Newton.hxx` | Value + derivative | Open derivative step |
| Newton Bounded | `MathRoot_Newton.hxx` | Value + derivative, finite bounds | Bounded derivative step |
| Brent's Method | `MathRoot_Brent.hxx` | Value, sign-changing bracket | Interpolation with bisection safeguard |
| Bisection | `MathRoot_Bisection.hxx` | Value, sign-changing bracket | Bracket halving |
| Bisection-Newton | `MathRoot_Bisection.hxx` | Value + derivative, sign-changing bracket | Newton step with bracket safeguard |
| Secant | `MathRoot_Secant.hxx` | Value and two starting points | Open secant step |

### Multiple Root Finders

| Algorithm | Header | Description |
|-----------|--------|-------------|
| FindAllRoots | `MathRoot_Multiple.hxx` | Finds sampled and sign-changing roots in an interval |
| FindAllRootsWithDerivative | `MathRoot_Multiple.hxx` | Same as above, with extrema detection |
| FindAllRootsWithIntervals | `MathRoot_All.hxx` | Finds roots and null intervals |

### Specialized Solvers

| Algorithm | Header | Description |
|-----------|--------|-------------|
| Trigonometric | `MathRoot_Trig.hxx` | Solves trigonometric equations |

## Usage

### Function Interface

Functions must provide one of the following interfaces:

```cpp
// For value-only methods (Brent, Bisection, Secant)
class MyFunction
{
public:
  bool Value(double theX, double& theF) const;
};

// For methods requiring derivatives (Newton)
class MyFunctionWithDerivative
{
public:
  bool Values(double theX, double& theF, double& theDF) const;
};
```

### Example: Newton's Method

```cpp
#include <MathRoot_Newton.hxx>

class QuadraticFunction
{
public:
  // f(x) = x^2 - 2 (roots at +/- sqrt(2))
  bool Values(double theX, double& theF, double& theDF) const
  {
    theF = theX * theX - 2.0;
    theDF = 2.0 * theX;
    return true;
  }
};

void Example()
{
  QuadraticFunction aFunc;
  MathUtils::Config aConfig;
  aConfig.XTolerance = 1e-10;
  aConfig.FTolerance = 1e-10;

  auto aResult = MathRoot::Newton(aFunc, 1.0, aConfig);
  if (aResult.IsDone())
  {
    double aRoot = *aResult.Root; // approximately sqrt(2)
  }
}
```

### Example: Finding All Roots

```cpp
#include <MathRoot_Multiple.hxx>

class SineFunction
{
public:
  bool Value(double theX, double& theF) const
  {
    theF = std::sin(theX);
    return true;
  }
};

void Example()
{
  SineFunction aFunc;
  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples = 100;

  auto aResult = MathRoot::FindAllRoots(aFunc, 0.0, 10.0, aConfig);
  if (aResult.IsDone())
  {
    // aResult.Roots contains: 0, PI, 2*PI, 3*PI
    for (size_t i = 0; i < aResult.NbRoots(); ++i)
    {
      double aRoot = aResult[i];
    }
  }
}
```

## Algorithm Selection Guide

- **Newton**: Local derivative-based method for smooth functions and a nearby initial guess
- **Newton Bounded**: Newton iteration constrained to a finite interval
- **Brent**: Value-only method combining interpolation and bracket contraction
- **Bisection**: Value-only method with predictable bracket contraction
- **Bisection-Newton**: Derivative step retained inside a sign-changing bracket
- **Secant**: Open value-only method using two starting points

Successful open-method results (Newton and Secant) have a finite residual within `FTolerance`; step
stagnation with a larger residual is not convergence. Bracketed methods (Brent, Bisection, and
Bisection-Newton) may also succeed when a continuous function's sign-changing bracket is localized
within `XTolerance`, so their returned residual can exceed `FTolerance`. A stalled Newton proposal
in the hybrid is not positional convergence unless the bracket itself is localized. Value-only
multiple-root sampling cannot generally detect tangential roots unless a sample point itself has an
acceptable residual; use `FindAllRootsWithDerivative` when extrema detection is required.

## Dependencies

- `MathUtils_Types.hxx`: Common types (Status, ScalarResult)
- `MathUtils_Config.hxx`: Solver configuration
- `MathUtils_Core.hxx`: Core utilities (IsZero, Clamp, etc.)
- `MathUtils_Convergence.hxx`: Convergence testing utilities
- `MathPoly_Quadratic.hxx`, `MathPoly_Quartic.hxx`: polynomial solvers for trigonometric equations

Multiple-root collections and indices are zero-based and use `size_t`. Iteration limits and stored
iteration counters use `uint32_t`.

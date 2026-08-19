# MathRoot Package

The MathRoot package provides a collection of root finding algorithms for scalar functions. These algorithms are designed to find solutions to the equation f(x) = 0 with various convergence guarantees and performance characteristics.

All methods require finite inputs, finite callback outputs, valid non-negative function tolerances, positive X tolerances and positive iteration/sample counts. Interval methods require strictly ordered bounds; reversed bounds are invalid input. A callback returning `false` produces `CallbackError`; a callback returning `true` with a non-finite value or derivative produces `NumericalError`.

## Overview

This package contains header-only template implementations of classical root finding methods, suitable for use with any functor that provides the required interface.

## Available Algorithms

### Single Root Finders

| Algorithm | Header | Requirements | Convergence |
|-----------|--------|--------------|-------------|
| Newton-Raphson | `MathRoot_Newton.hxx` | Value + Derivative | Quadratic |
| Newton Bounded | `MathRoot_Newton.hxx` | Value + Derivative | Quadratic (with safety) |
| Brent's Method | `MathRoot_Brent.hxx` | Value only | Superlinear |
| Bisection | `MathRoot_Bisection.hxx` | Value only | Linear |
| Bisection-Newton | `MathRoot_Bisection.hxx` | Value + Derivative | Quadratic (with safety) |
| Secant | `MathRoot_Secant.hxx` | Value only | Superlinear (~1.618) |

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

- **Newton**: Best for smooth functions when derivative is available and a good initial guess exists
- **Newton Bounded**: Use when Newton might diverge; provides safety with bounds
- **Brent**: Reliable choice when only function values are available; guaranteed convergence
- **Bisection**: Most robust but slowest; use when reliability is paramount
- **Bisection-Newton**: Good balance of robustness and speed
- **Secant**: Good alternative to Newton when derivatives are expensive to compute

Successful open-method results (Newton and Secant) have a finite residual within `FTolerance`; step stagnation with a larger residual is not converged. Bracketed methods (Brent, Bisection, and Bisection-Newton) may also succeed when a continuous function's sign-changing bracket is localized within `XTolerance`, so their returned residual can exceed `FTolerance`. A stalled Newton proposal in the hybrid is not positional convergence unless the bracket itself is localized. Value-only multiple-root sampling cannot generally detect tangential roots unless a sample point itself has an acceptable residual; use `FindAllRootsWithDerivative` when extrema detection is required.

## Dependencies

- `MathUtils_Types.hxx`: Common types (Status, ScalarResult)
- `MathUtils_Config.hxx`: Solver configuration
- `MathUtils_Core.hxx`: Core utilities (IsZero, Clamp, etc.)
- `MathUtils_Convergence.hxx`: Convergence testing utilities
- `MathPoly_Quadratic.hxx`, `MathPoly_Quartic.hxx`: polynomial solvers for trigonometric equations

Multiple-root collections and indices are zero-based and use `size_t`. Iteration limits and stored
iteration counters use `uint32_t`. Package-private helpers are defined in `MathRoot_Utils.hxx` under
`MathRoot::Utils`.

# MathOpt Package

The MathOpt package provides optimization and minimization algorithms for scalar and vector functions in the OCCT mathematical library.

## Overview

This package contains template-based implementations of various optimization algorithms for finding minima of functions. The algorithms are designed to be flexible and work with user-defined function objects.

## Algorithms

### 1D Minimization (MathOpt_Brent.hxx)

- **Brent**: Combines golden section search with parabolic interpolation for robust 1D minimization
- **Golden**: Golden section search for simple, guaranteed convergence
- **BrentWithBracket**: Automatic bracket search followed by Brent's method

### Gradient-Free N-D Optimization

- **Powell** (MathOpt_Powell.hxx): Conjugate direction method without derivatives
- **PSO** (MathOpt_PSO.hxx): Particle Swarm Optimization for global search
- **DifferentialEvolution** (MathOpt_GlobOpt.hxx): Evolutionary algorithm for global optimization
- **MultiStart** (MathOpt_GlobOpt.hxx): Multiple local searches from random starts

### Gradient-Based N-D Optimization

- **BFGS** (MathOpt_BFGS.hxx): Quasi-Newton method with superlinear convergence
- **L-BFGS** (MathOpt_BFGS.hxx): Limited-memory BFGS for large-scale problems
- **FRPR** (MathOpt_FRPR.hxx): Fletcher-Reeves-Polak-Ribiere conjugate gradient
- **Newton** (MathOpt_Newton.hxx): Newton's method with Hessian for quadratic convergence

### Constrained Optimization

- **Uzawa** (MathOpt_Uzawa.hxx): Constrained least squares with equality/inequality constraints

## Usage

All algorithms are implemented as template functions in the `MathOpt` namespace:

```cpp
#include <MathOpt_BFGS.hxx>

// Define function with Value and Gradient methods
class MyFunction
{
public:
  bool Value(const math_Vector& theX, double& theF);
  bool Gradient(const math_Vector& theX, math_Vector& theGrad);
};

// Use BFGS optimization
MyFunction aFunc;
math_Vector aStart(1, 2);
aStart(1) = 0.0;
aStart(2) = 0.0;

MathOpt::VectorResult aResult = MathOpt::BFGS(aFunc, aStart);
if (aResult.IsDone())
{
  const math_Vector& aSolution = *aResult.Solution;
  double aMinValue = *aResult.Value;
}
```

## Dependencies

- MathUtils_Types.hxx - Common types (Status, Config, Result structs)
- MathUtils_Config.hxx - Configuration structures
- MathUtils_Core.hxx - Core utilities
- MathUtils_LineSearch.hxx - Line search algorithms
- MathUtils_Deriv.hxx - Numerical differentiation
- MathUtils_Bracket.hxx - Bracket finding utilities
- MathLin_*.hxx - Linear algebra solvers

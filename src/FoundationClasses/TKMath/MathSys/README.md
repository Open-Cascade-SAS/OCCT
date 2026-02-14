# MathSys Package

## Overview

The MathSys package provides algorithms for solving systems of nonlinear equations.
These solvers are used by geometric algorithms for extrema, projection, and intersection tasks.

## Available Algorithms

### Newton-Raphson Method (`MathSys_Newton.hxx`)

The generic Newton-Raphson method solves systems `F(X) = 0` by repeatedly solving
`J(X) * dX = -F(X)`.

**Features:**
- Fast local quadratic convergence near the solution
- Generic vector/matrix interface for arbitrary dimension
- Supports bounded variants with box constraints

### Optimized 2D Newton Solvers (`MathSys_Newton2D.hxx`)

Specialized 2D solvers target compact, allocation-free execution for common CAD kernels.

**Features:**
- Zero allocation overhead (no `math_Vector`/`math_Matrix` in iteration loop)
- Fast direct 2x2 solve path
- SVD fallback for ill-conditioned symmetric Jacobians
- Squared-norm convergence checks (avoid unnecessary `sqrt`)
- Step limiting to prevent wild oscillations
- Gradient-descent fallback on singular Jacobians
- Armijo backtracking line search for symmetric mode

**Functions:**
- `MathSys::Solve2D()` - General 2D Newton solver
- `MathSys::Solve2DSymmetric()` - Robust 2D solver for symmetric Jacobian systems

**Usage Example (2D):**
```cpp
#include <MathSys_Newton2D.hxx>

MySymmetricFunc aFunc;

MathSys::NewtonBoundsN<2> aBounds;
aBounds.Min = {-10.0, -10.0};
aBounds.Max = {10.0, 10.0};

MathSys::NewtonOptions aOptions;
aOptions.ResidualTol      = 1.0e-10;
aOptions.MaxIterations    = 100;
aOptions.EnableLineSearch = true;

const MathSys::NewtonResultN<2> aResult =
  MathSys::Solve2DSymmetric(aFunc, {0.0, 0.0}, aBounds, aOptions);
if (aResult.IsDone())
{
  const double aU = aResult.X[0];
  const double aV = aResult.X[1];
}
```

### Optimized 3D Newton Solvers (`MathSys_Newton3D.hxx`)

3D solvers cover constrained geometric systems such as curve-surface extrema.

**Features:**
- Zero allocation overhead in core iteration path
- Cramer's rule/cofactor-based 3x3 solve
- Squared-norm convergence checks
- Step limiting for stability
- Gradient-descent fallback for singular cases
- Specialized curve-surface extrema formulation

**Functions:**
- `MathSys::Solve3D()` - General 3D Newton solver
- `MathSys::SolveCurveSurfaceExtrema3D()` - Specialized curve-surface extrema solver

**Usage Example (General 3D):**
```cpp
#include <MathSys_Newton3D.hxx>

MyFunc3D aFunc;

MathSys::NewtonBoundsN<3> aBounds;
aBounds.Min = {-100.0, -100.0, -100.0};
aBounds.Max = {100.0, 100.0, 100.0};

MathSys::NewtonOptions aOptions;
aOptions.ResidualTol   = 1.0e-10;
aOptions.MaxIterations = 100;

const MathSys::NewtonResultN<3> aResult =
  MathSys::Solve3D(aFunc, {1.0, 2.0, 3.0}, aBounds, aOptions);
if (aResult.IsDone())
{
  const double aX1 = aResult.X[0];
  const double aX2 = aResult.X[1];
  const double aX3 = aResult.X[2];
}
```

**Usage Example (Curve-Surface Extrema):**
```cpp
#include <MathSys_Newton3D.hxx>

CurveEval aCurve;
SurfaceEval aSurface;

MathSys::NewtonBoundsN<3> aBounds;
aBounds.Min = {tMin, uMin, vMin};
aBounds.Max = {tMax, uMax, vMax};

MathSys::NewtonOptions aOptions;
aOptions.ResidualTol   = tolerance;
aOptions.MaxIterations = maxIter;

const MathSys::NewtonResultN<3> aResult =
  MathSys::SolveCurveSurfaceExtrema3D(aCurve, aSurface, {t0, u0, v0}, aBounds, aOptions);
```

### Optimized 4D Newton Solvers (`MathSys_Newton4D.hxx`)

4D solvers handle coupled geometric systems such as surface-surface extrema.

**Features:**
- Zero allocation overhead in iteration loop
- Gaussian elimination with partial pivoting for 4x4 systems
- Squared-norm convergence checks
- Step limiting for robustness
- Gradient-descent fallback for singular Jacobians
- Specialized surface-surface extrema system with structured Jacobian blocks

**Functions:**
- `MathSys::Solve4D()` - General 4D Newton solver
- `MathSys::SolveSurfaceSurfaceExtrema4D()` - Specialized surface-surface extrema solver

**Usage Example (General 4D):**
```cpp
#include <MathSys_Newton4D.hxx>

MyFunc4D aFunc;

MathSys::NewtonBoundsN<4> aBounds;
aBounds.Min = {-10.0, -10.0, -10.0, -10.0};
aBounds.Max = {10.0, 10.0, 10.0, 10.0};

MathSys::NewtonOptions aOptions;
aOptions.ResidualTol   = 1.0e-10;
aOptions.MaxIterations = 100;

const MathSys::NewtonResultN<4> aResult =
  MathSys::Solve4D(aFunc, {0.0, 0.0, 0.0, 0.0}, aBounds, aOptions);
```

**Usage Example (Surface-Surface Extrema):**
```cpp
#include <MathSys_Newton4D.hxx>

SurfaceEval1 aSurf1;
SurfaceEval2 aSurf2;

MathSys::NewtonBoundsN<4> aBounds;
aBounds.Min = {u1Min, v1Min, u2Min, v2Min};
aBounds.Max = {u1Max, v1Max, u2Max, v2Max};

MathSys::NewtonOptions aOptions;
aOptions.ResidualTol   = tolerance;
aOptions.MaxIterations = maxIter;

const MathSys::NewtonResultN<4> aResult =
  MathSys::SolveSurfaceSurfaceExtrema4D(aSurf1, aSurf2, {u1_0, v1_0, u2_0, v2_0}, aBounds, aOptions);
```

### Levenberg-Marquardt Algorithm (`MathSys_LevenbergMarquardt.hxx`)

Levenberg-Marquardt targets nonlinear least-squares problems by minimizing `||F(X)||^2`.
It blends Gauss-Newton speed near minima with gradient-descent robustness far from minima.

**Features:**
- Robust convergence under poor initial guesses
- Adaptive damping to stabilize updates
- Works with the generic vector interface
- Suitable when pure Newton is too brittle for noisy systems

## Interface Styles and Return Types

MathSys exposes two interface styles:

- Generic vector API:
  - Headers: `MathSys_Newton.hxx`, `MathSys_LevenbergMarquardt.hxx`
  - Uses `math_Vector`/`math_Matrix`-based interfaces
  - Returns `MathUtils::VectorResult`

- Specialized small-dimension API:
  - Headers: `MathSys_Newton2D.hxx`, `MathSys_Newton3D.hxx`, `MathSys_Newton4D.hxx`
  - Uses scalar callables + fixed-size arrays
  - Returns `MathSys::NewtonResultN<N>`

Shared specialized API types are defined in `MathSys_NewtonTypes.hxx`:
- `MathSys::NewtonStatus`
- `MathSys::NewtonResultN<N>`
- `MathSys::NewtonBoundsN<N>`
- `MathSys::NewtonOptions`

## Convergence Notes

- Specialized solvers use strict residual-based convergence (`ResidualNorm <= ResidualTol`).
- Step-size stagnation alone does not produce `Converged` status.
- `Solve2DSymmetric()` uses Armijo backtracking with directional derivative based on `J^T * F`.

## Dependencies

- `MathUtils` - common utilities, numeric types, and generic solver support
- `gp` package - geometric point/vector primitives for extrema-specialized solvers

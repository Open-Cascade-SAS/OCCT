# MathSys Package

## Overview

The MathSys package provides algorithms for solving systems of nonlinear equations.
These solvers are essential for geometric computations involving curve/surface intersections,
constraint solving, and optimization problems in CAD/CAM applications.

## Available Algorithms

### Newton-Raphson Method (`MathSys_Newton.hxx`)

The Newton-Raphson method is an iterative algorithm for finding roots of systems of equations F(X) = 0.
At each iteration, it solves the linear system J(X) * dX = -F(X), where J is the Jacobian matrix.

**Features:**
- Fast quadratic convergence near the solution
- Supports per-variable tolerance specification
- Bounded variant with box constraints

**Functions:**
- `MathSys::Newton()` - Standard Newton-Raphson iteration
- `MathSys::NewtonBounded()` - Newton method with box constraints

### Optimized 2D Newton Solver (`MathSys_Newton2D.hxx`)

A highly optimized Newton solver specifically designed for 2D problems (systems of 2 equations
in 2 unknowns). Common use cases include point-surface extrema, curve intersections, and
surface-surface intersection curves.

**Features:**
- Zero allocation overhead (no math_Vector/math_Matrix)
- Cramer's rule for 2x2 linear system (faster than LU decomposition)
- Squared norm comparisons (avoids sqrt in convergence check)
- Step limiting to prevent wild oscillations
- Gradient descent fallback for singular Jacobian
- Symmetric Jacobian variant for gradient-based problems

**Functions:**
- `MathSys::Newton2D()` - General 2D Newton solver
- `MathSys::Newton2DSymmetric()` - Optimized for symmetric Jacobians (e.g., gradient of scalar function)

**Usage Example (Point-Surface Extrema):**
```cpp
// Function with symmetric Jacobian: F = [dD/dU, dD/dV] where D = ||S(u,v) - P||^2
auto aResult = MathSys::Newton2DSymmetric(myDistFunc, u0, v0,
                                           uMin, uMax, vMin, vMax,
                                           tolerance, maxIter);
if (aResult.IsDone())
{
  double u = aResult.U;
  double v = aResult.V;
}
```

### Levenberg-Marquardt Algorithm (`MathSys_LevenbergMarquardt.hxx`)

The Levenberg-Marquardt algorithm is designed for nonlinear least squares problems,
minimizing ||F(X)||^2. It combines Gauss-Newton (fast convergence near minimum) with
gradient descent (robust far from minimum) using adaptive damping.

**Features:**
- Robust convergence even with poor initial guesses
- Adaptive damping parameter for stability
- Configurable convergence criteria
- Bounded variant with box constraints

**Functions:**
- `MathSys::LevenbergMarquardt()` - Standard LM algorithm
- `MathSys::LevenbergMarquardtBounded()` - LM with box constraints

## Usage

All solvers in this package operate on function sets that provide:
- `NbVariables()` - Number of variables in the system
- `NbEquations()` - Number of equations
- `Values(X, F, J)` - Compute function values and Jacobian at point X

Results are returned as `VectorResult` structures containing:
- `Status` - Convergence status (OK, MaxIterations, NumericalError, etc.)
- `Solution` - The computed solution vector
- `NbIterations` - Number of iterations performed

## Dependencies

- `MathUtils` - Core utilities, types, and configuration
- `MathLin` - Linear algebra solvers for the Newton step

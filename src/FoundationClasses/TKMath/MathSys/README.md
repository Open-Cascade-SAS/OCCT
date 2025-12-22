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

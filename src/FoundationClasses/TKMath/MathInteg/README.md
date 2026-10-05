# MathInteg Package

Numerical integration (quadrature) algorithms for OCCT.

## Overview

The MathInteg package provides fixed-order and adaptive quadrature for finite, semi-infinite, and
infinite intervals, including double-exponential rules for endpoint singularities.

## Components

### MathInteg_Gauss.hxx
Gauss-Legendre quadrature methods:
- `Gauss` - Fixed-order Gauss-Legendre integration (order 0 is invalid; orders above 61 are
  clamped to 61)
- `GaussAdaptive` - Adaptive subdivision with error control using `IntegConfig.InitialOrder/MaxOrder`
- `GaussComposite` - Composite rule over multiple subintervals

### MathInteg_Kronrod.hxx
Gauss-Kronrod quadrature with embedded error estimation:
- `KronrodRule` - Single application of Gauss-Kronrod rule (Gauss orders 1 through 30)
- `Kronrod` - Adaptive Gauss-Kronrod integration
- `KronrodAuto` - Automatic order selection
- `KronrodSemiInfinite` - Integration over [a, +infinity)
- `KronrodInfinite` - Integration over (-infinity, +infinity)

### MathInteg_DoubleExp.hxx
Double exponential (tanh-sinh) quadrature:
- `TanhSinh` - Finite interval integration with endpoint singularity handling
- `ExpSinh` - Semi-infinite interval [a, +infinity)
- `SinhSinh` - Infinite interval (-infinity, +infinity)
- `DoubleExponential` - Automatic interval type detection
- `TanhSinhSingular` - Optimized for singular integrands
- `TanhSinhWithSingularity` - Known interior singularity

### MathInteg_Multiple.hxx
Multi-dimensional integration:
- `GaussMultiple` - N-dimensional Gauss-Legendre quadrature
- `GaussMultipleUniform` - Uniform order for all dimensions

### MathInteg_Set.hxx
Vector-valued function integration:
- `GaussSet` - Gauss-Legendre integration of R -> R^N functions with a positive integral
  `NbEquations()` result

## Usage

All functions are template-based and accept functor types with appropriate `Value()` methods. Results are returned as `IntegResult` or `SetResult` structures containing the integral value, error estimates, and status information.

Finite integrations use oriented intervals: reversing the lower and upper bounds negates the
integral, while equal finite bounds return exactly zero without evaluating the function. Non-finite
endpoints are invalid for finite-interval overloads. Adaptive methods interpret `Tolerance` as a
relative error tolerance when the integral magnitude exceeds machine epsilon.
For a near-zero integral, `Tolerance` is the absolute error tolerance and `RelativeError` is not
set because a meaningful relative estimate is unavailable.

Double-exponential rules limit each refinement level to
`DoubleExpConfig::MaxPointsPerLevel` callback evaluations (10,000 by default). Reaching that bound
returns `Status::MaxIterations`; this prevents tiny but finite transformed-coordinate steps from
creating unbounded sampling loops. `NbLevels` and `MaxIterations` independently bound refinement
levels; `NbLevels` defaults to 10 so endpoint-singular intervals have enough refinement depth.

```cpp
#include <MathInteg_Gauss.hxx>

class MyFunc
{
public:
  bool Value(double theX, double& theF)
  {
    theF = theX * theX;
    return true;
  }
};

MyFunc aFunc;
MathInteg::IntegResult aResult = MathInteg::Gauss(aFunc, 0.0, 1.0, 15);
if (aResult.IsDone())
{
  double aValue = *aResult.Value; // Should be ~0.333...
}
```

## Dependencies

- MathUtils package for core types, configuration, and helper functions
- math_Vector, math_IntegerVector, math_Matrix for multi-dimensional integration

Iteration counters use `uint32_t`; quadrature point and callback evaluation counts use `size_t`.

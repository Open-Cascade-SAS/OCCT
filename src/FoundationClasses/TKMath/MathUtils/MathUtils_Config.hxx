// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _MathUtils_Config_HeaderFile
#define _MathUtils_Config_HeaderFile

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

//! Modern math solver configuration structures.
namespace MathUtils
{

//! Squared function tolerance for Newton iterations (matching math_FunctionSetRoot Eps).
constexpr double THE_NEWTON_FTOL_SQ = 1.0e-32;

//! Relative parametric step tolerance factor for Newton iterations.
constexpr double THE_NEWTON_STEP_TOL_FACTOR = 1.0e-16;

//! Default maximum iterations for Newton solvers.
constexpr size_t THE_NEWTON_MAX_ITER = 100;

//=================================================================================================
//! @name Newton 2D Solver Constants
//! Constants for 2D Newton-Raphson solver (MathSys_Newton2D).
//=================================================================================================

//! Determinant threshold below which 2x2 Jacobian matrix is considered singular.
//! When |det(J)| < threshold, falls back to SVD or gradient descent.
constexpr double THE_NEWTON2D_SINGULAR_DET = 1.0e-25;

//! Squared gradient threshold below which point is considered a critical point.
//! When |grad|^2 < threshold, Newton iteration is at a stationary point.
constexpr double THE_NEWTON2D_CRITICAL_GRAD_SQ = 1.0e-60;

//! Maximum step size as fraction of domain size for bounded Newton iterations.
constexpr double THE_NEWTON2D_MAX_STEP_RATIO = 0.5;

//! Domain extension factor for soft boundary handling.
//! Solutions slightly outside domain (by this fraction) are allowed if converged.
//! Reduced to 1e-4 to closely match old algorithm behavior.
constexpr double THE_NEWTON2D_DOMAIN_EXT = 1.0e-4;

//! Stagnation progress ratio - improvement required each iteration to avoid stagnation.
//! Value of 0.999 means at least 0.1% improvement required.
constexpr double THE_NEWTON2D_STAGNATION_RATIO = 0.999;

//! Number of iterations without progress before declaring stagnation.
constexpr size_t THE_NEWTON2D_STAGNATION_COUNT = 3;

//! Maximum line search backtracking iterations.
constexpr size_t THE_NEWTON2D_LINE_SEARCH_MAX = 8;

//! Relative step threshold below which line search is skipped.
//! If step^2 / domain^2 < threshold, Newton is converging well.
constexpr double THE_NEWTON2D_SKIP_LINESEARCH_SQ = 0.01;

//! Tolerance relaxation factor for accepting stagnated solutions.
//! Stagnated solution accepted if |F| < tolerance * factor.
constexpr double THE_NEWTON2D_STAGNATION_RELAX = 10.0;

//! Function increase threshold triggering line search backtracking.
//! If f(new) > f(old) * threshold, backtracking is triggered.
constexpr double THE_NEWTON2D_BACKTRACK_TRIGGER = 1.5;

//! Backtracking acceptance ratio for line search.
//! Step accepted if f(new) < f(old) * ratio.
constexpr double THE_NEWTON2D_BACKTRACK_ACCEPT = 1.2;

//! Maximum relaxation factor for solutions at max iterations.
constexpr double THE_NEWTON2D_MAXITER_RELAX = 10.0;

//=================================================================================================
//! @name Hessian Classification Constants
//! Constants for extremum classification using second derivatives.
//=================================================================================================

//! Relative tolerance for Hessian degeneracy detection.
//! If |det(H)| < threshold * |H_ii| * |H_jj|, Hessian is considered degenerate.
constexpr double THE_HESSIAN_DEGENERACY_REL = 1.0e-8;

//! Absolute tolerance for Hessian degeneracy detection.
//! Minimum threshold for determinant comparison.
constexpr double THE_HESSIAN_DEGENERACY_ABS = 1.0e-20;

//=================================================================================================
//! @name Line Search Constants
//! Constants for line search acceptance conditions.
//=================================================================================================

//! Armijo condition constant (sufficient decrease).
//! Step accepted if: f(x + alpha*d) <= f(x) + c1 * alpha * grad_f . d
constexpr double THE_ARMIJO_C1 = 1.0e-4;

//! Configuration for iterative solvers.
//! Provides common settings for convergence criteria and iteration limits.
struct Config
{
  uint32_t MaxIterations = 100;   //!< Maximum number of iterations allowed
  double Tolerance     = 1.0e-10; //!< General convergence tolerance
  double XTolerance    = 1.0e-10; //!< Tolerance for solution change |x_{n+1} - x_n|
  double FTolerance    = 1.0e-10; //!< Tolerance for function value |f(x)|

  //! Relative tolerance used together with the absolute X/F tolerances.
  double RelativeTolerance = 1.0e-10;

  //! Minimum step size before declaring convergence or failure.
  double StepMin = std::numeric_limits<double>::epsilon();

  //! Default constructor with standard tolerances.
  Config() = default;

  //! Constructor with custom tolerance (sets all tolerances to same value).
  //! @param theTolerance convergence tolerance
  //! @param theMaxIter maximum iterations
  explicit Config(double theTolerance, uint32_t theMaxIter = 100)
      : MaxIterations(theMaxIter),
        Tolerance(theTolerance),
        XTolerance(theTolerance),
        FTolerance(theTolerance),
        RelativeTolerance(theTolerance)
  {
  }

  //! Return true when all iteration and tolerance settings are usable.
  bool IsValid() const
  {
    return MaxIterations > 0 && std::isfinite(Tolerance) && Tolerance >= 0.0
           && std::isfinite(XTolerance) && XTolerance >= 0.0 && std::isfinite(FTolerance)
           && FTolerance >= 0.0 && std::isfinite(RelativeTolerance) && RelativeTolerance >= 0.0
           && std::isfinite(StepMin) && StepMin > 0.0;
  }
};

//! Configuration for bounded 1D optimization and root finding.
//! Extends Config with interval bounds.
struct BoundedConfig : Config
{
  double LowerBound = -std::numeric_limits<double>::max(); //!< Lower bound of search interval
  double UpperBound = std::numeric_limits<double>::max();  //!< Upper bound of search interval

  //! Default constructor.
  BoundedConfig() = default;

  //! Constructor with bounds.
  //! @param theLower lower bound
  //! @param theUpper upper bound
  //! @param theTolerance convergence tolerance
  //! @param theMaxIter maximum iterations
  BoundedConfig(double theLower,
                double theUpper,
                double theTolerance = 1.0e-10,
                uint32_t theMaxIter   = 100)
      : Config(theTolerance, theMaxIter),
        LowerBound(theLower),
        UpperBound(theUpper)
  {
  }

  //! Return true when the base configuration and finite ordered bounds are valid.
  bool IsValid() const
  {
    return Config::IsValid() && std::isfinite(LowerBound) && std::isfinite(UpperBound)
           && LowerBound <= UpperBound;
  }
};

//! Configuration for N-dimensional optimization with optional bounds.
//! Bounds are passed separately as math_Vector for flexibility.
struct NDimConfig : Config
{
  bool UseBounds = false; //!< Whether to enforce bounds during optimization

  //! Default constructor.
  NDimConfig() = default;

  //! Constructor with tolerance.
  //! @param theTolerance convergence tolerance
  //! @param theMaxIter maximum iterations
  //! @param theUseBounds whether to use bounds
  explicit NDimConfig(double   theTolerance,
                      uint32_t theMaxIter  = 100,
                      bool     theUseBounds = false)
      : Config(theTolerance, theMaxIter),
        UseBounds(theUseBounds)
  {
  }
};

//! Configuration for numerical integration.
//! Provides settings for quadrature order and adaptive refinement.
struct IntegConfig
{
  uint32_t InitialOrder  = 15;    //!< Initial number of quadrature points
  uint32_t MaxOrder      = 61;    //!< Maximum quadrature order (Gauss-Legendre limit)
  uint32_t MaxIterations = 100;   //!< Maximum adaptive subdivision iterations
  double Tolerance     = 1.0e-10; //!< Relative tolerance for error estimation

  //! Default constructor.
  IntegConfig() = default;

  //! Constructor with custom tolerance.
  //! @param theTolerance relative tolerance
  //! @param theMaxIter maximum adaptive iterations
  explicit IntegConfig(double theTolerance, uint32_t theMaxIter = 100)
      : MaxIterations(theMaxIter),
        Tolerance(theTolerance)
  {
  }

  //! Return true when quadrature and tolerance settings are usable.
  bool IsValid() const
  {
    return InitialOrder > 0 && MaxOrder >= InitialOrder && MaxIterations > 0
           && std::isfinite(Tolerance) && Tolerance >= 0.0;
  }
};

//! Configuration for linear algebra solvers.
//! Provides settings for singularity detection and pivoting.
struct LinConfig
{
  double SingularityTolerance = 1.0e-15; //!< Tolerance for detecting singular matrices
  bool   UsePivoting          = true;    //!< Whether to use pivoting for stability

  //! Default constructor.
  LinConfig() = default;

  //! Return true when the singularity tolerance is finite and non-negative.
  bool IsValid() const
  {
    return std::isfinite(SingularityTolerance) && SingularityTolerance >= 0.0;
  }
};

} // namespace MathUtils

#endif // _MathUtils_Config_HeaderFile

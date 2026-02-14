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

//==================================================================================================
//! @name Newton 2D Solver Constants
//! Constants for 2D Newton-Raphson solver (MathSys_Newton2D).
//==================================================================================================

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
constexpr double THE_NEWTON2D_DOMAIN_EXT = 0.01;

//! Stagnation progress ratio - improvement required each iteration to avoid stagnation.
//! Value of 0.999 means at least 0.1% improvement required.
constexpr double THE_NEWTON2D_STAGNATION_RATIO = 0.999;

//! Number of iterations without progress before declaring stagnation.
constexpr int THE_NEWTON2D_STAGNATION_COUNT = 3;

//! Maximum line search backtracking iterations.
constexpr int THE_NEWTON2D_LINE_SEARCH_MAX = 12;

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

//! High precision tolerance target (5e-8, better than Precision::Confusion).
constexpr double THE_HIGH_PRECISION_TOL = 5.0e-8;

//! Configuration for iterative solvers.
//! Provides common settings for convergence criteria and iteration limits.
struct Config
{
  int    MaxIterations = 100;     //!< Maximum number of iterations allowed
  double Tolerance     = 1.0e-10; //!< General convergence tolerance
  double XTolerance    = 1.0e-10; //!< Tolerance for solution change |x_{n+1} - x_n|
  double FTolerance    = 1.0e-10; //!< Tolerance for function value |f(x)|

  //! Minimum step size before declaring convergence or failure.
  double StepMin = std::numeric_limits<double>::epsilon();

  //! Default constructor with standard tolerances.
  Config() = default;

  //! Constructor with custom tolerance (sets all tolerances to same value).
  //! @param theTolerance convergence tolerance
  //! @param theMaxIter maximum iterations
  explicit Config(double theTolerance, int theMaxIter = 100)
      : MaxIterations(theMaxIter),
        Tolerance(theTolerance),
        XTolerance(theTolerance),
        FTolerance(theTolerance)
  {
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
                int    theMaxIter   = 100)
      : Config(theTolerance, theMaxIter),
        LowerBound(theLower),
        UpperBound(theUpper)
  {
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
  explicit NDimConfig(double theTolerance, int theMaxIter = 100, bool theUseBounds = false)
      : Config(theTolerance, theMaxIter),
        UseBounds(theUseBounds)
  {
  }
};

//! Configuration for numerical integration.
//! Provides settings for quadrature order and adaptive refinement.
struct IntegConfig
{
  int    InitialOrder  = 15;      //!< Initial number of quadrature points
  int    MaxOrder      = 61;      //!< Maximum quadrature order (Gauss-Legendre limit)
  int    MaxIterations = 100;     //!< Maximum adaptive subdivision iterations
  double Tolerance     = 1.0e-10; //!< Relative tolerance for error estimation

  //! Default constructor.
  IntegConfig() = default;

  //! Constructor with custom tolerance.
  //! @param theTolerance relative tolerance
  //! @param theMaxIter maximum adaptive iterations
  explicit IntegConfig(double theTolerance, int theMaxIter = 100)
      : MaxIterations(theMaxIter),
        Tolerance(theTolerance)
  {
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
};

} // namespace MathUtils

#endif // _MathUtils_Config_HeaderFile

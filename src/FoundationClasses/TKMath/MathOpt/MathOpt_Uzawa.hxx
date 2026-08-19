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

#ifndef _MathOpt_Uzawa_HeaderFile
#define _MathOpt_Uzawa_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <MathLin_Crout.hxx>
#include "MathOpt_Utils.hxx"

#include <cmath>

namespace MathOpt
{
using namespace MathUtils;

//! Result for Uzawa constrained optimization.
struct UzawaResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Vector> Solution;     //!< Solution vector X
  std::optional<math_Vector> Dual;         //!< Dual (Lagrange) variables
  std::optional<math_Vector> Error;        //!< X - X0 (difference from starting point)
  std::optional<math_Vector> InitialError; //!< C*X0 - S (initial constraint violation)
  std::optional<math_Matrix> InverseCTC;   //!< (C * C^T)^-1 for gradient computation
  uint32_t                   NbIterations = 0;

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Configuration for Uzawa algorithm.
struct UzawaConfig
{
  double EpsLix        = 1.0e-6; //!< Tolerance for X convergence
  double EpsLic        = 1.0e-6; //!< Tolerance for dual variable convergence
  uint32_t MaxIterations = 500;   //!< Maximum iterations
};

//! Solve constrained least squares using Uzawa algorithm.
//!
//! Solves: min ||X - X0||^2 subject to C*X = S
//!
//! For equality constraints only, uses direct Crout decomposition.
//! For mixed equality/inequality constraints, uses iterative Uzawa method.
//!
//! The Uzawa algorithm is a dual decomposition method that:
//! 1. Updates primal variables X to minimize Lagrangian
//! 2. Updates dual variables (Lagrange multipliers) for constraint violations
//!
//! @param theCont constraint matrix C (Nce+Nci rows x N cols)
//! @param theSecont right-hand side S
//! @param theStartingPoint initial point X0
//! @param theNce number of equality constraints (first rows)
//! @param theNci number of inequality constraints (last rows, C*X <= S)
//! @param theConfig algorithm configuration
//! @return UzawaResult with solution and auxiliary data
inline UzawaResult Uzawa(const math_Matrix& theCont,
                         const math_Vector& theSecont,
                         const math_Vector& theStartingPoint,
                          size_t             theNce,
                          size_t             theNci,
                         const UzawaConfig& theConfig = UzawaConfig())
{
  UzawaResult aResult;

  const size_t aNlig = theCont.RowSize();
  const size_t aNcol = theCont.ColSize();

  // Validate dimensions
  if (theNce > aNlig || theNci != aNlig - theNce
      || theSecont.Size() != aNlig || theStartingPoint.Size() != aNcol
      || !Utils::IsFinite(theCont) || !Utils::IsFinite(theSecont)
      || !Utils::IsFinite(theStartingPoint) || theConfig.MaxIterations == 0
      || !std::isfinite(theConfig.EpsLix) || theConfig.EpsLix <= 0.0
      || !std::isfinite(theConfig.EpsLic) || theConfig.EpsLic <= 0.0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Compute initial error: C*X0 - S
  math_Vector aErrinit(aNlig);
  for (size_t i = 0; i < aNlig; ++i)
  {
    double aSum = 0.0;
    for (size_t j = 0; j < aNcol; ++j)
    {
      aSum += theCont.At(i, j) * theStartingPoint.At(j);
    }
    aErrinit.ChangeAt(i) = aSum - theSecont.At(i);
  }

  aResult.InitialError = aErrinit;

  // Initialize dual variables and error
  math_Vector aVardua(aNlig);
  math_Vector aErruza(aNcol, 0.0);
  math_Matrix aCTCinv(aNlig, aNlig, 0.0);

  if (theNci == 0)
  {
    // Direct solution for equality constraints only
    aResult.NbIterations = 1;

    // Compute C * C^T (symmetric)
    for (size_t i = 0; i < aNlig; ++i)
    {
      for (size_t j = 0; j <= i; ++j)
      {
        double aSum = 0.0;
        for (size_t k = 0; k < aNcol; ++k)
        {
          aSum += theCont.At(i, k) * theCont.At(j, k);
        }
        aCTCinv.ChangeAt(i, j) = aSum;
        if (i != j)
        {
          aCTCinv.ChangeAt(j, i) = aSum;
        }
      }
    }

    // Invert using Crout
    MathLin::InverseResult aInvResult = MathLin::InvertCrout(aCTCinv);
    if (!aInvResult.IsDone())
    {
      aResult.Status = Status::Singular;
      return aResult;
    }
    aCTCinv = *aInvResult.Inverse;
    if (!Utils::IsFinite(aCTCinv))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    // Compute dual variables: (C*C^T)^-1 * (C*X0 - S)
    for (size_t i = 0; i < aNlig; ++i)
    {
      double aSum = 0.0;
      for (size_t j = 0; j < aNlig; ++j)
      {
        aSum += aCTCinv.At(i, j) * aErrinit.At(j);
      }
      aVardua.ChangeAt(i) = aSum;
    }

    // Compute error: -C^T * dual
    for (size_t i = 0; i < aNcol; ++i)
    {
      double aSum = 0.0;
      for (size_t j = 0; j < aNlig; ++j)
      {
        aSum -= theCont.At(j, i) * aVardua.At(j);
      }
      aErruza.ChangeAt(i) = aSum;
    }

    // Compute solution
    math_Vector aResul(aNcol);
    for (size_t i = 0; i < aNcol; ++i)
    {
      aResul.ChangeAt(i) = theStartingPoint.At(i) + aErruza.At(i);
    }

    aResult.Solution   = aResul;
    aResult.Dual       = aVardua;
    aResult.Error      = aErruza;
    aResult.InverseCTC = aCTCinv;
    aResult.Status     = Utils::IsFinite(aResul) && Utils::IsFinite(aVardua)
                            && Utils::IsFinite(aErruza)
                         ? Status::OK
                         : Status::NumericalError;
    return aResult;
  }

  // Iterative Uzawa for mixed equality/inequality constraints
  // Initialize dual variables
  for (size_t i = 0; i < aNlig; ++i)
  {
    aVardua.ChangeAt(i) = (i < theNce) ? 0.0 : 1.0;
  }

  // Compute step size rho
  double aMatrixNorm = 0.0;
  for (size_t i = 0; i < aNlig; ++i)
  {
    for (size_t j = 0; j < aNcol; ++j)
    {
      const double aVal = theCont.At(i, j);
      aMatrixNorm       = std::hypot(aMatrixNorm, aVal);
    }
  }
  if (!std::isfinite(aMatrixNorm)
      || aMatrixNorm <= std::sqrt(std::numeric_limits<double>::min()))
  {
    aResult.Status = Status::Singular;
    return aResult;
  }
  const double aRho = 1.0 / aMatrixNorm / aMatrixNorm / std::sqrt(2.0);
  if (!std::isfinite(aRho) || aRho <= 0.0)
  {
    aResult.Status = Status::NumericalError;
    return aResult;
  }

  // Uzawa iterations
  for (uint32_t anIter = 0; anIter < theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter + 1;

    double aXmax = 0.0;

    // Update primal: X = X0 - C^T * dual
    for (size_t i = 0; i < aNcol; ++i)
    {
      const double aXprev = aErruza.At(i);
      double aSum   = 0.0;
      for (size_t j = 0; j < aNlig; ++j)
      {
        aSum -= theCont.At(j, i) * aVardua.At(j);
      }
      aErruza.ChangeAt(i) = aSum;

      if (anIter > 0)
      {
        aXmax = std::max(aXmax, std::abs(aErruza.At(i) - aXprev));
      }
    }

    // Update dual variables and compute constraint error
    double aErrMax = 0.0;
    for (size_t i = 0; i < aNlig; ++i)
    {
      // Constraint violation: C*X - S
      double aErr = aErrinit.At(i);
      for (size_t j = 0; j < aNcol; ++j)
      {
        aErr += theCont.At(i, j) * aErruza.At(j);
      }

      double aErr1;
      if (i < theNce)
      {
        // Equality constraint: update freely
        aVardua.ChangeAt(i) += aRho * aErr;
        aErr1 = std::abs(aRho * aErr);
      }
      else
      {
        // Inequality constraint: project to non-negative
        const double aXmuPrev = aVardua.At(i);
        aVardua.ChangeAt(i)   = std::max(0.0, aVardua.At(i) + aRho * aErr);
        aErr1                 = std::abs(aVardua.At(i) - aXmuPrev);
      }
      aErrMax = std::max(aErrMax, aErr1);
    }

    // Primal and dual changes are independent convergence requirements.
    if (anIter > 0 && aXmax <= theConfig.EpsLix && aErrMax <= theConfig.EpsLic)
    {
      math_Vector aResul(aNcol);
      for (size_t i = 0; i < aNcol; ++i)
      {
        aResul.ChangeAt(i) = theStartingPoint.At(i) + aErruza.At(i);
      }

      aResult.Solution = aResul;
      aResult.Dual     = aVardua;
      aResult.Error    = aErruza;
      aResult.Status   = Utils::IsFinite(aResul) && Utils::IsFinite(aVardua)
                            && Utils::IsFinite(aErruza)
                         ? Status::OK
                         : Status::NumericalError;
      return aResult;
    }
  }

  math_Vector aResul(aNcol);
  for (size_t i = 0; i < aNcol; ++i)
  {
    aResul.ChangeAt(i) = theStartingPoint.At(i) + aErruza.At(i);
  }
  aResult.Solution = aResul;
  aResult.Dual     = aVardua;
  aResult.Error    = aErruza;
  aResult.Status = Status::MaxIterations;
  return aResult;
}

//! Solve constrained least squares with equality constraints only.
//!
//! Convenience function for C*X = S with min ||X - X0||.
//!
//! @param theCont constraint matrix C
//! @param theSecont right-hand side S
//! @param theStartingPoint initial point X0
//! @param theConfig algorithm configuration
//! @return UzawaResult with solution
inline UzawaResult UzawaEquality(const math_Matrix& theCont,
                                 const math_Vector& theSecont,
                                 const math_Vector& theStartingPoint,
                                 const UzawaConfig& theConfig = UzawaConfig())
{
  return Uzawa(theCont, theSecont, theStartingPoint, theCont.RowSize(), 0, theConfig);
}

} // namespace MathOpt

#endif // _MathOpt_Uzawa_HeaderFile

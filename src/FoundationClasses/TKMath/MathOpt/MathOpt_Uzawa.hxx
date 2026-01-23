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
  int                        NbIterations = 0;

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Configuration for Uzawa algorithm.
struct UzawaConfig
{
  double EpsLix        = 1.0e-6; //!< Tolerance for X convergence
  double EpsLic        = 1.0e-6; //!< Tolerance for dual variable convergence
  int    MaxIterations = 500;    //!< Maximum iterations
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
                         int                theNce,
                         int                theNci,
                         const UzawaConfig& theConfig = UzawaConfig())
{
  UzawaResult aResult;

  const int aNlig = theCont.RowNumber();
  const int aNcol = theCont.ColNumber();

  // Validate dimensions
  if (theSecont.Length() != aNlig || (theNce + theNci) != aNlig
      || theStartingPoint.Length() != aNcol)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int aRowLower = theCont.LowerRow();
  const int aColLower = theCont.LowerCol();
  const int aSecLower = theSecont.Lower();
  const int aXLower   = theStartingPoint.Lower();

  // Compute initial error: C*X0 - S
  math_Vector aErrinit(1, aNlig);
  for (int i = 1; i <= aNlig; ++i)
  {
    double aSum = 0.0;
    for (int j = 1; j <= aNcol; ++j)
    {
      aSum += theCont(i + aRowLower - 1, j + aColLower - 1) * theStartingPoint(j + aXLower - 1);
    }
    aErrinit(i) = aSum - theSecont(i + aSecLower - 1);
  }

  aResult.InitialError = aErrinit;

  // Initialize dual variables and error
  math_Vector aVardua(1, aNlig);
  math_Vector aErruza(1, aNcol, 0.0);
  math_Matrix aCTCinv(1, aNlig, 1, aNlig, 0.0);

  if (theNci == 0)
  {
    // Direct solution for equality constraints only
    aResult.NbIterations = 1;

    // Compute C * C^T (symmetric)
    for (int i = 1; i <= aNlig; ++i)
    {
      for (int j = 1; j <= i; ++j)
      {
        double aSum = 0.0;
        for (int k = 1; k <= aNcol; ++k)
        {
          aSum += theCont(i + aRowLower - 1, k + aColLower - 1)
                  * theCont(j + aRowLower - 1, k + aColLower - 1);
        }
        aCTCinv(i, j) = aSum;
        if (i != j)
        {
          aCTCinv(j, i) = aSum;
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

    // Compute dual variables: (C*C^T)^-1 * (C*X0 - S)
    for (int i = 1; i <= aNlig; ++i)
    {
      double aSum = 0.0;
      for (int j = 1; j <= aNlig; ++j)
      {
        aSum += aCTCinv(i, j) * aErrinit(j);
      }
      aVardua(i) = aSum;
    }

    // Compute error: -C^T * dual
    for (int i = 1; i <= aNcol; ++i)
    {
      double aSum = 0.0;
      for (int j = 1; j <= aNlig; ++j)
      {
        aSum -= theCont(j + aRowLower - 1, i + aColLower - 1) * aVardua(j);
      }
      aErruza(i) = aSum;
    }

    // Compute solution
    math_Vector aResul(1, aNcol);
    for (int i = 1; i <= aNcol; ++i)
    {
      aResul(i) = theStartingPoint(i + aXLower - 1) + aErruza(i);
    }

    aResult.Solution   = aResul;
    aResult.Dual       = aVardua;
    aResult.Error      = aErruza;
    aResult.InverseCTC = aCTCinv;
    aResult.Status     = Status::OK;
    return aResult;
  }

  // Iterative Uzawa for mixed equality/inequality constraints
  // Initialize dual variables
  for (int i = 1; i <= aNlig; ++i)
  {
    aVardua(i) = (i <= theNce) ? 0.0 : 1.0;
  }

  // Compute step size rho
  double aNormat = 0.0;
  for (int i = 1; i <= aNlig; ++i)
  {
    double aNormli = 0.0;
    for (int j = 1; j <= aNcol; ++j)
    {
      double aVal = theCont(i + aRowLower - 1, j + aColLower - 1);
      aNormli += aVal * aVal;
    }
    aNormat += aNormli;
  }
  const double aRho = 1.0 / (std::sqrt(2.0) * aNormat);

  // Uzawa iterations
  for (int anIter = 1; anIter <= theConfig.MaxIterations; ++anIter)
  {
    aResult.NbIterations = anIter;

    double aXmax = 0.0;

    // Update primal: X = X0 - C^T * dual
    for (int i = 1; i <= aNcol; ++i)
    {
      double aXprev = aErruza(i);
      double aSum   = 0.0;
      for (int j = 1; j <= aNlig; ++j)
      {
        aSum -= theCont(j + aRowLower - 1, i + aColLower - 1) * aVardua(j);
      }
      aErruza(i) = aSum;

      if (anIter > 1)
      {
        aXmax = std::max(aXmax, std::abs(aErruza(i) - aXprev));
      }
    }

    // Update dual variables and compute constraint error
    double aErrMax = 0.0;
    for (int i = 1; i <= aNlig; ++i)
    {
      // Constraint violation: C*X - S
      double aErr = aErrinit(i);
      for (int j = 1; j <= aNcol; ++j)
      {
        aErr += theCont(i + aRowLower - 1, j + aColLower - 1) * aErruza(j);
      }

      double aErr1;
      if (i <= theNce)
      {
        // Equality constraint: update freely
        aVardua(i) += aRho * aErr;
        aErr1 = std::abs(aRho * aErr);
      }
      else
      {
        // Inequality constraint: project to non-negative
        double aXmuPrev = aVardua(i);
        aVardua(i)      = std::max(0.0, aVardua(i) + aRho * aErr);
        aErr1           = std::abs(aVardua(i) - aXmuPrev);
      }
      aErrMax = std::max(aErrMax, aErr1);
    }

    // Check convergence
    if (anIter > 1)
    {
      if (aXmax <= theConfig.EpsLix)
      {
        if (aErrMax <= theConfig.EpsLic)
        {
          // Converged
          math_Vector aResul(1, aNcol);
          for (int i = 1; i <= aNcol; ++i)
          {
            aResul(i) = theStartingPoint(i + aXLower - 1) + aErruza(i);
          }

          aResult.Solution = aResul;
          aResult.Dual     = aVardua;
          aResult.Error    = aErruza;
          aResult.Status   = Status::OK;
          return aResult;
        }
        else
        {
          // Dual did not converge
          aResult.Status = Status::NotConverged;
          return aResult;
        }
      }
    }
  }

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
  return Uzawa(theCont, theSecont, theStartingPoint, theCont.RowNumber(), 0, theConfig);
}

} // namespace MathOpt

#endif // _MathOpt_Uzawa_HeaderFile

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

#ifndef _MathInteg_Set_HeaderFile
#define _MathInteg_Set_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <math_Vector.hxx>
#include <MathUtils_GaussKronrodWeights.hxx>

#include <cmath>

namespace MathInteg
{
using namespace MathUtils;

//! Result for vector function integration.
struct SetResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Vector> Values; //!< Integral of each component
  int                        NbEquations = 0;

  bool IsDone() const { return Status == MathUtils::Status::OK; }

  explicit operator bool() const { return IsDone(); }
};

//! Gauss-Legendre integration of a vector-valued function.
//!
//! Integrates F: R -> R^N over interval [Lower, Upper]:
//! Result[i] = integral_{Lower}^{Upper} F_i(x) dx
//!
//! Uses Gauss-Legendre quadrature applied to each component.
//!
//! Note: Only 1D input is supported (M=1). Multi-dimensional input
//! is not implemented in the legacy API.
//!
//! @tparam Func function type with:
//!   - int NbEquations() - number of output components
//!   - bool Value(const math_Vector& theX, math_Vector& theF)
//! @param theFunc vector-valued function to integrate
//! @param theLower lower bound
//! @param theUpper upper bound
//! @param theOrder integration order (max 61)
//! @return SetResult containing vector of integrals
template <typename Func>
SetResult GaussSet(Func& theFunc, double theLower, double theUpper, int theOrder)
{
  SetResult aResult;

  // Get function dimensions
  const int aNbEqua = theFunc.NbEquations();
  if (aNbEqua <= 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  aResult.NbEquations = aNbEqua;

  // Clamp order
  int aOrder = std::min(theOrder, 61);
  aOrder     = std::max(aOrder, 1);

  // Get Gauss points and weights
  math_Vector aGP(1, aOrder);
  math_Vector aGW(1, aOrder);
  GetOrderedGaussPointsAndWeights(aOrder, aGP, aGW);

  math_Vector aPoints(0, aOrder - 1);
  math_Vector aWeights(0, aOrder - 1);
  for (int i = 0; i < aOrder; ++i)
  {
    aPoints(i)  = aGP(i + 1);
    aWeights(i) = aGW(i + 1);
  }

  // Coordinate transformation
  const double aXm = 0.5 * (theLower + theUpper);
  const double aXr = 0.5 * (theUpper - theLower);

  // Initialize result vector
  math_Vector aVal(1, aNbEqua, 0.0);
  math_Vector aTval(1, 1); // Input vector (1D)
  math_Vector aFVal1(1, aNbEqua);
  math_Vector aFVal2(1, aNbEqua);

  const int aInd  = aOrder / 2;
  const int aInd1 = (aOrder + 1) / 2;

  // Handle odd order case (middle point)
  if (aInd1 > aInd)
  {
    aTval(1) = aXm;
    if (!theFunc.Value(aTval, aVal))
    {
      aResult.Status = Status::NotConverged;
      return aResult;
    }
    for (int j = 1; j <= aNbEqua; ++j)
    {
      aVal(j) *= aWeights(aInd1 - 1);
    }
  }

  // Symmetric Gauss quadrature
  for (int i = 0; i < aInd; ++i)
  {
    aTval(1) = aXm + aXr * aPoints(i);
    if (!theFunc.Value(aTval, aFVal1))
    {
      aResult.Status = Status::NotConverged;
      return aResult;
    }

    aTval(1) = aXm - aXr * aPoints(i);
    if (!theFunc.Value(aTval, aFVal2))
    {
      aResult.Status = Status::NotConverged;
      return aResult;
    }

    for (int j = 1; j <= aNbEqua; ++j)
    {
      aVal(j) += (aFVal1(j) + aFVal2(j)) * aWeights(i);
    }
  }

  // Scale by half-width
  for (int j = 1; j <= aNbEqua; ++j)
  {
    aVal(j) *= aXr;
  }

  aResult.Values = aVal;
  aResult.Status = Status::OK;
  return aResult;
}

//! Gauss-Legendre integration of vector function using math_Vector bounds.
//!
//! Convenience overload that extracts scalar bounds from math_Vector.
//!
//! @tparam Func function type
//! @param theFunc vector-valued function
//! @param theLower lower bound vector (only first element used)
//! @param theUpper upper bound vector (only first element used)
//! @param theOrder integration order
//! @return SetResult containing vector of integrals
template <typename Func>
SetResult GaussSet(Func&              theFunc,
                   const math_Vector& theLower,
                   const math_Vector& theUpper,
                   int                theOrder)
{
  return GaussSet(theFunc, theLower(theLower.Lower()), theUpper(theUpper.Lower()), theOrder);
}

} // namespace MathInteg

#endif // _MathInteg_Set_HeaderFile

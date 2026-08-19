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
#include <type_traits>

namespace MathInteg
{
using namespace MathUtils;

//! Largest order supported by vector-valued Gauss integration.
constexpr size_t THE_SET_GAUSS_MAX_ORDER = 61;

//! Result for vector function integration.
struct SetResult
{
  MathUtils::Status          Status = MathUtils::Status::NotConverged;
  std::optional<math_Vector> Values; //!< Integral of each component
  size_t                     NbEquations = 0;
  size_t                     NbPoints    = 0; //!< Number of callback evaluations

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
//!   - integral NbEquations() - positive number of output components
//!   - bool Value(const math_Vector& theX, math_Vector& theF)
//! @param theFunc vector-valued function to integrate
//! @param theLower lower bound
//! @param theUpper upper bound
//! @param theOrder integration order (max 61)
//! @return SetResult containing vector of integrals
template <typename Func>
SetResult GaussSet(Func& theFunc, double theLower, double theUpper, size_t theOrder)
{
  SetResult aResult;

  if (!std::isfinite(theLower) || !std::isfinite(theUpper) || theLower == theUpper
      || !std::isfinite(theUpper - theLower) || theOrder == 0
      || theOrder > THE_SET_GAUSS_MAX_ORDER)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Get function dimensions
  using NbEquationsType = decltype(theFunc.NbEquations());
  static_assert(std::is_integral_v<NbEquationsType>, "NbEquations() must return an integral type");
  const NbEquationsType aNbEquaValue = theFunc.NbEquations();
  if constexpr (std::is_signed_v<NbEquationsType>)
  {
    if (aNbEquaValue <= 0)
    {
      aResult.Status = Status::InvalidInput;
      return aResult;
    }
  }
  else if (aNbEquaValue == 0)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  const size_t aNbEqua = static_cast<size_t>(aNbEquaValue);

  aResult.NbEquations = aNbEqua;

  // Get Gauss points and weights
  math_Vector aPoints(theOrder);
  math_Vector aWeights(theOrder);
  if (!GetOrderedGaussPointsAndWeights(static_cast<int>(theOrder), aPoints, aWeights))
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Coordinate transformation
  const double aXr = 0.5 * (theUpper - theLower);
  const double aXm = theLower + aXr;

  // Initialize result vector
  math_Vector aVal(aNbEqua, 0.0);
  math_Vector aTval(size_t{1}); // Input vector (1D)
  math_Vector aFVal1(aNbEqua);
  math_Vector aFVal2(aNbEqua);

  const size_t aInd  = theOrder / 2;
  const size_t aInd1 = (theOrder + 1) / 2;

  // Handle odd order case (middle point)
  if (aInd1 > aInd)
  {
    aTval.ChangeAt(0) = aXm;
    aVal.Init(0.0);
    ++aResult.NbPoints;
    if (!theFunc.Value(aTval, aVal))
    {
      aResult.Status = Status::CallbackError;
      return aResult;
    }
    for (size_t j = 0; j < aNbEqua; ++j)
    {
      if (!std::isfinite(aVal.At(j)))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
      aVal.ChangeAt(j) *= aWeights.At(aInd1 - 1);
    }
  }

  // Symmetric Gauss quadrature
  for (size_t i = 0; i < aInd; ++i)
  {
    aTval.ChangeAt(0) = aXm + aXr * aPoints.At(i);
    aFVal1.Init(0.0);
    ++aResult.NbPoints;
    if (!theFunc.Value(aTval, aFVal1))
    {
      aResult.Status = Status::CallbackError;
      return aResult;
    }

    aTval.ChangeAt(0) = aXm - aXr * aPoints.At(i);
    aFVal2.Init(0.0);
    ++aResult.NbPoints;
    if (!theFunc.Value(aTval, aFVal2))
    {
      aResult.Status = Status::CallbackError;
      return aResult;
    }

    for (size_t j = 0; j < aNbEqua; ++j)
    {
      if (!std::isfinite(aFVal1.At(j)) || !std::isfinite(aFVal2.At(j)))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
      aVal.ChangeAt(j) += (aFVal1.At(j) + aFVal2.At(j)) * aWeights.At(i);
      if (!std::isfinite(aVal.At(j)))
      {
        aResult.Status = Status::NumericalError;
        return aResult;
      }
    }
  }

  // Scale by half-width
  for (size_t j = 0; j < aNbEqua; ++j)
  {
    aVal.ChangeAt(j) *= aXr;
    if (!std::isfinite(aVal.At(j)))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
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
                   size_t             theOrder)
{
  if (theLower.Size() < 1 || theUpper.Size() < 1)
  {
    SetResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  return GaussSet(theFunc, theLower.At(0), theUpper.At(0), theOrder);
}

} // namespace MathInteg

#endif // _MathInteg_Set_HeaderFile

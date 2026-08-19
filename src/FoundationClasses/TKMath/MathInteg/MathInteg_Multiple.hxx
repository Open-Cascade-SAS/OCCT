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

#ifndef _MathInteg_Multiple_HeaderFile
#define _MathInteg_Multiple_HeaderFile

#include <MathUtils_Types.hxx>
#include <MathUtils_Config.hxx>
#include <math_Vector.hxx>
#include <math_IntegerVector.hxx>
#include <MathUtils_GaussKronrodWeights.hxx>

#include <NCollection_DynamicArray.hxx>
#include <NCollection_LinearVector.hxx>

#include <cmath>

namespace MathInteg
{
using namespace MathUtils;

//! Largest per-dimension order supported by the Gauss weight generator.
constexpr size_t THE_MULTIPLE_GAUSS_MAX_ORDER = 61;

//! Configuration for multi-dimensional Gauss integration.
struct MultipleConfig
{
  size_t MaxOrder = THE_MULTIPLE_GAUSS_MAX_ORDER; //!< Maximum integration order per dimension
};

//! Gauss-Legendre integration of a multi-variable function.
//!
//! Computes the N-dimensional integral using tensor product of 1D
//! Gauss-Legendre quadrature:
//! I = integral_{Lower}^{Upper} F(x1,...,xN) dx1...dxN
//!
//! Uses recursive summation over all Gauss points in each dimension.
//!
//! @tparam Func function type with bool Value(const math_Vector&, double&)
//! @param theFunc N-dimensional function to integrate
//! @param theNVars number of variables
//! @param theLower lower bounds for each variable
//! @param theUpper upper bounds for each variable
//! @param theOrder integration order for each variable (max 61)
//! @param theConfig optional configuration
//! @return IntegResult containing the integral value
template <typename Func>
IntegResult GaussMultiple(Func&                     theFunc,
                          size_t                    theNVars,
                          const math_Vector&        theLower,
                          const math_Vector&        theUpper,
                          const math_IntegerVector& theOrder,
                          const MultipleConfig&     theConfig = MultipleConfig())
{
  IntegResult aResult;

  // Validate inputs
  if (theNVars == 0 || theLower.Size() != theNVars || theUpper.Size() != theNVars
      || theOrder.Size() != theNVars || theConfig.MaxOrder == 0
      || theConfig.MaxOrder > THE_MULTIPLE_GAUSS_MAX_ORDER)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  // Validate all dimensions before allocating quadrature storage.
  size_t                           aNbEvaluations = 1;
  NCollection_LinearVector<size_t> anOrders;
  for (size_t i = 0; i < theNVars; ++i)
  {
    const double aLower       = theLower.At(i);
    const double anUpper      = theUpper.At(i);
    const int    anOrderValue = theOrder.At(i);
    if (!std::isfinite(aLower) || !std::isfinite(anUpper) || aLower == anUpper
        || !std::isfinite(anUpper - aLower) || anOrderValue < 1)
    {
      aResult.Status = Status::InvalidInput;
      return aResult;
    }
    const size_t anOrder = static_cast<size_t>(anOrderValue);
    if (anOrder > theConfig.MaxOrder || anOrder > THE_MULTIPLE_GAUSS_MAX_ORDER)
    {
      aResult.Status = Status::InvalidInput;
      return aResult;
    }
    anOrders.Append(anOrder);
    aNbEvaluations *= anOrder;
  }

  // Compute midpoints and half-widths for coordinate transformation
  math_Vector aXm(theNVars);
  math_Vector aXr(theNVars);
  for (size_t i = 0; i < theNVars; ++i)
  {
    aXr.ChangeAt(i) = 0.5 * (theUpper.At(i) - theLower.At(i));
    aXm.ChangeAt(i) = theLower.At(i) + aXr.At(i);
  }

  // Get Gauss points and weights for each variable
  // Use NCollection_DynamicArray since math_Vector has no default constructor
  NCollection_DynamicArray<math_Vector> aGaussPoints(std::min(theNVars, size_t{8}));
  NCollection_DynamicArray<math_Vector> aGaussWeights(std::min(theNVars, size_t{8}));

  for (size_t i = 0; i < theNVars; ++i)
  {
    const size_t anOrder = anOrders.Value(i);
    aGaussPoints.Append(math_Vector(anOrder));
    aGaussWeights.Append(math_Vector(anOrder));

    math_Vector aGP(anOrder);
    math_Vector aGW(anOrder);
    if (!GetOrderedGaussPointsAndWeights(static_cast<int>(anOrder), aGP, aGW))
    {
      aResult.Status = Status::InvalidInput;
      return aResult;
    }

    for (size_t k = 0; k < anOrder; ++k)
    {
      aGaussPoints.ChangeValue(i).ChangeAt(k)  = aGP.At(k);
      aGaussWeights.ChangeValue(i).ChangeAt(k) = aGW.At(k);
    }
  }

  double                           aVal = 0.0;
  math_Vector                      aX(theNVars);
  NCollection_LinearVector<size_t> anIndices;
  for (size_t i = 0; i < theNVars; ++i)
  {
    anIndices.Append(0);
  }

  for (size_t anEval = 0; anEval < aNbEvaluations; ++anEval)
  {
    double aWeight = 1.0;
    for (size_t j = 0; j < theNVars; ++j)
    {
      const size_t anIndex = anIndices.Value(j);
      aX.ChangeAt(j)       = aXm.At(j) + aXr.At(j) * aGaussPoints.Value(j).At(anIndex);
      aWeight *= aGaussWeights.Value(j).At(anIndex);
    }

    double aFunctionValue = 0.0;
    ++aResult.NbPoints;
    if (!theFunc.Value(aX, aFunctionValue))
    {
      aResult.Status = Status::CallbackError;
      return aResult;
    }
    if (!std::isfinite(aFunctionValue))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
    aVal += aWeight * aFunctionValue;
    if (!std::isfinite(aVal))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }

    for (size_t j = theNVars; j-- > 0;)
    {
      if (++anIndices.ChangeValue(j) < anOrders.Value(j))
      {
        break;
      }
      anIndices.ChangeValue(j) = 0;
    }
  }

  // Scale by half-widths
  for (size_t i = 0; i < theNVars; ++i)
  {
    aVal *= aXr.At(i);
    if (!std::isfinite(aVal))
    {
      aResult.Status = Status::NumericalError;
      return aResult;
    }
  }

  aResult.Value        = aVal;
  aResult.Status       = Status::OK;
  aResult.NbIterations = 1;
  return aResult;
}

//! Gauss-Legendre integration with uniform order for all variables.
//!
//! @tparam Func function type with bool Value(const math_Vector&, double&)
//! @param theFunc N-dimensional function to integrate
//! @param theNVars number of variables
//! @param theLower lower bounds for each variable
//! @param theUpper upper bounds for each variable
//! @param theOrder integration order for all variables
//! @return IntegResult containing the integral value
template <typename Func>
IntegResult GaussMultipleUniform(Func&              theFunc,
                                 size_t             theNVars,
                                 const math_Vector& theLower,
                                 const math_Vector& theUpper,
                                 size_t             theOrder)
{
  if (theNVars == 0 || theOrder == 0 || theOrder > THE_MULTIPLE_GAUSS_MAX_ORDER)
  {
    IntegResult aResult;
    aResult.Status = Status::InvalidInput;
    return aResult;
  }
  math_IntegerVector aOrders(theNVars, static_cast<int>(theOrder));
  return GaussMultiple(theFunc, theNVars, theLower, theUpper, aOrders);
}

} // namespace MathInteg

#endif // _MathInteg_Multiple_HeaderFile

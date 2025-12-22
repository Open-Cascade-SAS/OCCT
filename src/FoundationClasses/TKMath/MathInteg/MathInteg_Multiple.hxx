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
#include <math_Matrix.hxx>
#include <MathUtils_GaussKronrodWeights.hxx>

#include <NCollection_Vector.hxx>

#include <cmath>
#include <functional>

namespace MathInteg
{
using namespace MathUtils;

//! Configuration for multi-dimensional Gauss integration.
struct MultipleConfig
{
  int MaxOrder = 61; //!< Maximum integration order per dimension
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
                          int                       theNVars,
                          const math_Vector&        theLower,
                          const math_Vector&        theUpper,
                          const math_IntegerVector& theOrder,
                          const MultipleConfig&     theConfig = MultipleConfig())
{
  IntegResult aResult;

  // Validate inputs
  if (theNVars <= 0 || theLower.Length() != theNVars || theUpper.Length() != theNVars
      || theOrder.Length() != theNVars)
  {
    aResult.Status = Status::InvalidInput;
    return aResult;
  }

  const int aLowerL  = theLower.Lower();
  const int aLowerU  = theUpper.Lower();
  const int aLowerOr = theOrder.Lower();

  // Find maximum order and clamp orders
  math_IntegerVector aOrd(0, theNVars - 1);
  int                aMaxOrder = 0;
  for (int i = 0; i < theNVars; ++i)
  {
    aOrd(i) = std::min(theOrder(i + aLowerOr), theConfig.MaxOrder);
    aOrd(i) = std::max(aOrd(i), 1);
    if (aOrd(i) > aMaxOrder)
    {
      aMaxOrder = aOrd(i);
    }
  }

  // Compute midpoints and half-widths for coordinate transformation
  math_Vector aXm(0, theNVars - 1);
  math_Vector aXr(0, theNVars - 1);
  for (int i = 0; i < theNVars; ++i)
  {
    aXm(i) = 0.5 * (theLower(i + aLowerL) + theUpper(i + aLowerU));
    aXr(i) = 0.5 * (theUpper(i + aLowerU) - theLower(i + aLowerL));
  }

  // Get Gauss points and weights for each variable
  // Use NCollection_Vector since math_Vector has no default constructor
  NCollection_Vector<math_Vector> aGaussPoints;
  NCollection_Vector<math_Vector> aGaussWeights;

  for (int i = 0; i < theNVars; ++i)
  {
    aGaussPoints.Append(math_Vector(0, aOrd(i) - 1));
    aGaussWeights.Append(math_Vector(0, aOrd(i) - 1));

    math_Vector aGP(1, aOrd(i));
    math_Vector aGW(1, aOrd(i));
    GetOrderedGaussPointsAndWeights(aOrd(i), aGP, aGW);

    for (int k = 0; k < aOrd(i); ++k)
    {
      aGaussPoints.ChangeValue(i)(k)  = aGP(k + 1);
      aGaussWeights.ChangeValue(i)(k) = aGW(k + 1);
    }
  }

  // Recursive integration using lambda
  double      aVal = 0.0;
  math_Vector aX(1, theNVars);
  math_Vector aDx(1, theNVars);

  // Index array for iteration
  math_IntegerVector aInc(0, theNVars - 1, 0);

  // Iterative approach using index array
  std::function<bool(int)> aRecurse = [&](int theN) -> bool {
    if (theN == theNVars)
    {
      // Compute function value at current Gauss point
      for (int j = 0; j < theNVars; ++j)
      {
        aDx(j + 1) = aXr(j) * aGaussPoints.Value(j)(aInc(j));
        aX(j + 1)  = aXm(j) + aDx(j + 1);
      }

      double aF1;
      if (!theFunc.Value(aX, aF1))
      {
        return false;
      }

      // Compute product of weights
      double aWeight = 1.0;
      for (int j = 0; j < theNVars; ++j)
      {
        aWeight *= aGaussWeights.Value(j)(aInc(j));
      }

      aVal += aWeight * aF1;
      return true;
    }

    // Iterate over Gauss points for variable theN
    for (aInc(theN) = 0; aInc(theN) < aOrd(theN); ++aInc(theN))
    {
      if (!aRecurse(theN + 1))
      {
        return false;
      }
    }
    return true;
  };

  if (!aRecurse(0))
  {
    aResult.Status = Status::NotConverged;
    return aResult;
  }

  // Scale by half-widths
  for (int i = 0; i < theNVars; ++i)
  {
    aVal *= aXr(i);
  }

  aResult.Value  = aVal;
  aResult.Status = Status::OK;
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
                                 int                theNVars,
                                 const math_Vector& theLower,
                                 const math_Vector& theUpper,
                                 int                theOrder)
{
  math_IntegerVector aOrders(0, theNVars - 1, theOrder);
  return GaussMultiple(theFunc, theNVars, theLower, theUpper, aOrders);
}

} // namespace MathInteg

#endif // _MathInteg_Multiple_HeaderFile

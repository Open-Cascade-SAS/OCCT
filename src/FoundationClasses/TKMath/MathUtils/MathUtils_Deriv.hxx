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

#ifndef _MathUtils_Deriv_HeaderFile
#define _MathUtils_Deriv_HeaderFile

#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <MathUtils_Core.hxx>

#include <cmath>

//! Core utilities for modern math solvers.
namespace MathUtils
{

namespace Utils
{
inline bool Perturbations(double theX, double theStep, double& theMinus, double& thePlus)
{
  if (!std::isfinite(theX) || !std::isfinite(theStep) || theStep <= 0.0)
  {
    return false;
  }
  thePlus  = theX + theStep;
  theMinus = theX - theStep;
  if (thePlus == theX)
  {
    thePlus = std::nextafter(theX, std::numeric_limits<double>::infinity());
  }
  if (theMinus == theX)
  {
    theMinus = std::nextafter(theX, -std::numeric_limits<double>::infinity());
  }
  return std::isfinite(theMinus) && std::isfinite(thePlus) && theMinus < theX && theX < thePlus;
}

inline bool IsFiniteVector(const math_Vector& theVector)
{
  for (size_t i = 0; i < theVector.Size(); ++i)
  {
    if (!std::isfinite(theVector.At(i)))
    {
      return false;
    }
  }
  return true;
}
} // namespace Utils

//! Central difference derivative approximation for scalar functions.
//! f'(x) ~= (f(x+h) - f(x-h)) / (2h)
//! Accuracy: O(h^2)
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to differentiate
//! @param theX point at which to evaluate derivative
//! @param theDeriv computed derivative value
//! @param theStep step size (default 1e-8)
//! @return true if successful
template <typename Function>
bool CentralDifference(Function& theFunc, double theX, double& theDeriv, double theStep = 1.0e-8)
{
  double aMinus = 0.0;
  double aPlus  = 0.0;
  if (!Utils::Perturbations(theX, theStep, aMinus, aPlus))
  {
    return false;
  }
  double aFPlus  = 0.0;
  double aFMinus = 0.0;

  if (!theFunc.Value(aPlus, aFPlus) || !std::isfinite(aFPlus))
  {
    return false;
  }
  if (!theFunc.Value(aMinus, aFMinus) || !std::isfinite(aFMinus))
  {
    return false;
  }

  const double aDeriv = (aFPlus - aFMinus) / (aPlus - aMinus);
  if (!std::isfinite(aDeriv))
  {
    return false;
  }
  theDeriv = aDeriv;
  return true;
}

//! Forward difference derivative (one-sided).
//! f'(x) ~= (f(x+h) - f(x)) / h
//! Accuracy: O(h)
//! Useful when central difference is not possible (e.g., at boundaries).
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to differentiate
//! @param theX point at which to evaluate derivative
//! @param theFx function value at theX (if already computed)
//! @param theDeriv computed derivative value
//! @param theStep step size (default 1e-8)
//! @return true if successful
template <typename Function>
bool ForwardDifference(Function& theFunc,
                       double    theX,
                       double    theFx,
                       double&   theDeriv,
                       double    theStep = 1.0e-8)
{
  if (!std::isfinite(theX) || !std::isfinite(theFx) || !std::isfinite(theStep) || theStep <= 0.0)
  {
    return false;
  }
  double aXPlus = theX + theStep;
  if (aXPlus == theX)
  {
    aXPlus = std::nextafter(theX, std::numeric_limits<double>::infinity());
  }
  if (!std::isfinite(aXPlus) || aXPlus <= theX)
  {
    return false;
  }
  double aFPlus = 0.0;

  if (!theFunc.Value(aXPlus, aFPlus) || !std::isfinite(aFPlus))
  {
    return false;
  }

  const double aDeriv = (aFPlus - theFx) / (aXPlus - theX);
  if (!std::isfinite(aDeriv))
  {
    return false;
  }
  theDeriv = aDeriv;
  return true;
}

//! Numerical gradient using central differences for N-D functions.
//! df/dx[i] ~= (f(x + h[i]*e[i]) - f(x - h[i]*e[i])) / (2*h[i])
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to differentiate
//! @param theX point at which to evaluate gradient (temporarily modified)
//! @param theGrad output gradient vector (same dimension as theX)
//! @param theStep step size (default 1e-8)
//! @return true if successful
template <typename Function>
bool NumericalGradient(Function&    theFunc,
                       math_Vector& theX,
                       math_Vector& theGrad,
                       double       theStep = 1.0e-8)
{
  if (theX.Size() != theGrad.Size() || !Utils::IsFiniteVector(theX) || !std::isfinite(theStep)
      || theStep <= 0.0)
  {
    return false;
  }
  for (size_t i = 0; i < theX.Size(); ++i)
  {
    const double aXi    = theX.At(i);
    double       aMinus = 0.0;
    double       aPlus  = 0.0;
    if (!Utils::Perturbations(aXi, theStep, aMinus, aPlus))
    {
      return false;
    }
    double aFPlus  = 0.0;
    double aFMinus = 0.0;

    // Forward perturbation
    theX.ChangeAt(i) = aPlus;
    if (!theFunc.Value(theX, aFPlus) || !std::isfinite(aFPlus))
    {
      theX.ChangeAt(i) = aXi;
      return false;
    }

    // Backward perturbation
    theX.ChangeAt(i) = aMinus;
    if (!theFunc.Value(theX, aFMinus) || !std::isfinite(aFMinus))
    {
      theX.ChangeAt(i) = aXi;
      return false;
    }

    // Restore original value
    theX.ChangeAt(i) = aXi;

    // Central difference
    const double aDeriv = (aFPlus - aFMinus) / (aPlus - aMinus);
    if (!std::isfinite(aDeriv))
    {
      return false;
    }
    theGrad.ChangeAt(i) = aDeriv;
  }

  return true;
}

//! Numerical gradient with adaptive step size.
//! Uses step proportional to |x[i]| for better conditioning.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc function to differentiate
//! @param theX point at which to evaluate gradient
//! @param theGrad output gradient vector
//! @param theRelStep relative step size (default 1e-8)
//! @return true if successful
template <typename Function>
bool NumericalGradientAdaptive(Function&    theFunc,
                               math_Vector& theX,
                               math_Vector& theGrad,
                               double       theRelStep = 1.0e-8)
{
  if (theX.Size() != theGrad.Size() || !Utils::IsFiniteVector(theX)
      || !std::isfinite(theRelStep) || theRelStep <= 0.0)
  {
    return false;
  }
  for (size_t i = 0; i < theX.Size(); ++i)
  {
    const double aXi = theX.At(i);
    // Adaptive step: larger for larger |x|, with minimum floor
    const double aStep  = theRelStep * std::max(1.0, std::abs(aXi));
    double       aMinus = 0.0;
    double       aPlus  = 0.0;
    if (!Utils::Perturbations(aXi, aStep, aMinus, aPlus))
    {
      return false;
    }

    double aFPlus  = 0.0;
    double aFMinus = 0.0;

    theX.ChangeAt(i) = aPlus;
    if (!theFunc.Value(theX, aFPlus) || !std::isfinite(aFPlus))
    {
      theX.ChangeAt(i) = aXi;
      return false;
    }

    theX.ChangeAt(i) = aMinus;
    if (!theFunc.Value(theX, aFMinus) || !std::isfinite(aFMinus))
    {
      theX.ChangeAt(i) = aXi;
      return false;
    }

    theX.ChangeAt(i)    = aXi;
    const double aDeriv = (aFPlus - aFMinus) / (aPlus - aMinus);
    if (!std::isfinite(aDeriv))
    {
      return false;
    }
    theGrad.ChangeAt(i) = aDeriv;
  }

  return true;
}

//! Numerical Jacobian matrix for vector-valued functions.
//! J[i,j] = dF[i]/dx[j] ~= (F[i](x + h[j]*e[j]) - F[i](x - h[j]*e[j])) / (2*h[j])
//!
//! @tparam Function type with Value(const math_Vector& x, math_Vector& F) method
//! @param theFunc vector-valued function F: R^n -> R^m
//! @param theX point at which to evaluate Jacobian (n-dimensional)
//! @param theJac output Jacobian matrix (m x n)
//! @param theStep step size (default 1e-8)
//! @return true if successful
template <typename Function>
bool NumericalJacobian(Function&    theFunc,
                       math_Vector& theX,
                       math_Matrix& theJac,
                       double       theStep = 1.0e-8)
{
  const size_t aNbRows = theJac.RowSize();
  const size_t aNbCols = theJac.ColSize();

  if (theX.Size() != aNbCols || aNbRows == 0 || !Utils::IsFiniteVector(theX)
      || !std::isfinite(theStep) || theStep <= 0.0)
  {
    return false;
  }

  math_Vector aFPlus(aNbRows);
  math_Vector aFMinus(aNbRows);

  for (size_t j = 0; j < aNbCols; ++j)
  {
    const double aXj    = theX.At(j);
    double       aMinus = 0.0;
    double       aPlus  = 0.0;
    if (!Utils::Perturbations(aXj, theStep * std::max(1.0, std::abs(aXj)), aMinus, aPlus))
    {
      return false;
    }

    // Forward perturbation
    theX.ChangeAt(j) = aPlus;
    if (!theFunc.Value(theX, aFPlus) || !Utils::IsFiniteVector(aFPlus))
    {
      theX.ChangeAt(j) = aXj;
      return false;
    }

    // Backward perturbation
    theX.ChangeAt(j) = aMinus;
    if (!theFunc.Value(theX, aFMinus) || !Utils::IsFiniteVector(aFMinus))
    {
      theX.ChangeAt(j) = aXj;
      return false;
    }

    // Restore
    theX.ChangeAt(j) = aXj;

    // Fill column of Jacobian
    for (size_t i = 0; i < aNbRows; ++i)
    {
      const double aDeriv = (aFPlus.At(i) - aFMinus.At(i)) / (aPlus - aMinus);
      if (!std::isfinite(aDeriv))
      {
        return false;
      }
      theJac.ChangeAt(i, j) = aDeriv;
    }
  }

  return true;
}

//! Numerical Hessian matrix using finite differences.
//! H[i,j] = d^2f/dx[i]dx[j]
//! Uses central differences on gradient.
//!
//! @tparam Function type with Value(const math_Vector&, double&) method
//! @param theFunc scalar function f: R^n -> R
//! @param theX point at which to evaluate Hessian (n-dimensional)
//! @param theHess output Hessian matrix (n x n, symmetric)
//! @param theStep step size (default 1e-5, larger than gradient step)
//! @return true if successful
template <typename Function>
bool NumericalHessian(Function&    theFunc,
                      math_Vector& theX,
                      math_Matrix& theHess,
                      double       theStep = 1.0e-5)
{
  if (theHess.RowSize() != theX.Size() || theHess.ColSize() != theX.Size()
      || !Utils::IsFiniteVector(theX) || !std::isfinite(theStep) || theStep <= 0.0)
  {
    return false;
  }

  double aFx = 0.0;
  if (!theFunc.Value(theX, aFx) || !std::isfinite(aFx))
  {
    return false;
  }

  // Diagonal elements: d^2f/dx[i]^2 ~= (f(x+h[i]*e[i]) - 2f(x) + f(x-h[i]*e[i])) / h^2
  for (size_t i = 0; i < theX.Size(); ++i)
  {
    const double aXi    = theX.At(i);
    double       aMinus = 0.0;
    double       aPlus  = 0.0;
    if (!Utils::Perturbations(aXi, theStep * std::max(1.0, std::abs(aXi)), aMinus, aPlus))
    {
      return false;
    }
    double aFPlus  = 0.0;
    double aFMinus = 0.0;

    theX.ChangeAt(i) = aPlus;
    if (!theFunc.Value(theX, aFPlus) || !std::isfinite(aFPlus))
    {
      theX.ChangeAt(i) = aXi;
      return false;
    }

    theX.ChangeAt(i) = aMinus;
    if (!theFunc.Value(theX, aFMinus) || !std::isfinite(aFMinus))
    {
      theX.ChangeAt(i) = aXi;
      return false;
    }

    theX.ChangeAt(i) = aXi;

    const double aHPlus  = aPlus - aXi;
    const double aHMinus = aXi - aMinus;
    const double aD2 =
      2.0 * ((aFPlus - aFx) / aHPlus - (aFx - aFMinus) / aHMinus) / (aHPlus + aHMinus);
    if (!std::isfinite(aD2))
    {
      return false;
    }
    theHess.ChangeAt(i, i) = aD2;
  }

  // Off-diagonal elements: d^2f/dx[i]dx[j]
  // ~= (f(x+h[i]*e[i]+h[j]*e[j]) - f(x+h[i]*e[i]-h[j]*e[j])
  //    - f(x-h[i]*e[i]+h[j]*e[j]) + f(x-h[i]*e[i]-h[j]*e[j])) / (4h^2)
  for (size_t i = 0; i < theX.Size(); ++i)
  {
    for (size_t j = i + 1; j < theX.Size(); ++j)
    {
      const double aXi     = theX.At(i);
      const double aXj     = theX.At(j);
      double       aMinusI = 0.0, aPlusI = 0.0, aMinusJ = 0.0, aPlusJ = 0.0;
      if (!Utils::Perturbations(aXi, theStep * std::max(1.0, std::abs(aXi)), aMinusI, aPlusI)
          || !Utils::Perturbations(aXj, theStep * std::max(1.0, std::abs(aXj)), aMinusJ, aPlusJ))
      {
        return false;
      }
      double aFpp = 0.0, aFpm = 0.0, aFmp = 0.0, aFmm = 0.0;

      // f(x + h[i]*e[i] + h[j]*e[j])
      theX.ChangeAt(i) = aPlusI;
      theX.ChangeAt(j) = aPlusJ;
      if (!theFunc.Value(theX, aFpp) || !std::isfinite(aFpp))
      {
        theX.ChangeAt(i) = aXi;
        theX.ChangeAt(j) = aXj;
        return false;
      }

      // f(x + h[i]*e[i] - h[j]*e[j])
      theX.ChangeAt(j) = aMinusJ;
      if (!theFunc.Value(theX, aFpm) || !std::isfinite(aFpm))
      {
        theX.ChangeAt(i) = aXi;
        theX.ChangeAt(j) = aXj;
        return false;
      }

      // f(x - h[i]*e[i] - h[j]*e[j])
      theX.ChangeAt(i) = aMinusI;
      if (!theFunc.Value(theX, aFmm) || !std::isfinite(aFmm))
      {
        theX.ChangeAt(i) = aXi;
        theX.ChangeAt(j) = aXj;
        return false;
      }

      // f(x - h[i]*e[i] + h[j]*e[j])
      theX.ChangeAt(j) = aPlusJ;
      if (!theFunc.Value(theX, aFmp) || !std::isfinite(aFmp))
      {
        theX.ChangeAt(i) = aXi;
        theX.ChangeAt(j) = aXj;
        return false;
      }

      // Restore
      theX.ChangeAt(i) = aXi;
      theX.ChangeAt(j) = aXj;

      const double aHij  = (aFpp - aFpm - aFmp + aFmm) / ((aPlusI - aMinusI) * (aPlusJ - aMinusJ));
      if (!std::isfinite(aHij))
      {
        return false;
      }

      // Symmetric
      theHess.ChangeAt(i, j) = aHij;
      theHess.ChangeAt(j, i) = aHij;
    }
  }

  return true;
}

//! Second derivative using central difference.
//! f''(x) ~= (f(x+h) - 2f(x) + f(x-h)) / h^2
//!
//! @tparam Function type with Value(double theX, double& theF) method
//! @param theFunc function to differentiate
//! @param theX point at which to evaluate second derivative
//! @param theFx function value at theX (if already computed)
//! @param theD2f computed second derivative value
//! @param theStep step size (default 1e-5)
//! @return true if successful
template <typename Function>
bool SecondDerivative(Function& theFunc,
                      double    theX,
                      double    theFx,
                      double&   theD2f,
                      double    theStep = 1.0e-5)
{
  double aMinus = 0.0;
  double aPlus  = 0.0;
  if (!std::isfinite(theFx)
      || !Utils::Perturbations(theX, theStep * std::max(1.0, std::abs(theX)), aMinus, aPlus))
  {
    return false;
  }
  double aFPlus  = 0.0;
  double aFMinus = 0.0;

  if (!theFunc.Value(aPlus, aFPlus) || !std::isfinite(aFPlus))
  {
    return false;
  }
  if (!theFunc.Value(aMinus, aFMinus) || !std::isfinite(aFMinus))
  {
    return false;
  }

  const double aHPlus  = aPlus - theX;
  const double aHMinus = theX - aMinus;
  const double aD2 =
    2.0 * ((aFPlus - theFx) / aHPlus - (theFx - aFMinus) / aHMinus) / (aHPlus + aHMinus);
  if (!std::isfinite(aD2))
  {
    return false;
  }
  theD2f = aD2;
  return true;
}

} // namespace MathUtils

#endif // _MathUtils_Deriv_HeaderFile

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

#ifndef _ExtremaPC_DistanceFunction_HeaderFile
#define _ExtremaPC_DistanceFunction_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Distance function for point-curve extrema computation.
//!
//! Implements the normalized stationarity function
//! F(u) = (C(u) - P) . C'(u) / |C'(u)| and its derivative.
//! Normalization gives F geometric length units and makes tolerance behavior
//! independent of regular curve reparameterization.
//!
//! The roots of F(u) = 0 correspond to the extrema (local minima and maxima)
//! of the squared distance function D(u) = ||C(u) - P||^2.
//!
//! This class provides the interface required by MathRoot::FindAllRootsWithDerivative.
class ExtremaPC_DistanceFunction
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor.
  //! @param theCurve the curve adaptor
  //! @param theP the query point
  ExtremaPC_DistanceFunction(const Adaptor3d_Curve& theCurve, const gp_Pnt& theP)
      : myCurve(&theCurve),
        myP(theP)
  {
  }

  //! Computes F(u) = (C(u) - P) . C'(u) / |C'(u)|.
  //! @param theU parameter value
  //! @param theF output: function value
  //! @return true if computation succeeded
  bool Value(double theU, double& theF) const
  {
    gp_Pnt aPt;
    gp_Vec aD1;
    myCurve->D1(theU, aPt, aD1);

    gp_Vec aVec(myP, aPt);
    const double aSpeed = aD1.Magnitude();
    if (aSpeed <= gp::Resolution())
    {
      // The normalized function has no defined direction at a singular parameter.
      // Its continuous one-sided signs are used later for extrema classification.
      theF = 0.0;
      return true;
    }
    theF = aVec.Dot(aD1) / aSpeed;
    return true;
  }

  //! Computes F(u) and F'(u).
  //! Computes the normalized stationarity function and its derivative.
  //! @param theU parameter value
  //! @param theF output: function value
  //! @param theDF output: derivative value
  //! @return true if computation succeeded
  bool Values(double theU, double& theF, double& theDF) const
  {
    gp_Pnt aPt;
    gp_Vec aD1, aD2;
    myCurve->D2(theU, aPt, aD1, aD2);

    gp_Vec aVec(myP, aPt);
    const double aSpeed = aD1.Magnitude();
    if (aSpeed <= gp::Resolution())
    {
      theF  = 0.0;
      theDF = 0.0;
      return true;
    }
    const double aRawF = aVec.Dot(aD1);
    theF                = aRawF / aSpeed;
    theDF = (aD1.Dot(aD1) + aVec.Dot(aD2)) / aSpeed
            - aRawF * aD1.Dot(aD2) / (aSpeed * aSpeed * aSpeed);
    return true;
  }

  //! Computes the derivative F'(u).
  //! @param theU parameter value
  //! @param theDF output: derivative value
  //! @return true if computation succeeded
  bool Derivative(double theU, double& theDF) const
  {
    double aF;
    return Values(theU, aF, theDF);
  }

  //! Returns the first parameter of the curve.
  double FirstParameter() const { return myCurve->FirstParameter(); }

  //! Returns the last parameter of the curve.
  double LastParameter() const { return myCurve->LastParameter(); }

  //! Returns the query point.
  const gp_Pnt& Point() const { return myP; }

  //! Returns the curve.
  const Adaptor3d_Curve& Curve() const { return *myCurve; }

private:
  const Adaptor3d_Curve* myCurve; //!< Curve adaptor (not owned)
  gp_Pnt                 myP;     //!< Query point
};

#endif // _ExtremaPC_DistanceFunction_HeaderFile

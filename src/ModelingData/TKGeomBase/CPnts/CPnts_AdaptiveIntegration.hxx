// Copyright (c) 2026 OPEN CASCADE SAS
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

#ifndef _CPnts_AdaptiveIntegration_HeaderFile
#define _CPnts_AdaptiveIntegration_HeaderFile

#include <cmath>
#include <math_Function.hxx>
#include <math_GaussSingleIntegration.hxx>

//! Relative agreement two successive subdivision levels must reach.
constexpr double CPnts_IntegrationTolerance = 1.e-9;

//! Ceiling on the equal parts the range may be split into.
constexpr int CPnts_IntegrationMaxParts = 512;

//! One theOrder-point Gauss quadrature of theF over [theU1, theU2].
inline bool CPnts_GaussIntegrate(math_Function& theF,
                                 const double   theU1,
                                 const double   theU2,
                                 const int      theOrder,
                                 const double*  theTol,
                                 double&        theValue)
{
  math_GaussSingleIntegration anIntegral =
    theTol != nullptr ? math_GaussSingleIntegration(theF, theU1, theU2, theOrder, *theTol)
                      : math_GaussSingleIntegration(theF, theU1, theU2, theOrder);
  if (!anIntegral.IsDone())
  {
    return false;
  }
  theValue = anIntegral.Value();
  return true;
}

//! Integrate theF over [theU1, theU2], splitting the range into two, four, ... equal parts until
//! two successive levels agree to CPnts_IntegrationTolerance relatively.
//!
//! A single fixed-order Gauss rule cannot resolve an integrand that varies sharply across the
//! range it covers, and the arc-length integrand |C'(u)| does: over a whole ellipse it has four
//! extrema, and the order-10 rule order() gives a conic reads a 1 x 0.05 ellipse 1.7% long. The
//! error follows the width of one integration interval rather than the curve's type, so the same
//! rule applied to each GeomAbs_CN span of a B-spline is out by 6.0e-5 on a 5-point interpolation.
inline bool CPnts_AdaptiveIntegrate(math_Function& theF,
                                    const double   theU1,
                                    const double   theU2,
                                    const int      theOrder,
                                    const double*  theTol,
                                    double&        theValue)
{
  double aPrevious = 0.;
  if (!CPnts_GaussIntegrate(theF, theU1, theU2, theOrder, theTol, aPrevious))
  {
    return false;
  }
  for (int aNbParts = 2; aNbParts <= CPnts_IntegrationMaxParts; aNbParts *= 2)
  {
    const double aStep  = (theU2 - theU1) / aNbParts;
    double       aTotal = 0.;
    for (int aPart = 0; aPart < aNbParts; ++aPart)
    {
      double aPartValue = 0.;
      if (!CPnts_GaussIntegrate(theF,
                                theU1 + aPart * aStep,
                                aPart + 1 == aNbParts ? theU2 : theU1 + (aPart + 1) * aStep,
                                theOrder,
                                theTol,
                                aPartValue))
      {
        return false;
      }
      aTotal += aPartValue;
    }
    if (std::abs(aTotal - aPrevious) <= CPnts_IntegrationTolerance * std::abs(aTotal))
    {
      theValue = aTotal;
      return true;
    }
    aPrevious = aTotal;
  }
  theValue = aPrevious;
  return true;
}

#endif // _CPnts_AdaptiveIntegration_HeaderFile

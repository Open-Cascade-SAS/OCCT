// Created on: 1995-01-12
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// pmn -> 17/01/1996 added : Continuity, (Nb)Interals, D2, Trim

#include <Law_Function.hxx>
#include <Law_Linear.hxx>
#include <Standard_Type.hxx>

#include <algorithm>
#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(Law_Linear, Law_Function)

namespace
{
//==================================================================================================

double scaledDifference(const double theLeft, const double theRight, int& theExponent)
{
  std::frexp(std::max(std::abs(theLeft), std::abs(theRight)), &theExponent);
  return std::scalbn(theLeft, -theExponent) - std::scalbn(theRight, -theExponent);
}

//==================================================================================================

double linearValue(const double theParameter,
                   const double theFirst,
                   const double theLast,
                   const double theFirstValue,
                   const double theLastValue)
{
  if (theFirst == theLast)
  {
    return ((theParameter - theFirst) * theLastValue + (theLast - theParameter) * theFirstValue)
           / (theLast - theFirst);
  }
  if (theParameter == theFirst || theFirstValue == theLastValue)
  {
    return theFirstValue;
  }
  if (theParameter == theLast)
  {
    return theLastValue;
  }
  const double aMiddle           = theFirst * 0.5 + theLast * 0.5;
  const bool   isNearFirst       = (theFirst < theLast) == (theParameter <= aMiddle);
  const double aNearParameter    = isNearFirst ? theFirst : theLast;
  const double aFarParameter     = isNearFirst ? theLast : theFirst;
  const double aNearValue        = isNearFirst ? theFirstValue : theLastValue;
  const double aFarValue         = isNearFirst ? theLastValue : theFirstValue;
  const double aNativeWidth      = aFarParameter - aNearParameter;
  const double aNativeDifference = aFarValue - aNearValue;
  const double aFraction         = (theParameter - aNearParameter) / aNativeWidth;
  if (std::isfinite(aNativeWidth) && std::isfinite(aNativeDifference) && std::isnormal(aFraction))
  {
    return std::fma(aFraction, aNativeDifference, aNearValue);
  }
  // Separate binary scales retain precision for subnormal fractions and avoid overflowing the
  // width.
  int          aWidthExponent, aDistanceExponent, aValueExponent;
  const double aWidth      = scaledDifference(aFarParameter, aNearParameter, aWidthExponent);
  const double aDistance   = scaledDifference(theParameter, aNearParameter, aDistanceExponent);
  const double aDifference = scaledDifference(aFarValue, aNearValue, aValueExponent);
  return aNearValue
         + std::scalbn((aDistance / aWidth) * aDifference,
                       aDistanceExponent + aValueExponent - aWidthExponent);
}

//==================================================================================================

double linearDerivative(const double theFirst,
                        const double theLast,
                        const double theFirstValue,
                        const double theLastValue)
{
  if (theFirstValue == theLastValue && theFirst != theLast)
  {
    return 0;
  }
  const double aNativeWidth      = theLast - theFirst;
  const double aNativeDifference = theLastValue - theFirstValue;
  if (std::isfinite(aNativeWidth) && std::isfinite(aNativeDifference))
  {
    return aNativeDifference / aNativeWidth;
  }
  int          aParameterExponent, aValueExponent;
  const double aNumerator   = scaledDifference(theLastValue, theFirstValue, aValueExponent);
  const double aDenominator = scaledDifference(theLast, theFirst, aParameterExponent);
  return std::scalbn(aNumerator / aDenominator, aValueExponent - aParameterExponent);
}
} // namespace

Law_Linear::Law_Linear()
    : valdeb(0.0),
      valfin(0.0),
      pdeb(0.0),
      pfin(0.0)
{
}

void Law_Linear::Set(const double Pdeb, const double Valdeb, const double Pfin, const double Valfin)
{
  pdeb   = Pdeb;
  pfin   = Pfin;
  valdeb = Valdeb;
  valfin = Valfin;
}

//=================================================================================================

GeomAbs_Shape Law_Linear::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

// int Law_Linear::NbIntervals(const GeomAbs_Shape S) const
int Law_Linear::NbIntervals(const GeomAbs_Shape) const
{
  return 1;
}

//=================================================================================================

void Law_Linear::Intervals(NCollection_Array1<double>& T,
                           //                           const GeomAbs_Shape S) const
                           const GeomAbs_Shape) const
{
  T.SetValue(T.Lower(), pdeb);
  T.SetValue(T.Upper(), pfin);
}

double Law_Linear::Value(const double X)
{
  return linearValue(X, pdeb, pfin, valdeb, valfin);
}

void Law_Linear::D1(const double X, double& F, double& D)
{
  F = linearValue(X, pdeb, pfin, valdeb, valfin);
  D = linearDerivative(pdeb, pfin, valdeb, valfin);
}

void Law_Linear::D2(const double X, double& F, double& D, double& D2)
{
  F  = linearValue(X, pdeb, pfin, valdeb, valfin);
  D  = linearDerivative(pdeb, pfin, valdeb, valfin);
  D2 = 0;
}

//=================================================================================================

occ::handle<Law_Function> Law_Linear::Trim(const double PFirst,
                                           const double PLast,
                                           //				      const double Tol) const
                                           const double) const
{
  occ::handle<Law_Linear> l = new (Law_Linear)();
  double                  Vdeb, Vfin;
  Vdeb = linearValue(PFirst, pdeb, pfin, valdeb, valfin);
  Vfin = linearValue(PLast, pdeb, pfin, valdeb, valfin);
  l->Set(PFirst, Vdeb, PLast, Vfin);

  return l;
}

void Law_Linear::Bounds(double& PFirst, double& PLast)
{
  PFirst = pdeb;
  PLast  = pfin;
}

// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <ShapePersistent_Geom_Curve.hxx>

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>


Handle(Geom_Curve) ShapePersistent_Geom_Curve::pBezier::Import() const
{
  if (myPoles.IsNull())
    return NULL;

  if (myRational)
  {
    if (myWeights.IsNull())
      return NULL;
    return new Geom_BezierCurve (*myPoles->Array(), *myWeights->Array());
  }
  else
    return new Geom_BezierCurve (*myPoles->Array());
}

Handle(Geom_Curve) ShapePersistent_Geom_Curve::pBSpline::Import() const
{
  if (myPoles.IsNull() || myKnots.IsNull() || myMultiplicities.IsNull())
    return NULL;

  if (myRational)
  {
    if (myWeights.IsNull())
      return NULL;

    return new Geom_BSplineCurve (*myPoles->Array(),
                                  *myWeights->Array(),
                                  *myKnots->Array(),
                                  *myMultiplicities->Array(),
                                  mySpineDegree,
                                  myPeriodic);
  }
  else
    return new Geom_BSplineCurve (*myPoles->Array(),
                                  *myKnots->Array(),
                                  *myMultiplicities->Array(),
                                  mySpineDegree,
                                  myPeriodic);
}

Handle(Geom_Curve) ShapePersistent_Geom_Curve::pTrimmed::Import() const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom_TrimmedCurve (myBasisCurve->Import(), myFirstU, myLastU);
}

Handle(Geom_Curve) ShapePersistent_Geom_Curve::pOffset::Import() const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom_OffsetCurve
    (myBasisCurve->Import(), myOffsetValue, myOffsetDirection);
}

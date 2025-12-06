// Created on: 1995-03-08
// Created by: Laurent BUCHARD
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

#include <CSLib_Class2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{
//! Transforms a coordinate from original space to normalized [0,1] space.
//! @param[in] theU       Original coordinate value
//! @param[in] theUMin    Minimum bound of original range
//! @param[in] theURange  Range of original domain (theUMax - theUMin)
//! @return Normalized coordinate in [0,1], or original value if range is too small
inline double transformToNormalized(const double theU, const double theUMin, const double theURange)
{
  constexpr double THE_MIN_RANGE = 1e-10;
  if (theURange > THE_MIN_RANGE)
  {
    return (theU - theUMin) / theURange;
  }
  return theU;
}
} // namespace

//=================================================================================================

template <class TCol_Containers2d>
void CSLib_Class2d::init(const TCol_Containers2d& thePnts2d,
                         const double             theTolU,
                         const double             theTolV,
                         const double             theUMin,
                         const double             theVMin,
                         const double             theUMax,
                         const double             theVMax)
{
  myUMin = theUMin;
  myVMin = theVMin;
  myUMax = theUMax;
  myVMax = theVMax;

  // Validate input parameters.
  if (theUMax <= theUMin || theVMax <= theVMin || thePnts2d.Length() < 3)
  {
    myPointsCount = 0;
    return;
  }

  myPointsCount = thePnts2d.Length();
  myTolU        = theTolU;
  myTolV        = theTolV;

  // Allocate arrays with one extra element for closing the polygon.
  myPnts2dX.Resize(0, myPointsCount, false);
  myPnts2dY.Resize(0, myPointsCount, false);

  const double aDu = theUMax - theUMin;
  const double aDv = theVMax - theVMin;

  // Transform points to normalized coordinates.
  const int aLower = thePnts2d.Lower();
  for (int i = 0; i < myPointsCount; ++i)
  {
    const gp_Pnt2d& aP2D     = thePnts2d(i + aLower);
    myPnts2dX.ChangeValue(i) = transformToNormalized(aP2D.X(), theUMin, aDu);
    myPnts2dY.ChangeValue(i) = transformToNormalized(aP2D.Y(), theVMin, aDv);
  }

  // Close the polygon by copying first point to last position.
  myPnts2dX.ChangeLast() = myPnts2dX.First();
  myPnts2dY.ChangeLast() = myPnts2dY.First();

  // Normalize tolerances.
  constexpr double THE_MIN_RANGE = 1e-10;
  if (aDu > THE_MIN_RANGE)
  {
    myTolU /= aDu;
  }
  if (aDv > THE_MIN_RANGE)
  {
    myTolV /= aDv;
  }
}

//=================================================================================================

CSLib_Class2d::CSLib_Class2d(const TColgp_Array1OfPnt2d& thePnts2d,
                             const double                theTolU,
                             const double                theTolV,
                             const double                theUMin,
                             const double                theVMin,
                             const double                theUMax,
                             const double                theVMax)
{
  init(thePnts2d, theTolU, theTolV, theUMin, theVMin, theUMax, theVMax);
}

//=================================================================================================

CSLib_Class2d::CSLib_Class2d(const TColgp_SequenceOfPnt2d& thePnts2d,
                             const double                  theTolU,
                             const double                  theTolV,
                             const double                  theUMin,
                             const double                  theVMin,
                             const double                  theUMax,
                             const double                  theVMax)
{
  init(thePnts2d, theTolU, theTolV, theUMin, theVMin, theUMax, theVMax);
}

//=================================================================================================

CSLib_Class2d::Result CSLib_Class2d::SiDans(const gp_Pnt2d& thePoint) const
{
  if (myPointsCount == 0)
  {
    return Result_Uncertain;
  }

  double aX = thePoint.X();
  double aY = thePoint.Y();

  // Compute tolerance in original coordinate space.
  const double aTolU = myTolU * (myUMax - myUMin);
  const double aTolV = myTolV * (myVMax - myVMin);

  // Quick rejection test for points clearly outside the bounding box.
  if (aX < (myUMin - aTolU) || aX > (myUMax + aTolU) || aY < (myVMin - aTolV)
      || aY > (myVMax + aTolV))
  {
    return Result_Outside;
  }

  // Transform to normalized coordinates.
  aX = transformToNormalized(aX, myUMin, myUMax - myUMin);
  aY = transformToNormalized(aY, myVMin, myVMax - myVMin);

  // Perform classification with ON detection.
  const Result aResult = internalSiDansOuOn(aX, aY);
  if (aResult == Result_Uncertain)
  {
    return Result_Uncertain; // ON boundary
  }

  // Check corner points with tolerance for boundary detection.
  if (myTolU > 0.0 || myTolV > 0.0)
  {
    const bool isInside = (aResult == Result_Inside);
    if (isInside != internalSiDans(aX - myTolU, aY - myTolV)
        || isInside != internalSiDans(aX + myTolU, aY - myTolV)
        || isInside != internalSiDans(aX - myTolU, aY + myTolV)
        || isInside != internalSiDans(aX + myTolU, aY + myTolV))
    {
      return Result_Uncertain; // Near boundary
    }
  }

  return aResult;
}

//=================================================================================================

CSLib_Class2d::Result CSLib_Class2d::SiDans_OnMode(const gp_Pnt2d& thePoint,
                                                   const double    theTol) const
{
  if (myPointsCount == 0)
  {
    return Result_Uncertain;
  }

  double aX = thePoint.X();
  double aY = thePoint.Y();

  // Quick rejection test.
  if (aX < (myUMin - theTol) || aX > (myUMax + theTol) || aY < (myVMin - theTol)
      || aY > (myVMax + theTol))
  {
    return Result_Outside;
  }

  // Transform to normalized coordinates.
  aX = transformToNormalized(aX, myUMin, myUMax - myUMin);
  aY = transformToNormalized(aY, myVMin, myVMax - myVMin);

  // Perform classification with ON detection.
  const Result aResult = internalSiDansOuOn(aX, aY);

  // Check corner points with tolerance.
  if (theTol > 0.0)
  {
    const bool isInside = (aResult == Result_Inside);
    if (isInside != internalSiDans(aX - theTol, aY - theTol)
        || isInside != internalSiDans(aX + theTol, aY - theTol)
        || isInside != internalSiDans(aX - theTol, aY + theTol)
        || isInside != internalSiDans(aX + theTol, aY + theTol))
    {
      return Result_Uncertain;
    }
  }

  return aResult;
}

//=================================================================================================

bool CSLib_Class2d::internalSiDans(const double thePx, const double thePy) const
{
  // Ray-casting algorithm: count edge crossings with a horizontal ray from (Px, Py) to +infinity.
  int aNbCrossings = 0;

  double aPrevDx          = myPnts2dX.Value(0) - thePx;
  double aPrevDy          = myPnts2dY.Value(0) - thePy;
  bool   aPrevYIsNegative = (aPrevDy < 0.0);

  for (int aNextIdx = 1; aNextIdx <= myPointsCount; ++aNextIdx)
  {
    const double aCurrDx          = myPnts2dX.Value(aNextIdx) - thePx;
    const double aCurrDy          = myPnts2dY.Value(aNextIdx) - thePy;
    const bool   aCurrYIsNegative = (aCurrDy < 0.0);

    // Check for edge crossing when Y changes sign.
    if (aCurrYIsNegative != aPrevYIsNegative)
    {
      if (aPrevDx > 0.0 && aCurrDx > 0.0)
      {
        // Both endpoints are to the right of the test point.
        ++aNbCrossings;
      }
      else if (aPrevDx > 0.0 || aCurrDx > 0.0)
      {
        // Compute X intersection with horizontal line Y = 0.
        const double aXIntersect = aPrevDx - aPrevDy * (aCurrDx - aPrevDx) / (aCurrDy - aPrevDy);
        if (aXIntersect > 0.0)
        {
          ++aNbCrossings;
        }
      }
      aPrevYIsNegative = aCurrYIsNegative;
    }

    aPrevDx = aCurrDx;
    aPrevDy = aCurrDy;
  }

  // Odd number of crossings means inside.
  return (aNbCrossings & 1) != 0;
}

//=================================================================================================

CSLib_Class2d::Result CSLib_Class2d::internalSiDansOuOn(const double thePx,
                                                        const double thePy) const
{
  // Ray-casting algorithm with ON detection.
  int aNbCrossings = 0;

  double aPrevDx          = myPnts2dX.Value(0) - thePx;
  double aPrevDy          = myPnts2dY.Value(0) - thePy;
  bool   aPrevYIsNegative = (aPrevDy < 0.0);

  for (int aNextIdx = 1; aNextIdx <= myPointsCount; ++aNextIdx)
  {
    const int    aPrevIdx = aNextIdx - 1;
    const double aCurrDx  = myPnts2dX.Value(aNextIdx) - thePx;
    const double aCurrDy  = myPnts2dY.Value(aNextIdx) - thePy;

    // Check if point is very close to current vertex.
    if (aCurrDx < myTolU && aCurrDx > -myTolU && aCurrDy < myTolV && aCurrDy > -myTolV)
    {
      return Result_Uncertain; // ON boundary (at vertex)
    }

    // Check if point is ON the edge by computing Y at the test point's X.
    // Skip interpolation for nearly vertical edges to avoid division instability.
    // For vertical edges, the ON detection is handled by the tolerance check above.
    const double aEdgeDx = myPnts2dX.Value(aNextIdx) - myPnts2dX.Value(aPrevIdx);
    if ((myPnts2dX.Value(aPrevIdx) - thePx) * aCurrDx < 0.0
        && std::abs(aEdgeDx) > Precision::PConfusion())
    {
      const double aInterpY =
        myPnts2dY.Value(aNextIdx)
        - (myPnts2dY.Value(aNextIdx) - myPnts2dY.Value(aPrevIdx)) / aEdgeDx * aCurrDx;
      const double aDeltaY = aInterpY - thePy;
      if (aDeltaY >= -myTolV && aDeltaY <= myTolV)
      {
        return Result_Uncertain; // ON boundary (on edge)
      }
    }

    const bool aCurrYIsNegative = (aCurrDy < 0.0);
    if (aCurrYIsNegative != aPrevYIsNegative)
    {
      if (aPrevDx > 0.0 && aCurrDx > 0.0)
      {
        ++aNbCrossings;
      }
      else if (aPrevDx > 0.0 || aCurrDx > 0.0)
      {
        const double aXIntersect = aPrevDx - aPrevDy * (aCurrDx - aPrevDx) / (aCurrDy - aPrevDy);
        if (aXIntersect > 0.0)
        {
          ++aNbCrossings;
        }
      }
      aPrevYIsNegative = aCurrYIsNegative;
    }

    aPrevDx = aCurrDx;
    aPrevDy = aCurrDy;
  }

  // Odd number of crossings means inside.
  return ((aNbCrossings & 1) != 0) ? Result_Inside : Result_Outside;
}

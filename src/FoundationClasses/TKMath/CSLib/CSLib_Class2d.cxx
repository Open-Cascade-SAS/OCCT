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
inline double transformToNormalized(const double theU,
                                    const double theUMin,
                                    const double theURange)
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
    myPnts2dX.Nullify();
    myPnts2dY.Nullify();
    myN = 0;
    return;
  }

  myN    = thePnts2d.Length();
  myTolU = theTolU;
  myTolV = theTolV;

  // Allocate arrays with one extra element for closing the polygon.
  myPnts2dX = new TColStd_Array1OfReal(0, myN);
  myPnts2dY = new TColStd_Array1OfReal(0, myN);

  const double aDu = theUMax - theUMin;
  const double aDv = theVMax - theVMin;

  // Transform points to normalized coordinates.
  const int aLower = thePnts2d.Lower();
  for (int i = 0; i < myN; ++i)
  {
    const gp_Pnt2d& aP2D      = thePnts2d(i + aLower);
    myPnts2dX->ChangeValue(i) = transformToNormalized(aP2D.X(), theUMin, aDu);
    myPnts2dY->ChangeValue(i) = transformToNormalized(aP2D.Y(), theVMin, aDv);
  }

  // Close the polygon by copying first point to last position.
  myPnts2dX->ChangeLast() = myPnts2dX->First();
  myPnts2dY->ChangeLast() = myPnts2dY->First();

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

int CSLib_Class2d::SiDans(const gp_Pnt2d& thePoint) const
{
  if (myN == 0)
  {
    return 0;
  }

  double aX = thePoint.X();
  double aY = thePoint.Y();

  // Compute tolerance in original coordinate space.
  const double aTolU = myTolU * (myUMax - myUMin);
  const double aTolV = myTolV * (myVMax - myVMin);

  // Quick rejection test for points clearly outside the bounding box.
  if (myUMin < myUMax && myVMin < myVMax)
  {
    if (aX < (myUMin - aTolU) || aX > (myUMax + aTolU) ||
        aY < (myVMin - aTolV) || aY > (myVMax + aTolV))
    {
      return -1;
    }

    // Transform to normalized coordinates.
    aX = transformToNormalized(aX, myUMin, myUMax - myUMin);
    aY = transformToNormalized(aY, myVMin, myVMax - myVMin);
  }

  // Perform classification with ON detection.
  const int aResult = InternalSiDansOuOn(aX, aY);
  if (aResult == -1)
  {
    return 0; // ON boundary
  }

  // Check corner points with tolerance for boundary detection.
  if (myTolU > 0.0 || myTolV > 0.0)
  {
    if (aResult != InternalSiDans(aX - myTolU, aY - myTolV) ||
        aResult != InternalSiDans(aX + myTolU, aY - myTolV) ||
        aResult != InternalSiDans(aX - myTolU, aY + myTolV) ||
        aResult != InternalSiDans(aX + myTolU, aY + myTolV))
    {
      return 0; // Uncertain (near boundary)
    }
  }

  return (aResult != 0) ? 1 : -1;
}

//=================================================================================================

int CSLib_Class2d::SiDans_OnMode(const gp_Pnt2d& thePoint, const double theTol) const
{
  if (myN == 0)
  {
    return 0;
  }

  double aX = thePoint.X();
  double aY = thePoint.Y();

  // Quick rejection test.
  if (myUMin < myUMax && myVMin < myVMax)
  {
    if (aX < (myUMin - theTol) || aX > (myUMax + theTol) ||
        aY < (myVMin - theTol) || aY > (myVMax + theTol))
    {
      return -1;
    }

    // Transform to normalized coordinates.
    aX = transformToNormalized(aX, myUMin, myUMax - myUMin);
    aY = transformToNormalized(aY, myVMin, myVMax - myVMin);
  }

  // Perform classification with ON detection.
  const int aResult = InternalSiDansOuOn(aX, aY);

  // Check corner points with tolerance.
  if (theTol > 0.0)
  {
    if (aResult != InternalSiDans(aX - theTol, aY - theTol) ||
        aResult != InternalSiDans(aX + theTol, aY - theTol) ||
        aResult != InternalSiDans(aX - theTol, aY + theTol) ||
        aResult != InternalSiDans(aX + theTol, aY + theTol))
    {
      return 0;
    }
  }

  return (aResult != 0) ? 1 : -1;
}

//=================================================================================================

int CSLib_Class2d::InternalSiDans(const double thePx, const double thePy) const
{
  // Ray-casting algorithm: count edge crossings with a horizontal ray from (Px, Py) to +infinity.
  int aNbCrossings = 0;

  double aX  = myPnts2dX->Value(0) - thePx;
  double aY  = myPnts2dY->Value(0) - thePy;
  int    aSH = (aY < 0.0) ? -1 : 1;

  for (int i = 0, ip1 = 1; i < myN; ++i, ++ip1)
  {
    const double aNx = myPnts2dX->Value(ip1) - thePx;
    const double aNy = myPnts2dY->Value(ip1) - thePy;
    const int    aNH = (aNy < 0.0) ? -1 : 1;

    // Check for edge crossing when Y changes sign.
    if (aNH != aSH)
    {
      if (aX > 0.0 && aNx > 0.0)
      {
        // Both endpoints are to the right of the test point.
        ++aNbCrossings;
      }
      else if (aX > 0.0 || aNx > 0.0)
      {
        // Compute X intersection with horizontal line Y = 0.
        const double aXIntersect = aX - aY * (aNx - aX) / (aNy - aY);
        if (aXIntersect > 0.0)
        {
          ++aNbCrossings;
        }
      }
      aSH = aNH;
    }

    aX = aNx;
    aY = aNy;
  }

  // Odd number of crossings means inside.
  return aNbCrossings & 1;
}

//=================================================================================================

int CSLib_Class2d::InternalSiDansOuOn(const double thePx, const double thePy) const
{
  // Ray-casting algorithm with ON detection.
  int    aNbCrossings = 0;
  double aYmin        = 0.0;

  double aX  = myPnts2dX->Value(0) - thePx;
  double aY  = myPnts2dY->Value(0) - thePy;
  aYmin      = aY;
  int aSH    = (aY < 0.0) ? -1 : 1;

  for (int i = 0, ip1 = 1; i < myN; ++i, ++ip1)
  {
    const double aNx = myPnts2dX->Value(ip1) - thePx;
    const double aNy = myPnts2dY->Value(ip1) - thePy;

    // Check if point is very close to current vertex.
    if (aNx < myTolU && aNx > -myTolU && aNy < myTolV && aNy > -myTolV)
    {
      return -1; // ON boundary (at vertex)
    }

    // Check if point is ON the edge by computing Y at the test point's X.
    // Skip interpolation for nearly vertical edges to avoid division instability.
    // For vertical edges, the ON detection is handled by the tolerance check above.
    const double aDx = myPnts2dX->Value(ip1) - myPnts2dX->Value(ip1 - 1);
    if ((myPnts2dX->Value(ip1 - 1) - thePx) * aNx < 0.0 && std::abs(aDx) > Precision::PConfusion())
    {
      const double aCurPY =
        myPnts2dY->Value(ip1) - (myPnts2dY->Value(ip1) - myPnts2dY->Value(ip1 - 1)) / aDx * aNx;
      const double aDeltaY = aCurPY - thePy;
      if (aDeltaY >= -myTolV && aDeltaY <= myTolV)
      {
        return -1; // ON boundary (on edge)
      }
    }

    const int aNH = (aNy < 0.0) ? -1 : 1;
    if (aNH != aSH)
    {
      if (aX > 0.0 && aNx > 0.0)
      {
        ++aNbCrossings;
      }
      else if (aX > 0.0 || aNx > 0.0)
      {
        const double aXIntersect = aX - aY * (aNx - aX) / (aNy - aY);
        if (aXIntersect > 0.0)
        {
          ++aNbCrossings;
        }
      }
      aSH = aNH;
    }
    else
    {
      // Track minimum Y for degenerate cases.
      if (aNy < aYmin)
      {
        aYmin = aNy;
      }
    }

    aX = aNx;
    aY = aNy;
  }

  return aNbCrossings & 1;
}

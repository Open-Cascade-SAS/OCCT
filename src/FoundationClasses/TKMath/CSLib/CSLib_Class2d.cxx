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
#include <NCollection_LinearVector.hxx>
#include <Precision.hxx>

#include <algorithm>
#include <cmath>
#include <limits>

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


  //! Grid cache for O(1) classification of points far from polygon edges.
  //! Construction is delayed until repeated queries amortize its cost.
  static constexpr int    THE_GRID_SIZE       = 32; //!< Grid resolution per axis
  static constexpr int    THE_GRID_MIN_POINTS = 24; //!< Small polygons stay on exact O(N) path
  static constexpr size_t THE_GRID_BUILD_QUERY_COUNT = 64; //!< Queries before lazy construction
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

CSLib_Class2d::CSLib_Class2d(const NCollection_Array1<gp_Pnt2d>& thePnts2d,
                             const double                        theTolU,
                             const double                        theTolV,
                             const double                        theUMin,
                             const double                        theVMin,
                             const double                        theUMax,
                             const double                        theVMax)
{
  init(thePnts2d, theTolU, theTolV, theUMin, theVMin, theUMax, theVMax);
}

//=================================================================================================

CSLib_Class2d::CSLib_Class2d(const NCollection_Sequence<gp_Pnt2d>& thePnts2d,
                             const double                          theTolU,
                             const double                          theTolV,
                             const double                          theUMin,
                             const double                          theVMin,
                             const double                          theUMax,
                             const double                          theVMax)
{
  init(thePnts2d, theTolU, theTolV, theUMin, theVMin, theUMax, theVMax);
}

//=================================================================================================

CSLib_Class2d::CSLib_Class2d(const NCollection_DynamicArray<gp_Pnt2d>& thePnts2d,
                             const double                              theTolU,
                             const double                              theTolV,
                             const double                              theUMin,
                             const double                              theVMin,
                             const double                              theUMax,
                             const double                              theVMax)
{
  init(thePnts2d, theTolU, theTolV, theUMin, theVMin, theUMax, theVMax);
}

//=================================================================================================

CSLib_Class2d::CSLib_Class2d(const CSLib_Class2d& theOther)
    : myPnts2dX(theOther.myPnts2dX),
      myPnts2dY(theOther.myPnts2dY),
      myTolU(theOther.myTolU),
      myTolV(theOther.myTolV),
      myPointsCount(theOther.myPointsCount),
      myUMin(theOther.myUMin),
      myVMin(theOther.myVMin),
      myUMax(theOther.myUMax),
      myVMax(theOther.myVMax),
      myQueryCount(theOther.myQueryCount.load(std::memory_order_relaxed))
{
  if (theOther.myGridState.load(std::memory_order_acquire) == 2)
  {
    myGrid = theOther.myGrid;
    myGridState.store(2, std::memory_order_relaxed);
  }
}

//=================================================================================================

CSLib_Class2d& CSLib_Class2d::operator=(const CSLib_Class2d& theOther)
{
  if (this == &theOther)
  {
    return *this;
  }
  CSLib_Class2d aCopy(theOther);
  *this = std::move(aCopy);
  return *this;
}

//=================================================================================================

void CSLib_Class2d::buildGridCache() const
{
  if (myPointsCount < THE_GRID_MIN_POINTS || myGridState.load(std::memory_order_acquire) == 2)
  {
    return;
  }

  unsigned char anExpectedState = 0;
  if (!myGridState.compare_exchange_strong(anExpectedState,
                                           static_cast<unsigned char>(1),
                                           std::memory_order_acq_rel,
                                           std::memory_order_acquire))
  {
    return;
  }

  try
  {
    const int aTotalCells = THE_GRID_SIZE * THE_GRID_SIZE;
    myGrid.Resize(0, aTotalCells - 1, false);

    const double                      aCellSize = 1.0 / THE_GRID_SIZE;
    const double                      anEps     = 8.0 * std::numeric_limits<double>::epsilon();
    const double*                     pX        = &myPnts2dX.First();
    const double*                     pY        = &myPnts2dY.First();
    NCollection_LinearVector<uint8_t> aBoundaryCells(static_cast<size_t>(aTotalCells), 0);

    // Rasterize tolerance-expanded edge boxes into the fixed grid. This is
    // equivalent to testing every cell box against every edge box, but avoids
    // an O(grid cells * polygon edges) scan.
    for (int anEdgeIdx = 0; anEdgeIdx < myPointsCount; ++anEdgeIdx)
    {
      const double aMinX = std::min(pX[anEdgeIdx], pX[anEdgeIdx + 1]) - myTolU - anEps;
      const double aMaxX = std::max(pX[anEdgeIdx], pX[anEdgeIdx + 1]) + myTolU + anEps;
      const double aMinY = std::min(pY[anEdgeIdx], pY[anEdgeIdx + 1]) - myTolV - anEps;
      const double aMaxY = std::max(pY[anEdgeIdx], pY[anEdgeIdx + 1]) + myTolV + anEps;
      if (aMaxX < 0.0 || aMinX > 1.0 || aMaxY < 0.0 || aMinY > 1.0)
      {
        continue;
      }

      const int aMinCellX =
        std::clamp(static_cast<int>(std::ceil(aMinX * THE_GRID_SIZE)) - 1, 0, THE_GRID_SIZE - 1);
      const int aMaxCellX =
        std::clamp(static_cast<int>(std::floor(aMaxX * THE_GRID_SIZE)), 0, THE_GRID_SIZE - 1);
      const int aMinCellY =
        std::clamp(static_cast<int>(std::ceil(aMinY * THE_GRID_SIZE)) - 1, 0, THE_GRID_SIZE - 1);
      const int aMaxCellY =
        std::clamp(static_cast<int>(std::floor(aMaxY * THE_GRID_SIZE)), 0, THE_GRID_SIZE - 1);

      for (int aCellY = aMinCellY; aCellY <= aMaxCellY; ++aCellY)
      {
        for (int aCellX = aMinCellX; aCellX <= aMaxCellX; ++aCellX)
        {
          const size_t aCellIndex    = static_cast<size_t>(aCellY * THE_GRID_SIZE + aCellX);
          aBoundaryCells[aCellIndex] = 1;
        }
      }
    }

    // A polygon cannot change classification inside a connected set of cells
    // that contains no boundary. Classify one center per component and flood
    // the result through the remaining cells.
    NCollection_LinearVector<uint8_t> aVisitedCells(static_cast<size_t>(aTotalCells), 0);
    NCollection_LinearVector<int>     aCellQueue(static_cast<size_t>(aTotalCells));
    for (int aCellIndex = 0; aCellIndex < aTotalCells; ++aCellIndex)
    {
      const size_t anIndex = static_cast<size_t>(aCellIndex);
      if (aBoundaryCells[anIndex] != 0)
      {
        myGrid.SetValue(aCellIndex, GridCell_Boundary);
        aVisitedCells[anIndex] = 1;
        continue;
      }
      if (aVisitedCells[anIndex] != 0)
      {
        continue;
      }

      const int      aSeedX       = aCellIndex % THE_GRID_SIZE;
      const int      aSeedY       = aCellIndex / THE_GRID_SIZE;
      const double   aSeedCenterX = (static_cast<double>(aSeedX) + 0.5) * aCellSize;
      const double   aSeedCenterY = (static_cast<double>(aSeedY) + 0.5) * aCellSize;
      const GridCell aComponentValue =
        internalSiDans(aSeedCenterX, aSeedCenterY) ? GridCell_Inside : GridCell_Outside;

      aCellQueue.Clear();
      aCellQueue.Append(aCellIndex);
      aVisitedCells[anIndex] = 1;
      for (size_t aQueueIndex = 0; aQueueIndex < aCellQueue.Size(); ++aQueueIndex)
      {
        const int aCurrentCell = aCellQueue[aQueueIndex];
        myGrid.SetValue(aCurrentCell, aComponentValue);

        const int aCurrentX         = aCurrentCell % THE_GRID_SIZE;
        const int aCurrentY         = aCurrentCell / THE_GRID_SIZE;
        const int aNeighborCells[4] = {aCurrentX > 0 ? aCurrentCell - 1 : -1,
                                       aCurrentX + 1 < THE_GRID_SIZE ? aCurrentCell + 1 : -1,
                                       aCurrentY > 0 ? aCurrentCell - THE_GRID_SIZE : -1,
                                       aCurrentY + 1 < THE_GRID_SIZE ? aCurrentCell + THE_GRID_SIZE
                                                                     : -1};
        for (size_t aNeighborIndex = 0; aNeighborIndex < 4; ++aNeighborIndex)
        {
          const int aNeighborCell = aNeighborCells[aNeighborIndex];
          if (aNeighborCell < 0)
          {
            continue;
          }
          const size_t aNeighbor = static_cast<size_t>(aNeighborCell);
          if (aVisitedCells[aNeighbor] == 0 && aBoundaryCells[aNeighbor] == 0)
          {
            aVisitedCells[aNeighbor] = 1;
            aCellQueue.Append(aNeighborCell);
          }
        }
      }
    }
    myGridState.store(2, std::memory_order_release);
  }
  catch (...)
  {
    myGridState.store(0, std::memory_order_release);
    throw;
  }
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

  // Build the acceleration grid only for sustained workloads. Short-lived
  // classifiers and small polygons remain on the cheaper exact scan.
  if (myGridState.load(std::memory_order_acquire) != 2 && myPointsCount >= THE_GRID_MIN_POINTS)
  {
    const size_t aQueryCount = myQueryCount.fetch_add(1, std::memory_order_relaxed) + 1;
    if (aQueryCount >= THE_GRID_BUILD_QUERY_COUNT)
    {
      buildGridCache();
    }
  }

  // Fast-path: conservative grid lookup (O(1) away from polygon edges).
  if (aX >= 0.0 && aX <= 1.0 && aY >= 0.0 && aY <= 1.0
      && myGridState.load(std::memory_order_acquire) == 2)
  {
    int aIX              = static_cast<int>(aX * THE_GRID_SIZE);
    int aIY              = static_cast<int>(aY * THE_GRID_SIZE);
    aIX                  = std::clamp(aIX, 0, THE_GRID_SIZE - 1);
    aIY                  = std::clamp(aIY, 0, THE_GRID_SIZE - 1);
    const GridCell aCell = myGrid.Value(aIY * THE_GRID_SIZE + aIX);
    if (aCell == GridCell_Inside)
      return Result_Inside;
    if (aCell == GridCell_Outside)
      return Result_Outside;
    // GridCell_Boundary — fall through to exact classification.
  }

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
  // Use raw pointers for cache-friendly sequential access and auto-vectorization.
  const double* pX = &myPnts2dX.First();
  const double* pY = &myPnts2dY.First();

  int aNbCrossings = 0;

  double aPrevDx          = pX[0] - thePx;
  double aPrevDy          = pY[0] - thePy;
  bool   aPrevYIsNegative = (aPrevDy < 0.0);

  for (int aNextIdx = 1; aNextIdx <= myPointsCount; ++aNextIdx)
  {
    const double aCurrDx          = pX[aNextIdx] - thePx;
    const double aCurrDy          = pY[aNextIdx] - thePy;
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
  // Use raw pointers for cache-friendly sequential access and auto-vectorization.
  const double* pX = &myPnts2dX.First();
  const double* pY = &myPnts2dY.First();

  int aNbCrossings = 0;

  double aPrevDx          = pX[0] - thePx;
  double aPrevDy          = pY[0] - thePy;
  bool   aPrevYIsNegative = (aPrevDy < 0.0);

  for (int aNextIdx = 1; aNextIdx <= myPointsCount; ++aNextIdx)
  {
    const int    aPrevIdx = aNextIdx - 1;
    const double aCurrDx  = pX[aNextIdx] - thePx;
    const double aCurrDy  = pY[aNextIdx] - thePy;

    // Check if point is very close to current vertex.
    if (aCurrDx < myTolU && aCurrDx > -myTolU && aCurrDy < myTolV && aCurrDy > -myTolV)
    {
      return Result_Uncertain; // ON boundary (at vertex)
    }

    // Check if point is ON the edge by computing Y at the test point's X.
    // Skip interpolation for nearly vertical edges to avoid division instability.
    // For vertical edges, the ON detection is handled by the tolerance check above.
    const double aEdgeDx = pX[aNextIdx] - pX[aPrevIdx];
    if ((pX[aPrevIdx] - thePx) * aCurrDx < 0.0 && std::abs(aEdgeDx) > Precision::PConfusion())
    {
      const double aInterpY = pY[aNextIdx] - (pY[aNextIdx] - pY[aPrevIdx]) / aEdgeDx * aCurrDx;
      const double aDeltaY  = aInterpY - thePy;
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

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
#include <Standard_OutOfMemory.hxx>

#include <algorithm>
#include <cmath>
#include <limits>
#include <new>

namespace
{
constexpr double THE_MIN_NORMALIZATION_RANGE = 1.0e-10;

//! Returns true when direct arithmetic cannot reach OCCT's geometric infinity range.
inline bool isSafeForDirectArithmetic(const double theValue)
{
  return !std::isnan(theValue) && !Precision::IsInfinite(theValue);
}

//! Returns true for representable values, including OCCT's finite infinity sentinels.
inline bool isRepresentableValue(const double theValue)
{
  constexpr double THE_MAX_VALUE = std::numeric_limits<double>::max();
  return theValue >= -THE_MAX_VALUE && theValue <= THE_MAX_VALUE;
}

//! Transforms a coordinate from original space to normalized [0,1] space.
//! @param[in] theU       Original coordinate value
//! @param[in] theUMin    Minimum bound of original range
//! @param[in] theUMax    Maximum bound of original range
//! @return Normalized coordinate in [0,1], or original value if range is too small
inline double transformToNormalized(const double theU, const double theUMin, const double theUMax)
{
  const double aRange = theUMax - theUMin;
  if (aRange > THE_MIN_NORMALIZATION_RANGE)
  {
    const double aDifference = theU - theUMin;
    if (isSafeForDirectArithmetic(aDifference) && isSafeForDirectArithmetic(aRange))
    {
      return aDifference / aRange;
    }
    const double aScale     = std::max(std::abs(theUMin), std::abs(theUMax));
    const double aScaledMin = theUMin / aScale;
    return (theU / aScale - aScaledMin) / (theUMax / aScale - aScaledMin);
  }
  return theU;
}

//! Converts an external tolerance to a finite, non-negative value.
inline double sanitizeTolerance(const double theTolerance)
{
  if (std::isnan(theTolerance) || theTolerance <= 0.0)
  {
    return 0.0;
  }
  return isRepresentableValue(theTolerance) ? theTolerance : std::numeric_limits<double>::max();
}

//! Normalizes a distance without overflowing when the finite bounds span more
//! than the representable double range.
inline double normalizeTolerance(const double theTolerance,
                                 const double theMin,
                                 const double theMax)
{
  const double aTolerance = sanitizeTolerance(theTolerance);
  const double aRange     = theMax - theMin;
  if (!(aRange > THE_MIN_NORMALIZATION_RANGE))
  {
    return aTolerance;
  }
  if (isSafeForDirectArithmetic(aRange))
  {
    return sanitizeTolerance(aTolerance / aRange);
  }

  const double aScale       = std::max(std::abs(theMin), std::abs(theMax));
  const double aScaledRange = theMax / aScale - theMin / aScale;
  return sanitizeTolerance((aTolerance / aScale) / aScaledRange);
}

//! Grid cache for O(1) classification of points far from polygon edges.
//! Construction is delayed until repeated queries amortize its cost.
static constexpr int    THE_GRID_SIZE              = 32;
static constexpr int    THE_GRID_MIN_POINTS        = 24;
static constexpr size_t THE_GRID_BUILD_QUERY_COUNT = 64;
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
  if (!isRepresentableValue(theUMin) || !isRepresentableValue(theVMin)
      || !isRepresentableValue(theUMax) || !isRepresentableValue(theVMax) || theUMax <= theUMin
      || theVMax <= theVMin || thePnts2d.Length() < 3)
  {
    myPointsCount = 0;
    return;
  }

  myPointsCount  = thePnts2d.Length();
  myOriginalTolU = sanitizeTolerance(theTolU);
  myOriginalTolV = sanitizeTolerance(theTolV);
  myTolU         = normalizeTolerance(theTolU, theUMin, theUMax);
  myTolV         = normalizeTolerance(theTolV, theVMin, theVMax);

  // Allocate arrays with one extra element for closing the polygon.
  myPnts2dX.Resize(0, myPointsCount, false);
  myPnts2dY.Resize(0, myPointsCount, false);

  // Transform points to normalized coordinates.
  const int aLower = thePnts2d.Lower();
  for (int i = 0; i < myPointsCount; ++i)
  {
    const gp_Pnt2d& aP2D     = thePnts2d(i + aLower);
    myPnts2dX.ChangeValue(i) = transformToNormalized(aP2D.X(), theUMin, theUMax);
    myPnts2dY.ChangeValue(i) = transformToNormalized(aP2D.Y(), theVMin, theVMax);
  }

  // Close the polygon by copying first point to last position.
  myPnts2dX.ChangeLast() = myPnts2dX.First();
  myPnts2dY.ChangeLast() = myPnts2dY.First();
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
      myOriginalTolU(theOther.myOriginalTolU),
      myOriginalTolV(theOther.myOriginalTolV),
      myPointsCount(theOther.myPointsCount),
      myUMin(theOther.myUMin),
      myVMin(theOther.myVMin),
      myUMax(theOther.myUMax),
      myVMax(theOther.myVMax),
      myQueryCount(theOther.myGridState.load(std::memory_order_acquire) == GridState::Building
                     ? 0
                     : theOther.myQueryCount.load(std::memory_order_relaxed))
{
  const GridState aState = theOther.myGridState.load(std::memory_order_acquire);
  if (aState == GridState::Ready)
  {
    myGrid = theOther.myGrid;
    myGridState.store(GridState::Ready, std::memory_order_relaxed);
  }
  else if (aState == GridState::Disabled)
  {
    myGridState.store(GridState::Disabled, std::memory_order_relaxed);
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

CSLib_Class2d::CSLib_Class2d(CSLib_Class2d&& theOther) noexcept
    : myPnts2dX(std::move(theOther.myPnts2dX)),
      myPnts2dY(std::move(theOther.myPnts2dY)),
      myTolU(theOther.myTolU),
      myTolV(theOther.myTolV),
      myOriginalTolU(theOther.myOriginalTolU),
      myOriginalTolV(theOther.myOriginalTolV),
      myPointsCount(theOther.myPointsCount),
      myUMin(theOther.myUMin),
      myVMin(theOther.myVMin),
      myUMax(theOther.myUMax),
      myVMax(theOther.myVMax),
      myQueryCount(theOther.myGridState.load(std::memory_order_acquire) == GridState::Building
                     ? 0
                     : theOther.myQueryCount.load(std::memory_order_relaxed))
{
  const GridState aState = theOther.myGridState.load(std::memory_order_acquire);
  if (aState == GridState::Ready)
  {
    myGrid = std::move(theOther.myGrid);
    myGridState.store(GridState::Ready, std::memory_order_relaxed);
  }
  else if (aState == GridState::Disabled)
  {
    myGridState.store(GridState::Disabled, std::memory_order_relaxed);
  }
  theOther.myPointsCount = 0;
  theOther.myGridState.store(GridState::Disabled, std::memory_order_release);
}

//=================================================================================================

CSLib_Class2d& CSLib_Class2d::operator=(CSLib_Class2d&& theOther) noexcept
{
  if (this == &theOther)
  {
    return *this;
  }

  const GridState aState = theOther.myGridState.load(std::memory_order_acquire);
  myPnts2dX              = std::move(theOther.myPnts2dX);
  myPnts2dY              = std::move(theOther.myPnts2dY);
  myTolU                 = theOther.myTolU;
  myTolV                 = theOther.myTolV;
  myOriginalTolU         = theOther.myOriginalTolU;
  myOriginalTolV         = theOther.myOriginalTolV;
  myPointsCount          = theOther.myPointsCount;
  myUMin                 = theOther.myUMin;
  myVMin                 = theOther.myVMin;
  myUMax                 = theOther.myUMax;
  myVMax                 = theOther.myVMax;
  myQueryCount.store(
    aState == GridState::Building ? 0 : theOther.myQueryCount.load(std::memory_order_relaxed),
    std::memory_order_relaxed);
  myGrid = NCollection_Array1<GridCell>();
  if (aState == GridState::Ready)
  {
    myGrid = std::move(theOther.myGrid);
    myGridState.store(GridState::Ready, std::memory_order_relaxed);
  }
  else
  {
    myGridState.store(aState == GridState::Disabled ? GridState::Disabled : GridState::NotBuilt,
                      std::memory_order_relaxed);
  }
  theOther.myPointsCount = 0;
  theOther.myGridState.store(GridState::Disabled, std::memory_order_release);
  return *this;
}

//=================================================================================================

void CSLib_Class2d::buildGridCache() const
{
  if (myPointsCount < THE_GRID_MIN_POINTS)
  {
    return;
  }

  GridState anExpectedState = GridState::NotBuilt;
  if (!myGridState.compare_exchange_strong(anExpectedState,
                                           GridState::Building,
                                           std::memory_order_acq_rel,
                                           std::memory_order_acquire))
  {
    return;
  }

  try
  {
    const int aTotalCells = THE_GRID_SIZE * THE_GRID_SIZE;
    myGrid.Resize(0, aTotalCells - 1, false);
    myGrid.Init(GridCell_Unvisited);

    const double                  aCellSize = 1.0 / THE_GRID_SIZE;
    constexpr double              anEps     = 8.0 * std::numeric_limits<double>::epsilon();
    const double*                 pX        = &myPnts2dX.First();
    const double*                 pY        = &myPnts2dY.First();
    NCollection_LinearVector<int> aCellQueue(static_cast<size_t>(aTotalCells));

    // Non-finite polygon data and domain-sized tolerances are valid for the
    // exact path but cannot produce a useful, safely indexed cache.
    if (myTolU >= 1.0 || myTolV >= 1.0)
    {
      myGrid = NCollection_Array1<GridCell>();
      myGridState.store(GridState::Disabled, std::memory_order_release);
      return;
    }
    for (int aPointIdx = 0; aPointIdx < myPointsCount; ++aPointIdx)
    {
      if (!isSafeForDirectArithmetic(pX[aPointIdx]) || !isSafeForDirectArithmetic(pY[aPointIdx]))
      {
        myGrid = NCollection_Array1<GridCell>();
        myGridState.store(GridState::Disabled, std::memory_order_release);
        return;
      }
    }

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

      const double aClippedMinX = std::clamp(aMinX, 0.0, 1.0);
      const double aClippedMaxX = std::clamp(aMaxX, 0.0, 1.0);
      const double aClippedMinY = std::clamp(aMinY, 0.0, 1.0);
      const double aClippedMaxY = std::clamp(aMaxY, 0.0, 1.0);
      const int    aMinCellX =
        std::max(static_cast<int>(std::ceil(aClippedMinX * THE_GRID_SIZE)) - 1, 0);
      const int aMaxCellX =
        std::min(static_cast<int>(std::floor(aClippedMaxX * THE_GRID_SIZE)), THE_GRID_SIZE - 1);
      const int aMinCellY =
        std::max(static_cast<int>(std::ceil(aClippedMinY * THE_GRID_SIZE)) - 1, 0);
      const int aMaxCellY =
        std::min(static_cast<int>(std::floor(aClippedMaxY * THE_GRID_SIZE)), THE_GRID_SIZE - 1);

      for (int aCellY = aMinCellY; aCellY <= aMaxCellY; ++aCellY)
      {
        for (int aCellX = aMinCellX; aCellX <= aMaxCellX; ++aCellX)
        {
          myGrid.SetValue(aCellY * THE_GRID_SIZE + aCellX, GridCell_Boundary);
        }
      }
    }

    // A polygon cannot change classification inside a connected set of cells
    // that contains no boundary. Classify one center per component and flood
    // the result through the remaining cells.
    int aUsableCellCount = 0;
    for (int aCellIndex = 0; aCellIndex < aTotalCells; ++aCellIndex)
    {
      if (myGrid.Value(aCellIndex) != GridCell_Unvisited)
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
      myGrid.SetValue(aCellIndex, aComponentValue);
      for (size_t aQueueIndex = 0; aQueueIndex < aCellQueue.Size(); ++aQueueIndex)
      {
        const int aCurrentCell = aCellQueue[aQueueIndex];
        ++aUsableCellCount;

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
          if (myGrid.Value(aNeighborCell) == GridCell_Unvisited)
          {
            myGrid.SetValue(aNeighborCell, aComponentValue);
            aCellQueue.Append(aNeighborCell);
          }
        }
      }
    }

    // Even a partial cache is useful after the sustained-use threshold: a
    // classified cell avoids an O(edges) scan, while boundary cells remain exact.
    if (aUsableCellCount == 0)
    {
      myGrid = NCollection_Array1<GridCell>();
      myGridState.store(GridState::Disabled, std::memory_order_release);
      return;
    }
    myGridState.store(GridState::Ready, std::memory_order_release);
  }
  catch (const Standard_OutOfMemory&)
  {
    myGrid = NCollection_Array1<GridCell>();
    myQueryCount.store(0, std::memory_order_relaxed);
    myGridState.store(GridState::NotBuilt, std::memory_order_release);
  }
  catch (const std::bad_alloc&)
  {
    myGrid = NCollection_Array1<GridCell>();
    myQueryCount.store(0, std::memory_order_relaxed);
    myGridState.store(GridState::NotBuilt, std::memory_order_release);
  }
  catch (...)
  {
    myGrid = NCollection_Array1<GridCell>();
    myQueryCount.store(0, std::memory_order_relaxed);
    myGridState.store(GridState::NotBuilt, std::memory_order_release);
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
  const double aTolU = myOriginalTolU;
  const double aTolV = myOriginalTolV;

  // Quick rejection test for points clearly outside the bounding box.
  if (aX < (myUMin - aTolU) || aX > (myUMax + aTolU) || aY < (myVMin - aTolV)
      || aY > (myVMax + aTolV))
  {
    return Result_Outside;
  }

  // Transform to normalized coordinates.
  aX = transformToNormalized(aX, myUMin, myUMax);
  aY = transformToNormalized(aY, myVMin, myVMax);

  // Build the acceleration grid only for sustained workloads. Short-lived
  // classifiers and small polygons remain on the cheaper exact scan.
  GridState aGridState = myGridState.load(std::memory_order_acquire);
  if (aGridState == GridState::NotBuilt && myPointsCount >= THE_GRID_MIN_POINTS)
  {
    const size_t aQueryCount = myQueryCount.fetch_add(1, std::memory_order_relaxed) + 1;
    if (aQueryCount >= THE_GRID_BUILD_QUERY_COUNT)
    {
      buildGridCache();
      aGridState = myGridState.load(std::memory_order_acquire);
    }
  }

  // Fast-path: conservative grid lookup (O(1) away from polygon edges).
  if (aGridState == GridState::Ready && isSafeForDirectArithmetic(aX)
      && isSafeForDirectArithmetic(aY) && aX >= 0.0 && aX <= 1.0 && aY >= 0.0 && aY <= 1.0)
  {
    int aIX              = static_cast<int>(aX * THE_GRID_SIZE);
    int aIY              = static_cast<int>(aY * THE_GRID_SIZE);
    aIX                  = std::clamp(aIX, 0, THE_GRID_SIZE - 1);
    aIY                  = std::clamp(aIY, 0, THE_GRID_SIZE - 1);
    const GridCell aCell = myGrid.Value(aIY * THE_GRID_SIZE + aIX);
    if (aCell == GridCell_Inside)
    {
      return Result_Inside;
    }
    if (aCell == GridCell_Outside)
    {
      return Result_Outside;
    }
    // Boundary cells fall through to exact classification.
  }

  // Perform classification with ON detection.
  const Result aResult = internalSiDansOuOn(aX, aY, myTolU, myTolV);
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

  double       aX         = thePoint.X();
  double       aY         = thePoint.Y();
  const double aTolerance = sanitizeTolerance(theTol);
  const double aTolU      = normalizeTolerance(aTolerance, myUMin, myUMax);
  const double aTolV      = normalizeTolerance(aTolerance, myVMin, myVMax);

  // Quick rejection test.
  if (aX < (myUMin - aTolerance) || aX > (myUMax + aTolerance) || aY < (myVMin - aTolerance)
      || aY > (myVMax + aTolerance))
  {
    return Result_Outside;
  }

  // Transform to normalized coordinates.
  aX = transformToNormalized(aX, myUMin, myUMax);
  aY = transformToNormalized(aY, myVMin, myVMax);

  // Perform classification with ON detection.
  const Result aResult = internalSiDansOuOn(aX, aY, aTolU, aTolV);

  // Check corner points with tolerance.
  if (aTolU > 0.0 || aTolV > 0.0)
  {
    const bool isInside = (aResult == Result_Inside);
    if (isInside != internalSiDans(aX - aTolU, aY - aTolV)
        || isInside != internalSiDans(aX + aTolU, aY - aTolV)
        || isInside != internalSiDans(aX - aTolU, aY + aTolV)
        || isInside != internalSiDans(aX + aTolU, aY + aTolV))
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
                                                        const double thePy,
                                                        const double theTolU,
                                                        const double theTolV) const
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
    if (aCurrDx < theTolU && aCurrDx > -theTolU && aCurrDy < theTolV && aCurrDy > -theTolV)
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
      if (aDeltaY >= -theTolV && aDeltaY <= theTolV)
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

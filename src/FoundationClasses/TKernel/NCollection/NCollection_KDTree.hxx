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

#ifndef NCollection_KDTree_HeaderFile
#define NCollection_KDTree_HeaderFile

#include <NCollection_Array1.hxx>
#include <NCollection_DynamicArray.hxx>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <type_traits>

//! @brief Static KD-Tree for efficient point set queries.
//!
//! NCollection_KDTree is a balanced KD-Tree built once from a set of points,
//! supporting efficient nearest-neighbor, k-nearest, range (sphere), and
//! box (AABB) queries.
//!
//! Key features:
//! - O(N log N) construction via median-split (std::nth_element)
//! - O(log N) nearest-neighbor search with bounding box pruning
//! - O(N^(1-1/d) + k) range and box search
//! - Cache-friendly permutation-based layout (no node allocations)
//! - Optional per-point radii for sphere-aware queries (compile-time, zero overhead when unused)
//! - Works with any point type providing Coord(int) with 1-based indexing
//!
//! Best suited for:
//! - Static point clouds (build once, query many times)
//! - Nearest-neighbor and k-nearest queries
//! - Spatial range filtering
//! - Sphere containment and weighted nearest queries (when HasRadii = true)
//!
//! Required interface for ThePointType (besides copy constructor and operator =):
//! @code
//!   class MyPointType
//!   {
//!   public:
//!     double Coord(int theIndex) const;
//!     // Returns coordinate along theIndex axis (1-based, from 1 to TheDimension)
//!   };
//! @endcode
//!
//! Works out-of-the-box with gp_Pnt, gp_Pnt2d, gp_XYZ, gp_XY.
//!
//! @tparam ThePointType  Point type providing Coord(int) with 1-based indexing
//! @tparam TheDimension  Spatial dimension (compile-time constant, typically 2 or 3)
//! @tparam HasRadii      When true, enables per-point radii for ContainingSearch
//!                       and NearestWeighted queries (default: false)
template <class ThePointType, int TheDimension, bool HasRadii = false>
class NCollection_KDTree
{
public:
  //! Empty constructor. Creates an empty tree.
  NCollection_KDTree()
      : mySize(0)
  {
  }

  //! Build the tree from a C array of points (no radii).
  //! Only available when HasRadii is false.
  //! @param[in] thePoints pointer to contiguous array of points
  //! @param[in] theCount  number of points
  template <bool R = HasRadii, typename = std::enable_if_t<!R>>
  void Build(const ThePointType* thePoints, size_t theCount)
  {
    if (theCount == 0)
    {
      Clear();
      return;
    }
    mySize = theCount;
    myPoints.Resize(1, static_cast<int>(theCount), false);
    myIndices.Resize(1, static_cast<int>(theCount), false);
    for (size_t i = 0; i < theCount; ++i)
    {
      myPoints.SetValue(static_cast<int>(i + 1), thePoints[i]);
      myIndices.SetValue(static_cast<int>(i + 1), i + 1);
    }
    buildRecursive(1, static_cast<int>(mySize), 0);
  }

  //! Build the tree from an NCollection_Array1 (no radii).
  //! Only available when HasRadii is false.
  //! @param[in] thePoints array of points (any lower bound)
  template <bool R = HasRadii, typename = std::enable_if_t<!R>>
  void Build(const NCollection_Array1<ThePointType>& thePoints)
  {
    const int aCount = thePoints.Length();
    if (aCount == 0)
    {
      Clear();
      return;
    }
    mySize = static_cast<size_t>(aCount);
    myPoints.Resize(1, aCount, false);
    myIndices.Resize(1, aCount, false);
    for (int i = 0; i < aCount; ++i)
    {
      myPoints.SetValue(i + 1, thePoints.Value(thePoints.Lower() + i));
      myIndices.SetValue(i + 1, static_cast<size_t>(i + 1));
    }
    buildRecursive(1, static_cast<int>(mySize), 0);
  }

  //! Build the tree from a C array of points with per-point radii.
  //! Only available when HasRadii is true.
  //! @param[in] thePoints pointer to contiguous array of points
  //! @param[in] theRadii  pointer to contiguous array of radii (one per point)
  //! @param[in] theCount  number of points
  template <bool R = HasRadii, typename = std::enable_if_t<R>>
  void Build(const ThePointType* thePoints, const double* theRadii, size_t theCount)
  {
    if (theCount == 0)
    {
      Clear();
      return;
    }
    mySize       = theCount;
    const int aN = static_cast<int>(theCount);
    myPoints.Resize(1, aN, false);
    myIndices.Resize(1, aN, false);
    myRadii.Resize(1, aN, false);
    myMaxRadius.Resize(1, aN, false);
    for (size_t i = 0; i < theCount; ++i)
    {
      const int anI = static_cast<int>(i + 1);
      myPoints.SetValue(anI, thePoints[i]);
      myIndices.SetValue(anI, i + 1);
      myRadii.SetValue(anI, theRadii[i]);
    }
    buildRecursive(1, aN, 0);
  }

  //! Build the tree from NCollection_Array1 of points with radii.
  //! Only available when HasRadii is true.
  //! @param[in] thePoints array of points (any lower bound)
  //! @param[in] theRadii  array of radii (same length as thePoints)
  template <bool R = HasRadii, typename = std::enable_if_t<R>>
  void Build(const NCollection_Array1<ThePointType>& thePoints,
             const NCollection_Array1<double>&       theRadii)
  {
    const int aCount = thePoints.Length();
    if (aCount == 0)
    {
      Clear();
      return;
    }
    mySize = static_cast<size_t>(aCount);
    myPoints.Resize(1, aCount, false);
    myIndices.Resize(1, aCount, false);
    myRadii.Resize(1, aCount, false);
    myMaxRadius.Resize(1, aCount, false);
    for (int i = 0; i < aCount; ++i)
    {
      myPoints.SetValue(i + 1, thePoints.Value(thePoints.Lower() + i));
      myIndices.SetValue(i + 1, static_cast<size_t>(i + 1));
      myRadii.SetValue(i + 1, theRadii.Value(theRadii.Lower() + i));
    }
    buildRecursive(1, static_cast<int>(mySize), 0);
  }

  //! Returns true if the tree contains no points.
  bool IsEmpty() const { return mySize == 0; }

  //! Returns the number of points in the tree.
  size_t Size() const { return mySize; }

  //! Clears the tree.
  void Clear()
  {
    mySize = 0;
    myPoints.Resize(1, 0, false);
    myIndices.Resize(1, 0, false);
    if constexpr (HasRadii)
    {
      myRadii.Resize(1, 0, false);
      myMaxRadius.Resize(1, 0, false);
    }
  }

  //! Returns the point at the given 1-based original index.
  //! @param[in] theIndex 1-based point index
  //! @return const reference to the point
  const ThePointType& Point(size_t theIndex) const
  {
    return myPoints.Value(static_cast<int>(theIndex));
  }

  //! Returns the radius of the point at the given 1-based original index.
  //! Only available when HasRadii is true.
  //! @param[in] theIndex 1-based point index
  //! @return radius of the point
  template <bool R = HasRadii, typename = std::enable_if_t<R>>
  double Radius(size_t theIndex) const
  {
    return myRadii.Value(static_cast<int>(theIndex));
  }

  //! Finds the nearest point to theQuery.
  //! @param[in] theQuery query point
  //! @return 1-based index of nearest point, 0 if tree is empty
  size_t NearestPoint(const ThePointType& theQuery) const
  {
    double aDummy = 0.0;
    return NearestPoint(theQuery, aDummy);
  }

  //! Finds the nearest point to theQuery and returns the squared distance.
  //! @param[in]  theQuery      query point
  //! @param[out] theSqDistance  squared distance to the nearest point
  //! @return 1-based index of nearest point, 0 if tree is empty
  size_t NearestPoint(const ThePointType& theQuery, double& theSqDistance) const
  {
    if (IsEmpty())
    {
      theSqDistance = 0.0;
      return 0;
    }
    size_t aBestIndex  = 0;
    double aBestSqDist = std::numeric_limits<double>::max();
    double aBoundsMin[TheDimension];
    double aBoundsMax[TheDimension];
    initBounds(aBoundsMin, aBoundsMax);
    nearestRecursive(theQuery,
                     1,
                     static_cast<int>(mySize),
                     0,
                     aBestIndex,
                     aBestSqDist,
                     aBoundsMin,
                     aBoundsMax);
    theSqDistance = aBestSqDist;
    return aBestIndex;
  }

  //! Finds all nearest points to theQuery that share the same minimum distance.
  //! Useful when multiple points are equidistant from the query.
  //! First finds the nearest distance via tree traversal, then collects all points
  //! at that distance using a range search with theTolerance for floating-point comparison.
  //! @param[in]  theQuery      query point
  //! @param[in]  theTolerance  absolute tolerance for distance equality (default 0.0)
  //! @param[out] theSqDistance squared distance to the nearest points
  //! @return array of 1-based indices of all equidistant nearest points (empty if tree is empty)
  NCollection_DynamicArray<size_t> NearestPoints(const ThePointType& theQuery,
                                                 double              theTolerance,
                                                 double&             theSqDistance) const
  {
    NCollection_DynamicArray<size_t> aResult;
    if (IsEmpty())
    {
      theSqDistance = 0.0;
      return aResult;
    }
    // Find the nearest distance first
    size_t aBestIndex  = 0;
    double aBestSqDist = std::numeric_limits<double>::max();
    double aBoundsMin[TheDimension];
    double aBoundsMax[TheDimension];
    initBounds(aBoundsMin, aBoundsMax);
    nearestRecursive(theQuery,
                     1,
                     static_cast<int>(mySize),
                     0,
                     aBestIndex,
                     aBestSqDist,
                     aBoundsMin,
                     aBoundsMax);
    theSqDistance = aBestSqDist;
    // Collect all points at this distance (within tolerance)
    const double aBestDist       = std::sqrt(aBestSqDist);
    const double aSearchRadius   = aBestDist + theTolerance;
    const double aSearchRadiusSq = aSearchRadius * aSearchRadius;
    const double aMinSqDist      = (aBestDist - theTolerance > 0.0)
                                     ? (aBestDist - theTolerance) * (aBestDist - theTolerance)
                                     : 0.0;
    // Use range search to get candidates, then filter by exact distance tolerance
    NCollection_DynamicArray<size_t> aCandidates;
    rangeSearchRecursive(theQuery, aSearchRadiusSq, 1, static_cast<int>(mySize), 0, aCandidates);
    for (int i = 0; i < aCandidates.Size(); ++i)
    {
      const size_t anIdx   = aCandidates[i];
      const double aSqDist = squareDistance(theQuery, myPoints.Value(static_cast<int>(anIdx)));
      if (aSqDist >= aMinSqDist && aSqDist <= aSearchRadiusSq)
      {
        aResult.Append(anIdx);
      }
    }
    return aResult;
  }

  //! Finds the K nearest points to theQuery.
  //! Results are sorted by distance (closest first).
  //! @param[in]  theQuery       query point
  //! @param[in]  theK           number of nearest points to find
  //! @param[out] theIndices     1-based indices of the found points (resized internally)
  //! @param[out] theSqDistances squared distances of the found points (resized internally)
  //! @return actual count of points found (may be less than theK)
  size_t KNearestPoints(const ThePointType&         theQuery,
                        size_t                      theK,
                        NCollection_Array1<size_t>& theIndices,
                        NCollection_Array1<double>& theSqDistances) const
  {
    if (IsEmpty() || theK == 0)
    {
      theIndices.Resize(1, 0, false);
      theSqDistances.Resize(1, 0, false);
      return 0;
    }
    const size_t anActualK = std::min(theK, mySize);
    // Max-heap: pairs of (sqDist, index)
    NCollection_Array1<std::pair<double, size_t>> aHeap(1, static_cast<int>(anActualK));
    size_t                                        aHeapSize = 0;
    double                                        aBoundsMin[TheDimension];
    double                                        aBoundsMax[TheDimension];
    initBounds(aBoundsMin, aBoundsMax);
    kNearestRecursive(theQuery,
                      1,
                      static_cast<int>(mySize),
                      0,
                      aHeap,
                      aHeapSize,
                      anActualK,
                      aBoundsMin,
                      aBoundsMax);
    // Extract from heap sorted closest-first
    const int aCount = static_cast<int>(aHeapSize);
    theIndices.Resize(1, aCount, false);
    theSqDistances.Resize(1, aCount, false);
    // Heap is max-heap, so extract in reverse (largest first) then reverse
    for (int i = aCount; i >= 1; --i)
    {
      theIndices.SetValue(i, aHeap.Value(1).second);
      theSqDistances.SetValue(i, aHeap.Value(1).first);
      // Pop max from heap
      aHeap.SetValue(1, aHeap.Value(static_cast<int>(aHeapSize)));
      --aHeapSize;
      heapSiftDown(aHeap, 1, static_cast<int>(aHeapSize));
    }
    return static_cast<size_t>(aCount);
  }

  //! Finds all points within theRadius of theQuery (sphere search).
  //! @param[in] theQuery  query point
  //! @param[in] theRadius search radius
  //! @return array of 1-based indices of found points
  NCollection_DynamicArray<size_t> RangeSearch(const ThePointType& theQuery, double theRadius) const
  {
    NCollection_DynamicArray<size_t> aResult;
    if (IsEmpty() || theRadius < 0.0)
    {
      return aResult;
    }
    const double aRadiusSq = theRadius * theRadius;
    rangeSearchRecursive(theQuery, aRadiusSq, 1, static_cast<int>(mySize), 0, aResult);
    return aResult;
  }

  //! Finds all points within the axis-aligned bounding box [theMin, theMax].
  //! @param[in] theMin minimum corner of the box
  //! @param[in] theMax maximum corner of the box
  //! @return array of 1-based indices of found points
  NCollection_DynamicArray<size_t> BoxSearch(const ThePointType& theMin,
                                             const ThePointType& theMax) const
  {
    NCollection_DynamicArray<size_t> aResult;
    if (IsEmpty())
    {
      return aResult;
    }
    boxSearchRecursive(theMin, theMax, 1, static_cast<int>(mySize), 0, aResult);
    return aResult;
  }

  //! Finds all points whose sphere contains theQuery.
  //! Point i "contains" theQuery if dist(theQuery, point_i) <= radius_i.
  //! Only available when HasRadii is true.
  //! @param[in] theQuery query point
  //! @return array of 1-based indices of containing points
  template <bool R = HasRadii, typename = std::enable_if_t<R>>
  NCollection_DynamicArray<size_t> ContainingSearch(const ThePointType& theQuery) const
  {
    NCollection_DynamicArray<size_t> aResult;
    if (IsEmpty())
    {
      return aResult;
    }
    double aBoundsMin[TheDimension];
    double aBoundsMax[TheDimension];
    initBounds(aBoundsMin, aBoundsMax);
    containingSearchRecursive(theQuery,
                              1,
                              static_cast<int>(mySize),
                              0,
                              aResult,
                              aBoundsMin,
                              aBoundsMax);
    return aResult;
  }

  //! Finds the point whose sphere surface is closest to theQuery.
  //! Minimizes gap distance = dist(theQuery, point_i) - radius_i.
  //! Negative gap means theQuery is inside the sphere.
  //! Only available when HasRadii is true.
  //! @param[in] theQuery query point
  //! @return 1-based index of nearest-weighted point, 0 if tree is empty
  template <bool R = HasRadii, typename = std::enable_if_t<R>>
  size_t NearestWeighted(const ThePointType& theQuery) const
  {
    double aDummy = 0.0;
    return NearestWeighted(theQuery, aDummy);
  }

  //! Finds the point whose sphere surface is closest to theQuery.
  //! Minimizes gap distance = dist(theQuery, point_i) - radius_i.
  //! Negative gap means theQuery is inside the sphere.
  //! Only available when HasRadii is true.
  //! @param[in]  theQuery       query point
  //! @param[out] theGapDistance  gap distance to the nearest sphere surface (negative = inside)
  //! @return 1-based index of nearest-weighted point, 0 if tree is empty
  template <bool R = HasRadii, typename = std::enable_if_t<R>>
  size_t NearestWeighted(const ThePointType& theQuery, double& theGapDistance) const
  {
    if (IsEmpty())
    {
      theGapDistance = 0.0;
      return 0;
    }
    size_t aBestIndex = 0;
    double aBestGap   = std::numeric_limits<double>::max();
    double aBoundsMin[TheDimension];
    double aBoundsMax[TheDimension];
    initBounds(aBoundsMin, aBoundsMax);
    nearestWeightedRecursive(theQuery,
                             1,
                             static_cast<int>(mySize),
                             0,
                             aBestIndex,
                             aBestGap,
                             aBoundsMin,
                             aBoundsMax);
    theGapDistance = aBestGap;
    return aBestIndex;
  }

  //! Copy constructor.
  NCollection_KDTree(const NCollection_KDTree& theOther) = default;

  //! Move constructor.
  NCollection_KDTree(NCollection_KDTree&& theOther) noexcept
      : myPoints(std::move(theOther.myPoints)),
        myIndices(std::move(theOther.myIndices)),
        myRadii(std::move(theOther.myRadii)),
        myMaxRadius(std::move(theOther.myMaxRadius)),
        mySize(theOther.mySize)
  {
    theOther.mySize = 0;
  }

  //! Copy assignment.
  //! Uses copy-and-swap because NCollection_Array1::operator= requires same size.
  NCollection_KDTree& operator=(const NCollection_KDTree& theOther)
  {
    if (this != &theOther)
    {
      NCollection_KDTree aCopy(theOther);
      *this = std::move(aCopy);
    }
    return *this;
  }

  //! Move assignment.
  NCollection_KDTree& operator=(NCollection_KDTree&& theOther) noexcept
  {
    if (this != &theOther)
    {
      myPoints        = std::move(theOther.myPoints);
      myIndices       = std::move(theOther.myIndices);
      myRadii         = std::move(theOther.myRadii);
      myMaxRadius     = std::move(theOther.myMaxRadius);
      mySize          = theOther.mySize;
      theOther.mySize = 0;
    }
    return *this;
  }

private:
  //! Squared distance between two points.
  static double squareDistance(const ThePointType& theP1, const ThePointType& theP2)
  {
    double aSqDist = 0.0;
    for (int i = 1; i <= TheDimension; ++i)
    {
      const double aDiff = theP1.Coord(i) - theP2.Coord(i);
      aSqDist += aDiff * aDiff;
    }
    return aSqDist;
  }

  //! Checks whether a point is inside a box [theMin, theMax].
  static bool isInsideBox(const ThePointType& thePoint,
                          const ThePointType& theMin,
                          const ThePointType& theMax)
  {
    for (int i = 1; i <= TheDimension; ++i)
    {
      const double aCoord = thePoint.Coord(i);
      if (aCoord < theMin.Coord(i) || aCoord > theMax.Coord(i))
      {
        return false;
      }
    }
    return true;
  }

  //! Leaf bucket size: subtrees with this many points or fewer
  //! are scanned linearly instead of recursing further.
  static constexpr int THE_LEAF_SIZE = 32;

  //! Minimum squared distance from a point to an axis-aligned bounding box.
  static double sqDistToBox(const ThePointType& theQuery,
                            const double        theBoundsMin[],
                            const double        theBoundsMax[])
  {
    double aSqDist = 0.0;
    for (int i = 0; i < TheDimension; ++i)
    {
      const double aCoord = theQuery.Coord(i + 1);
      if (aCoord < theBoundsMin[i])
      {
        const double aDiff = theBoundsMin[i] - aCoord;
        aSqDist += aDiff * aDiff;
      }
      else if (aCoord > theBoundsMax[i])
      {
        const double aDiff = aCoord - theBoundsMax[i];
        aSqDist += aDiff * aDiff;
      }
    }
    return aSqDist;
  }

  //! Initializes bounding box arrays to [-max, +max] in all dimensions.
  static void initBounds(double theBoundsMin[], double theBoundsMax[])
  {
    for (int i = 0; i < TheDimension; ++i)
    {
      theBoundsMin[i] = -std::numeric_limits<double>::max();
      theBoundsMax[i] = std::numeric_limits<double>::max();
    }
  }

  //! Sift-down for a max-heap on the range [1..theSize].
  static void heapSiftDown(NCollection_Array1<std::pair<double, size_t>>& theHeap,
                           int                                            theIdx,
                           int                                            theSize)
  {
    while (true)
    {
      int       aLargest = theIdx;
      const int aLeft    = 2 * theIdx;
      const int aRight   = 2 * theIdx + 1;
      if (aLeft <= theSize && theHeap.Value(aLeft).first > theHeap.Value(aLargest).first)
      {
        aLargest = aLeft;
      }
      if (aRight <= theSize && theHeap.Value(aRight).first > theHeap.Value(aLargest).first)
      {
        aLargest = aRight;
      }
      if (aLargest == theIdx)
      {
        break;
      }
      std::swap(theHeap.ChangeValue(theIdx), theHeap.ChangeValue(aLargest));
      theIdx = aLargest;
    }
  }

  //! Sift-up for a max-heap on the range [1..theSize].
  static void heapSiftUp(NCollection_Array1<std::pair<double, size_t>>& theHeap, int theIdx)
  {
    while (theIdx > 1)
    {
      const int aParent = theIdx / 2;
      if (theHeap.Value(theIdx).first <= theHeap.Value(aParent).first)
      {
        break;
      }
      std::swap(theHeap.ChangeValue(theIdx), theHeap.ChangeValue(aParent));
      theIdx = aParent;
    }
  }

  //! Recursive build: partitions myIndices[theLo..theHi] into a balanced KD-Tree.
  //! @return maximum radius in the subtree (0.0 when HasRadii is false)
  double buildRecursive(int theLo, int theHi, int theDepth)
  {
    if (theLo > theHi)
    {
      return 0.0;
    }
    if (theLo == theHi)
    {
      if constexpr (HasRadii)
      {
        const double aR = myRadii.Value(static_cast<int>(myIndices.Value(theLo)));
        myMaxRadius.SetValue(theLo, aR);
        return aR;
      }
      return 0.0;
    }
    const int theAxis = theDepth % TheDimension;
    const int theMid  = (theLo + theHi) / 2;
    // Use std::nth_element on the index range to place the median
    std::nth_element(&myIndices.ChangeValue(theLo),
                     &myIndices.ChangeValue(theMid),
                     &myIndices.ChangeValue(theHi) + 1,
                     [this, theAxis](size_t a, size_t b) {
                       return myPoints.Value(static_cast<int>(a)).Coord(theAxis + 1)
                              < myPoints.Value(static_cast<int>(b)).Coord(theAxis + 1);
                     });
    if constexpr (HasRadii)
    {
      const double aLeftMax  = buildRecursive(theLo, theMid - 1, theDepth + 1);
      const double aRightMax = buildRecursive(theMid + 1, theHi, theDepth + 1);
      const double aNodeR    = myRadii.Value(static_cast<int>(myIndices.Value(theMid)));
      const double aMaxR     = std::max({aNodeR, aLeftMax, aRightMax});
      myMaxRadius.SetValue(theMid, aMaxR);
      return aMaxR;
    }
    else
    {
      buildRecursive(theLo, theMid - 1, theDepth + 1);
      buildRecursive(theMid + 1, theHi, theDepth + 1);
      return 0.0;
    }
  }

  //! Recursive nearest-neighbor search with bounding box pruning and leaf buckets.
  void nearestRecursive(const ThePointType& theQuery,
                        int                 theLo,
                        int                 theHi,
                        int                 theDepth,
                        size_t&             theBestIndex,
                        double&             theBestSqDist,
                        double              theBoundsMin[],
                        double              theBoundsMax[]) const
  {
    if (theLo > theHi)
    {
      return;
    }
    // Bounding box pruning: skip subtree if it cannot contain a closer point
    if (sqDistToBox(theQuery, theBoundsMin, theBoundsMax) >= theBestSqDist)
    {
      return;
    }
    // Leaf bucket: linear scan for small subtrees
    if (theHi - theLo + 1 <= THE_LEAF_SIZE)
    {
      for (int i = theLo; i <= theHi; ++i)
      {
        const size_t anIdx   = myIndices.Value(i);
        const double aSqDist = squareDistance(theQuery, myPoints.Value(static_cast<int>(anIdx)));
        if (aSqDist < theBestSqDist)
        {
          theBestSqDist = aSqDist;
          theBestIndex  = anIdx;
        }
      }
      return;
    }
    const int           theMid     = (theLo + theHi) / 2;
    const size_t        aNodeIndex = myIndices.Value(theMid);
    const ThePointType& aNodePoint = myPoints.Value(static_cast<int>(aNodeIndex));
    const double        aSqDist    = squareDistance(theQuery, aNodePoint);
    if (aSqDist < theBestSqDist)
    {
      theBestSqDist = aSqDist;
      theBestIndex  = aNodeIndex;
    }
    const int    theAxis   = theDepth % TheDimension;
    const double aSplitVal = aNodePoint.Coord(theAxis + 1);
    const double aDiff     = theQuery.Coord(theAxis + 1) - aSplitVal;
    // Visit closer subtree first, then farther subtree.
    // The bounding box check at the top of recursion handles pruning.
    if (aDiff <= 0.0)
    {
      const double aSavedMax = theBoundsMax[theAxis];
      theBoundsMax[theAxis]  = aSplitVal;
      nearestRecursive(theQuery,
                       theLo,
                       theMid - 1,
                       theDepth + 1,
                       theBestIndex,
                       theBestSqDist,
                       theBoundsMin,
                       theBoundsMax);
      theBoundsMax[theAxis]  = aSavedMax;
      const double aSavedMin = theBoundsMin[theAxis];
      theBoundsMin[theAxis]  = aSplitVal;
      nearestRecursive(theQuery,
                       theMid + 1,
                       theHi,
                       theDepth + 1,
                       theBestIndex,
                       theBestSqDist,
                       theBoundsMin,
                       theBoundsMax);
      theBoundsMin[theAxis] = aSavedMin;
    }
    else
    {
      const double aSavedMin = theBoundsMin[theAxis];
      theBoundsMin[theAxis]  = aSplitVal;
      nearestRecursive(theQuery,
                       theMid + 1,
                       theHi,
                       theDepth + 1,
                       theBestIndex,
                       theBestSqDist,
                       theBoundsMin,
                       theBoundsMax);
      theBoundsMin[theAxis]  = aSavedMin;
      const double aSavedMax = theBoundsMax[theAxis];
      theBoundsMax[theAxis]  = aSplitVal;
      nearestRecursive(theQuery,
                       theLo,
                       theMid - 1,
                       theDepth + 1,
                       theBestIndex,
                       theBestSqDist,
                       theBoundsMin,
                       theBoundsMax);
      theBoundsMax[theAxis] = aSavedMax;
    }
  }

  //! Recursive k-nearest search with bounding box pruning, max-heap, and leaf buckets.
  void kNearestRecursive(const ThePointType&                            theQuery,
                         int                                            theLo,
                         int                                            theHi,
                         int                                            theDepth,
                         NCollection_Array1<std::pair<double, size_t>>& theHeap,
                         size_t&                                        theHeapSize,
                         size_t                                         theK,
                         double                                         theBoundsMin[],
                         double                                         theBoundsMax[]) const
  {
    if (theLo > theHi)
    {
      return;
    }
    // Bounding box pruning: skip subtree if it cannot improve the K-th best
    if (theHeapSize == theK
        && sqDistToBox(theQuery, theBoundsMin, theBoundsMax) >= theHeap.Value(1).first)
    {
      return;
    }
    // Leaf bucket: linear scan for small subtrees
    if (theHi - theLo + 1 <= THE_LEAF_SIZE)
    {
      for (int i = theLo; i <= theHi; ++i)
      {
        const size_t anIdx   = myIndices.Value(i);
        const double aSqDist = squareDistance(theQuery, myPoints.Value(static_cast<int>(anIdx)));
        if (theHeapSize < theK)
        {
          ++theHeapSize;
          theHeap.SetValue(static_cast<int>(theHeapSize), {aSqDist, anIdx});
          heapSiftUp(theHeap, static_cast<int>(theHeapSize));
        }
        else if (aSqDist < theHeap.Value(1).first)
        {
          theHeap.SetValue(1, {aSqDist, anIdx});
          heapSiftDown(theHeap, 1, static_cast<int>(theHeapSize));
        }
      }
      return;
    }
    const int           theMid     = (theLo + theHi) / 2;
    const size_t        aNodeIndex = myIndices.Value(theMid);
    const ThePointType& aNodePoint = myPoints.Value(static_cast<int>(aNodeIndex));
    const double        aSqDist    = squareDistance(theQuery, aNodePoint);
    if (theHeapSize < theK)
    {
      ++theHeapSize;
      theHeap.SetValue(static_cast<int>(theHeapSize), {aSqDist, aNodeIndex});
      heapSiftUp(theHeap, static_cast<int>(theHeapSize));
    }
    else if (aSqDist < theHeap.Value(1).first)
    {
      theHeap.SetValue(1, {aSqDist, aNodeIndex});
      heapSiftDown(theHeap, 1, static_cast<int>(theHeapSize));
    }
    const int    theAxis   = theDepth % TheDimension;
    const double aSplitVal = aNodePoint.Coord(theAxis + 1);
    const double aDiff     = theQuery.Coord(theAxis + 1) - aSplitVal;
    // Visit closer subtree first, bounding box check at the top handles pruning
    if (aDiff <= 0.0)
    {
      const double aSavedMax = theBoundsMax[theAxis];
      theBoundsMax[theAxis]  = aSplitVal;
      kNearestRecursive(theQuery,
                        theLo,
                        theMid - 1,
                        theDepth + 1,
                        theHeap,
                        theHeapSize,
                        theK,
                        theBoundsMin,
                        theBoundsMax);
      theBoundsMax[theAxis]  = aSavedMax;
      const double aSavedMin = theBoundsMin[theAxis];
      theBoundsMin[theAxis]  = aSplitVal;
      kNearestRecursive(theQuery,
                        theMid + 1,
                        theHi,
                        theDepth + 1,
                        theHeap,
                        theHeapSize,
                        theK,
                        theBoundsMin,
                        theBoundsMax);
      theBoundsMin[theAxis] = aSavedMin;
    }
    else
    {
      const double aSavedMin = theBoundsMin[theAxis];
      theBoundsMin[theAxis]  = aSplitVal;
      kNearestRecursive(theQuery,
                        theMid + 1,
                        theHi,
                        theDepth + 1,
                        theHeap,
                        theHeapSize,
                        theK,
                        theBoundsMin,
                        theBoundsMax);
      theBoundsMin[theAxis]  = aSavedMin;
      const double aSavedMax = theBoundsMax[theAxis];
      theBoundsMax[theAxis]  = aSplitVal;
      kNearestRecursive(theQuery,
                        theLo,
                        theMid - 1,
                        theDepth + 1,
                        theHeap,
                        theHeapSize,
                        theK,
                        theBoundsMin,
                        theBoundsMax);
      theBoundsMax[theAxis] = aSavedMax;
    }
  }

  //! Recursive range search (sphere) with leaf buckets.
  void rangeSearchRecursive(const ThePointType&               theQuery,
                            double                            theRadiusSq,
                            int                               theLo,
                            int                               theHi,
                            int                               theDepth,
                            NCollection_DynamicArray<size_t>& theIndices) const
  {
    if (theLo > theHi)
    {
      return;
    }
    // Leaf bucket: linear scan for small subtrees
    if (theHi - theLo + 1 <= THE_LEAF_SIZE)
    {
      for (int i = theLo; i <= theHi; ++i)
      {
        const size_t anIdx   = myIndices.Value(i);
        const double aSqDist = squareDistance(theQuery, myPoints.Value(static_cast<int>(anIdx)));
        if (aSqDist <= theRadiusSq)
        {
          theIndices.Append(anIdx);
        }
      }
      return;
    }
    const int           theMid     = (theLo + theHi) / 2;
    const size_t        aNodeIndex = myIndices.Value(theMid);
    const ThePointType& aNodePoint = myPoints.Value(static_cast<int>(aNodeIndex));
    const double        aSqDist    = squareDistance(theQuery, aNodePoint);
    if (aSqDist <= theRadiusSq)
    {
      theIndices.Append(aNodeIndex);
    }
    if (theLo == theHi)
    {
      return;
    }
    const int    theAxis = theDepth % TheDimension;
    const double aDiff   = theQuery.Coord(theAxis + 1) - aNodePoint.Coord(theAxis + 1);
    // Always visit the near subtree
    if (aDiff <= 0.0)
    {
      rangeSearchRecursive(theQuery, theRadiusSq, theLo, theMid - 1, theDepth + 1, theIndices);
      if (aDiff * aDiff <= theRadiusSq)
      {
        rangeSearchRecursive(theQuery, theRadiusSq, theMid + 1, theHi, theDepth + 1, theIndices);
      }
    }
    else
    {
      rangeSearchRecursive(theQuery, theRadiusSq, theMid + 1, theHi, theDepth + 1, theIndices);
      if (aDiff * aDiff <= theRadiusSq)
      {
        rangeSearchRecursive(theQuery, theRadiusSq, theLo, theMid - 1, theDepth + 1, theIndices);
      }
    }
  }

  //! Recursive box search (AABB) with leaf buckets.
  void boxSearchRecursive(const ThePointType&               theMin,
                          const ThePointType&               theMax,
                          int                               theLo,
                          int                               theHi,
                          int                               theDepth,
                          NCollection_DynamicArray<size_t>& theIndices) const
  {
    if (theLo > theHi)
    {
      return;
    }
    // Leaf bucket: linear scan for small subtrees
    if (theHi - theLo + 1 <= THE_LEAF_SIZE)
    {
      for (int i = theLo; i <= theHi; ++i)
      {
        const size_t anIdx = myIndices.Value(i);
        if (isInsideBox(myPoints.Value(static_cast<int>(anIdx)), theMin, theMax))
        {
          theIndices.Append(anIdx);
        }
      }
      return;
    }
    const int           theMid     = (theLo + theHi) / 2;
    const size_t        aNodeIndex = myIndices.Value(theMid);
    const ThePointType& aNodePoint = myPoints.Value(static_cast<int>(aNodeIndex));
    if (isInsideBox(aNodePoint, theMin, theMax))
    {
      theIndices.Append(aNodeIndex);
    }
    if (theLo == theHi)
    {
      return;
    }
    const int    theAxis = theDepth % TheDimension;
    const double aCoord  = aNodePoint.Coord(theAxis + 1);
    if (aCoord >= theMin.Coord(theAxis + 1))
    {
      boxSearchRecursive(theMin, theMax, theLo, theMid - 1, theDepth + 1, theIndices);
    }
    if (aCoord <= theMax.Coord(theAxis + 1))
    {
      boxSearchRecursive(theMin, theMax, theMid + 1, theHi, theDepth + 1, theIndices);
    }
  }

  //! Recursive containing search: finds all spheres containing theQuery.
  //! Uses maxRadius pruning to skip subtrees where no sphere can reach the query.
  void containingSearchRecursive(const ThePointType&               theQuery,
                                 int                               theLo,
                                 int                               theHi,
                                 int                               theDepth,
                                 NCollection_DynamicArray<size_t>& theIndices,
                                 double                            theBoundsMin[],
                                 double                            theBoundsMax[]) const
  {
    if (theLo > theHi)
    {
      return;
    }
    // Prune: if min distance to subtree box > max radius in subtree, no sphere contains query
    const int    theMid = (theLo + theHi) / 2;
    const double aMaxR  = myMaxRadius.Value(theMid);
    if (sqDistToBox(theQuery, theBoundsMin, theBoundsMax) > aMaxR * aMaxR)
    {
      return;
    }
    // Leaf bucket: linear scan for small subtrees
    if (theHi - theLo + 1 <= THE_LEAF_SIZE)
    {
      for (int i = theLo; i <= theHi; ++i)
      {
        const size_t anIdx   = myIndices.Value(i);
        const double aR      = myRadii.Value(static_cast<int>(anIdx));
        const double aSqDist = squareDistance(theQuery, myPoints.Value(static_cast<int>(anIdx)));
        if (aSqDist <= aR * aR)
        {
          theIndices.Append(anIdx);
        }
      }
      return;
    }
    // Check current node
    const size_t        aNodeIndex = myIndices.Value(theMid);
    const ThePointType& aNodePoint = myPoints.Value(static_cast<int>(aNodeIndex));
    const double        aNodeR     = myRadii.Value(static_cast<int>(aNodeIndex));
    if (squareDistance(theQuery, aNodePoint) <= aNodeR * aNodeR)
    {
      theIndices.Append(aNodeIndex);
    }
    // Recurse both subtrees with bounds tracking
    const int    theAxis   = theDepth % TheDimension;
    const double aSplitVal = aNodePoint.Coord(theAxis + 1);
    const double aSavedMax = theBoundsMax[theAxis];
    theBoundsMax[theAxis]  = aSplitVal;
    containingSearchRecursive(theQuery,
                              theLo,
                              theMid - 1,
                              theDepth + 1,
                              theIndices,
                              theBoundsMin,
                              theBoundsMax);
    theBoundsMax[theAxis]  = aSavedMax;
    const double aSavedMin = theBoundsMin[theAxis];
    theBoundsMin[theAxis]  = aSplitVal;
    containingSearchRecursive(theQuery,
                              theMid + 1,
                              theHi,
                              theDepth + 1,
                              theIndices,
                              theBoundsMin,
                              theBoundsMax);
    theBoundsMin[theAxis] = aSavedMin;
  }

  //! Recursive nearest-weighted search: finds point minimizing dist - radius.
  //! Uses bounding box + maxRadius pruning to skip subtrees.
  void nearestWeightedRecursive(const ThePointType& theQuery,
                                int                 theLo,
                                int                 theHi,
                                int                 theDepth,
                                size_t&             theBestIndex,
                                double&             theBestGap,
                                double              theBoundsMin[],
                                double              theBoundsMax[]) const
  {
    if (theLo > theHi)
    {
      return;
    }
    // Prune: minimum possible gap = sqrt(sqDistToBox) - maxRadius.
    // To avoid sqrt: if (bestGap + maxRadius) >= 0, then sqDistToBox >= (bestGap + maxRadius)^2.
    const int    theMid          = (theLo + theHi) / 2;
    const double aMaxR           = myMaxRadius.Value(theMid);
    const double aSqDistToBox    = sqDistToBox(theQuery, theBoundsMin, theBoundsMax);
    const double aPruneThreshold = theBestGap + aMaxR;
    if (aPruneThreshold >= 0.0 && aSqDistToBox >= aPruneThreshold * aPruneThreshold)
    {
      return;
    }
    // Leaf bucket: linear scan for small subtrees
    if (theHi - theLo + 1 <= THE_LEAF_SIZE)
    {
      for (int i = theLo; i <= theHi; ++i)
      {
        const size_t anIdx = myIndices.Value(i);
        const double aDist =
          std::sqrt(squareDistance(theQuery, myPoints.Value(static_cast<int>(anIdx))));
        const double aGap = aDist - myRadii.Value(static_cast<int>(anIdx));
        if (aGap < theBestGap)
        {
          theBestGap   = aGap;
          theBestIndex = anIdx;
        }
      }
      return;
    }
    // Check current node
    const size_t        aNodeIndex = myIndices.Value(theMid);
    const ThePointType& aNodePoint = myPoints.Value(static_cast<int>(aNodeIndex));
    const double        aDist      = std::sqrt(squareDistance(theQuery, aNodePoint));
    const double        aGap       = aDist - myRadii.Value(static_cast<int>(aNodeIndex));
    if (aGap < theBestGap)
    {
      theBestGap   = aGap;
      theBestIndex = aNodeIndex;
    }
    // Recurse with bounds tracking, visit closer subtree first
    const int    theAxis   = theDepth % TheDimension;
    const double aSplitVal = aNodePoint.Coord(theAxis + 1);
    const double aDiff     = theQuery.Coord(theAxis + 1) - aSplitVal;
    if (aDiff <= 0.0)
    {
      const double aSavedMax = theBoundsMax[theAxis];
      theBoundsMax[theAxis]  = aSplitVal;
      nearestWeightedRecursive(theQuery,
                               theLo,
                               theMid - 1,
                               theDepth + 1,
                               theBestIndex,
                               theBestGap,
                               theBoundsMin,
                               theBoundsMax);
      theBoundsMax[theAxis]  = aSavedMax;
      const double aSavedMin = theBoundsMin[theAxis];
      theBoundsMin[theAxis]  = aSplitVal;
      nearestWeightedRecursive(theQuery,
                               theMid + 1,
                               theHi,
                               theDepth + 1,
                               theBestIndex,
                               theBestGap,
                               theBoundsMin,
                               theBoundsMax);
      theBoundsMin[theAxis] = aSavedMin;
    }
    else
    {
      const double aSavedMin = theBoundsMin[theAxis];
      theBoundsMin[theAxis]  = aSplitVal;
      nearestWeightedRecursive(theQuery,
                               theMid + 1,
                               theHi,
                               theDepth + 1,
                               theBestIndex,
                               theBestGap,
                               theBoundsMin,
                               theBoundsMax);
      theBoundsMin[theAxis]  = aSavedMin;
      const double aSavedMax = theBoundsMax[theAxis];
      theBoundsMax[theAxis]  = aSplitVal;
      nearestWeightedRecursive(theQuery,
                               theLo,
                               theMid - 1,
                               theDepth + 1,
                               theBestIndex,
                               theBestGap,
                               theBoundsMin,
                               theBoundsMax);
      theBoundsMax[theAxis] = aSavedMax;
    }
  }

private:
  NCollection_Array1<ThePointType> myPoints; //!< Copy of input points (1-based)
  NCollection_Array1<size_t> myIndices; //!< Permutation array encoding tree structure (1-based)
  NCollection_Array1<double> myRadii;   //!< Per-point radii (1-based, only used when HasRadii)
  NCollection_Array1<double>
         myMaxRadius; //!< Max radius in subtree rooted at [mid] (only used when HasRadii)
  size_t mySize;      //!< Number of points
};

#endif

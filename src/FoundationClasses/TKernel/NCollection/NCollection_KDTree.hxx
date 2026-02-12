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

//! @brief Static KD-Tree for efficient point set queries.
//!
//! NCollection_KDTree is a balanced KD-Tree built once from a set of points,
//! supporting efficient nearest-neighbor, k-nearest, range (sphere), and
//! box (AABB) queries.
//!
//! Key features:
//! - O(N log N) construction via median-split (std::nth_element)
//! - O(log N) nearest-neighbor search with hyperplane pruning
//! - O(N^(1-1/d) + k) range and box search
//! - Cache-friendly permutation-based layout (no node allocations)
//! - Works with any point type providing Coord(int) with 1-based indexing
//!
//! Best suited for:
//! - Static point clouds (build once, query many times)
//! - Nearest-neighbor and k-nearest queries
//! - Spatial range filtering
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
template <class ThePointType, int TheDimension>
class NCollection_KDTree
{
public:
  //! Empty constructor. Creates an empty tree.
  NCollection_KDTree()
      : mySize(0)
  {
  }

  //! Build the tree from a C array of points.
  //! @param[in] thePoints pointer to contiguous array of points
  //! @param[in] theCount  number of points
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

  //! Build the tree from an NCollection_Array1.
  //! @param[in] thePoints array of points (any lower bound)
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
  }

  //! Returns the point at the given 1-based original index.
  //! @param[in] theIndex 1-based point index
  //! @return const reference to the point
  const ThePointType& Point(size_t theIndex) const
  {
    return myPoints.Value(static_cast<int>(theIndex));
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
    nearestRecursive(theQuery, 1, static_cast<int>(mySize), 0, aBestIndex, aBestSqDist);
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
    nearestRecursive(theQuery, 1, static_cast<int>(mySize), 0, aBestIndex, aBestSqDist);
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
    kNearestRecursive(theQuery, 1, static_cast<int>(mySize), 0, aHeap, aHeapSize, anActualK);
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

  //! Copy constructor.
  NCollection_KDTree(const NCollection_KDTree& theOther) = default;

  //! Move constructor.
  NCollection_KDTree(NCollection_KDTree&& theOther) noexcept
      : myPoints(std::move(theOther.myPoints)),
        myIndices(std::move(theOther.myIndices)),
        mySize(theOther.mySize)
  {
    theOther.mySize = 0;
  }

  //! Copy assignment.
  NCollection_KDTree& operator=(const NCollection_KDTree& theOther) = default;

  //! Move assignment.
  NCollection_KDTree& operator=(NCollection_KDTree&& theOther) noexcept
  {
    if (this != &theOther)
    {
      myPoints        = std::move(theOther.myPoints);
      myIndices       = std::move(theOther.myIndices);
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
  void buildRecursive(int theLo, int theHi, int theDepth)
  {
    if (theLo >= theHi)
    {
      return;
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
    buildRecursive(theLo, theMid - 1, theDepth + 1);
    buildRecursive(theMid + 1, theHi, theDepth + 1);
  }

  //! Recursive nearest-neighbor search with hyperplane pruning.
  void nearestRecursive(const ThePointType& theQuery,
                        int                 theLo,
                        int                 theHi,
                        int                 theDepth,
                        size_t&             theBestIndex,
                        double&             theBestSqDist) const
  {
    if (theLo > theHi)
    {
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
    if (theLo == theHi)
    {
      return;
    }
    const int    theAxis = theDepth % TheDimension;
    const double aDiff   = theQuery.Coord(theAxis + 1) - aNodePoint.Coord(theAxis + 1);
    // Visit closer subtree first
    const int aFirstLo  = aDiff <= 0.0 ? theLo : theMid + 1;
    const int aFirstHi  = aDiff <= 0.0 ? theMid - 1 : theHi;
    const int aSecondLo = aDiff <= 0.0 ? theMid + 1 : theLo;
    const int aSecondHi = aDiff <= 0.0 ? theHi : theMid - 1;
    nearestRecursive(theQuery, aFirstLo, aFirstHi, theDepth + 1, theBestIndex, theBestSqDist);
    // Prune far subtree if the splitting plane is farther than current best
    if (aDiff * aDiff < theBestSqDist)
    {
      nearestRecursive(theQuery, aSecondLo, aSecondHi, theDepth + 1, theBestIndex, theBestSqDist);
    }
  }

  //! Recursive k-nearest search with max-heap pruning.
  void kNearestRecursive(const ThePointType&                            theQuery,
                         int                                            theLo,
                         int                                            theHi,
                         int                                            theDepth,
                         NCollection_Array1<std::pair<double, size_t>>& theHeap,
                         size_t&                                        theHeapSize,
                         size_t                                         theK) const
  {
    if (theLo > theHi)
    {
      return;
    }
    const int           theMid     = (theLo + theHi) / 2;
    const size_t        aNodeIndex = myIndices.Value(theMid);
    const ThePointType& aNodePoint = myPoints.Value(static_cast<int>(aNodeIndex));
    const double        aSqDist    = squareDistance(theQuery, aNodePoint);
    if (theHeapSize < theK)
    {
      // Heap not full, insert
      ++theHeapSize;
      theHeap.SetValue(static_cast<int>(theHeapSize), {aSqDist, aNodeIndex});
      heapSiftUp(theHeap, static_cast<int>(theHeapSize));
    }
    else if (aSqDist < theHeap.Value(1).first)
    {
      // Replace max in heap
      theHeap.SetValue(1, {aSqDist, aNodeIndex});
      heapSiftDown(theHeap, 1, static_cast<int>(theHeapSize));
    }
    if (theLo == theHi)
    {
      return;
    }
    const int    theAxis   = theDepth % TheDimension;
    const double aDiff     = theQuery.Coord(theAxis + 1) - aNodePoint.Coord(theAxis + 1);
    const int    aFirstLo  = aDiff <= 0.0 ? theLo : theMid + 1;
    const int    aFirstHi  = aDiff <= 0.0 ? theMid - 1 : theHi;
    const int    aSecondLo = aDiff <= 0.0 ? theMid + 1 : theLo;
    const int    aSecondHi = aDiff <= 0.0 ? theHi : theMid - 1;
    kNearestRecursive(theQuery, aFirstLo, aFirstHi, theDepth + 1, theHeap, theHeapSize, theK);
    // Prune far subtree
    if (theHeapSize < theK || aDiff * aDiff < theHeap.Value(1).first)
    {
      kNearestRecursive(theQuery, aSecondLo, aSecondHi, theDepth + 1, theHeap, theHeapSize, theK);
    }
  }

  //! Recursive range search (sphere).
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

  //! Recursive box search (AABB).
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
    // Left subtree has points with coord <= aCoord on this axis
    // Only visit if box's max on this axis >= smallest possible (which is always true for left)
    // but prune if aCoord < theMin on this axis (all left children are even smaller)
    if (aCoord >= theMin.Coord(theAxis + 1))
    {
      boxSearchRecursive(theMin, theMax, theLo, theMid - 1, theDepth + 1, theIndices);
    }
    // Right subtree has points with coord >= aCoord on this axis
    // Prune if aCoord > theMax on this axis
    if (aCoord <= theMax.Coord(theAxis + 1))
    {
      boxSearchRecursive(theMin, theMax, theMid + 1, theHi, theDepth + 1, theIndices);
    }
  }

private:
  NCollection_Array1<ThePointType> myPoints; //!< Copy of input points (1-based)
  NCollection_Array1<size_t> myIndices; //!< Permutation array encoding tree structure (1-based)
  size_t                     mySize;    //!< Number of points
};

#endif

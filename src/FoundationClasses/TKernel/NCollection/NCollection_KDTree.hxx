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

#ifndef NCollection_KDTree_HeaderFile
#define NCollection_KDTree_HeaderFile

#include <NCollection_Array1.hxx>
#include <NCollection_Vector.hxx>
#include <Standard_OutOfRange.hxx>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>

//! @brief Template KD-Tree for efficient nearest neighbor queries.
//!
//! A k-dimensional tree (KD-Tree) is a space-partitioning data structure
//! for organizing points in a k-dimensional space. It enables efficient
//! nearest neighbor queries with O(log N) average complexity.
//!
//! @tparam PointType Type of points stored (must support operator[] for coordinates)
//! @tparam N Dimensionality of the space (default 3 for 3D points)
//! @tparam CoordAccessor Functor to access coordinates from PointType
//!
//! Usage example with gp_Pnt:
//! @code
//! struct GpPntAccessor {
//!   static double Coord(const gp_Pnt& thePnt, int theAxis) {
//!     return thePnt.Coord(theAxis + 1); // gp_Pnt uses 1-based indexing
//!   }
//! };
//! NCollection_KDTree<gp_Pnt, 3, GpPntAccessor> aTree;
//! aTree.Build(myPoints);
//! aTree.FindKNearest(queryPt, 5, indices, distances);
//! @endcode
template <class PointType,
          int   N = 3,
          class CoordAccessor =
            std::function<double(const PointType&, int)>>
class NCollection_KDTree
{
public:
  //! Default coordinate accessor using operator[].
  struct DefaultAccessor
  {
    static double Coord(const PointType& thePnt, int theAxis) { return thePnt[theAxis]; }
  };

  //! KD-Tree node structure.
  struct Node
  {
    int PointIndex; //!< Index of point in original array (-1 for internal nodes)
    int Left;       //!< Index of left child (-1 if none)
    int Right;      //!< Index of right child (-1 if none)
    int SplitAxis;  //!< Axis used for splitting (0 to N-1)

    Node()
        : PointIndex(-1),
          Left(-1),
          Right(-1),
          SplitAxis(0)
    {
    }
  };

  //! Result entry for nearest neighbor queries.
  struct NearestEntry
  {
    int    Index;          //!< Index of point in original array
    double SquareDistance; //!< Squared distance to query point

    NearestEntry(int theIndex = -1, double theSqDist = 0.0)
        : Index(theIndex),
          SquareDistance(theSqDist)
    {
    }

    bool operator<(const NearestEntry& theOther) const
    {
      return SquareDistance < theOther.SquareDistance;
    }

    bool operator>(const NearestEntry& theOther) const
    {
      return SquareDistance > theOther.SquareDistance;
    }
  };

public:
  //! Default constructor. Creates empty tree.
  NCollection_KDTree()
      : myPoints(nullptr),
        myIsBuilt(false)
  {
  }

  //! Constructor with immediate build.
  //! @param[in] thePoints array of points to index
  explicit NCollection_KDTree(const NCollection_Array1<PointType>& thePoints)
      : myPoints(nullptr),
        myIsBuilt(false)
  {
    Build(thePoints);
  }

  //! Build the KD-Tree from array of points.
  //! @param[in] thePoints array of points to index (must remain valid during tree lifetime)
  void Build(const NCollection_Array1<PointType>& thePoints);

  //! Clear the tree and release memory.
  void Clear()
  {
    myNodes.Clear();
    myPoints  = nullptr;
    myIsBuilt = false;
  }

  //! Rebuild the tree (useful after external point modifications).
  void Rebuild()
  {
    if (myPoints != nullptr)
    {
      const NCollection_Array1<PointType>* aPoints = myPoints;
      Clear();
      Build(*aPoints);
    }
  }

  //! @return true if tree is built and ready for queries
  bool IsBuilt() const { return myIsBuilt; }

  //! @return true if tree is empty
  bool IsEmpty() const { return myNodes.IsEmpty(); }

  //! @return number of points in the tree
  int Size() const { return myPoints != nullptr ? myPoints->Length() : 0; }

  //! Find K nearest neighbors to query point.
  //! @param[in] theQuery query point
  //! @param[in] theK number of neighbors to find
  //! @param[out] theIndices indices of found neighbors (sorted by distance)
  //! @param[out] theSquareDistances squared distances to found neighbors
  void FindKNearest(const PointType&        theQuery,
                    int                     theK,
                    NCollection_Vector<int>&    theIndices,
                    NCollection_Vector<double>& theSquareDistances) const;

  //! Find nearest neighbor to query point.
  //! @param[in] theQuery query point
  //! @param[out] theIndex index of nearest neighbor (-1 if tree is empty)
  //! @param[out] theSquareDistance squared distance to nearest neighbor
  //! @return true if neighbor found
  bool FindNearest(const PointType& theQuery, int& theIndex, double& theSquareDistance) const;

  //! Find all points within given radius.
  //! @param[in] theQuery query point
  //! @param[in] theRadius search radius
  //! @param[out] theIndices indices of found points
  //! @param[out] theSquareDistances squared distances to found points (optional)
  void FindInRadius(const PointType&            theQuery,
                    double                      theRadius,
                    NCollection_Vector<int>&    theIndices,
                    NCollection_Vector<double>* theSquareDistances = nullptr) const;

  //! Access node by index (for debugging/testing).
  //! @param[in] theIndex node index
  //! @return node at given index
  const Node& GetNode(int theIndex) const { return myNodes.Value(theIndex); }

  //! @return number of nodes in tree
  int NbNodes() const { return myNodes.Length(); }

protected:
  //! Compute squared distance between two points.
  static double squareDistance(const PointType& thePnt1, const PointType& thePnt2);

  //! Get coordinate value from point.
  static double getCoord(const PointType& thePnt, int theAxis);

  //! Build subtree recursively.
  //! @param[in] theIndices array of point indices to partition
  //! @param[in] theStart start index in theIndices
  //! @param[in] theEnd end index in theIndices (exclusive)
  //! @param[in] theDepth current depth (determines split axis)
  //! @return index of created node
  int buildSubtree(NCollection_Vector<int>& theIndices, int theStart, int theEnd, int theDepth);

  //! Search for K nearest neighbors recursively.
  //! @param[in] theNodeIdx current node index
  //! @param[in] theQuery query point
  //! @param[in] theK number of neighbors
  //! @param[in,out] theHeap max-heap of current best neighbors
  void searchKNearest(int                                                          theNodeIdx,
                      const PointType&                                             theQuery,
                      int                                                          theK,
                      std::priority_queue<NearestEntry, std::vector<NearestEntry>>& theHeap) const;

  //! Search for points within radius recursively.
  //! @param[in] theNodeIdx current node index
  //! @param[in] theQuery query point
  //! @param[in] theRadiusSq squared search radius
  //! @param[out] theIndices indices of found points
  //! @param[out] theSquareDistances squared distances (optional)
  void searchInRadius(int                         theNodeIdx,
                      const PointType&            theQuery,
                      double                      theRadiusSq,
                      NCollection_Vector<int>&    theIndices,
                      NCollection_Vector<double>* theSquareDistances) const;

private:
  NCollection_Vector<Node>             myNodes;   //!< Tree nodes
  const NCollection_Array1<PointType>* myPoints;  //!< Pointer to original points array
  bool                                 myIsBuilt; //!< Flag indicating tree is ready
};

// Include template implementation
#include <NCollection_KDTree.lxx>

#endif // NCollection_KDTree_HeaderFile

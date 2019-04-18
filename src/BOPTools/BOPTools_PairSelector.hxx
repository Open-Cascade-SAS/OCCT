// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef BOPTools_PairSelector_HeaderFile
#define BOPTools_PairSelector_HeaderFile

#include <BVH_Traverse.hxx>
#include <BVH_BoxSet.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <algorithm>

//! Template Selector for selection of the elements from two BVH trees.
template <int Dimension>
class BOPTools_PairSelector :
  public BVH_PairTraverse <Standard_Real, Dimension, BVH_BoxSet <Standard_Real, Dimension, Standard_Integer>>
{
public: //! @name public types

  //! Auxiliary structure to keep the pair of indices
  struct PairIDs
  {
    PairIDs (const Standard_Integer theId1 = -1,
             const Standard_Integer theId2 = -1)
      : ID1 (theId1), ID2 (theId2)
    {}

    Standard_Boolean operator< (const PairIDs& theOther) const
    {
      return ID1 < theOther.ID1 ||
            (ID1 == theOther.ID1 && ID2 < theOther.ID2);
    }

    Standard_Integer ID1;
    Standard_Integer ID2;
  };

  typedef typename BVH::VectorType<Standard_Real, Dimension>::Type BVH_VecNd;

public: //! @name Constructor

  //! Empty constructor
  BOPTools_PairSelector()
    : mySameBVHs (Standard_False)
  {}

public: //! @name public interfaces

  //! Clears the indices
  void Clear()
  {
    myPairs.Clear();
  }

  //! Sorts the indices
  void Sort()
  {
    std::sort (myPairs.begin(), myPairs.end());
  }

  //! Tells to selector that BVH trees are the same.
  //! If the flag is set to true the resulting vector will contain
  //! only unique pairs (mirrored pairs will be rejected,
  //! e.g. (1, 2) will be taken, (2, 1) will be rejected) and will
  //! not contain pairs in which IDs are the same (pair (1, 1) will be rejected).
  //! If it is required to have a full vector of pairs even
  //! for the same BVH trees, just keep the false value of this flag.
  void SetSame (const Standard_Boolean theIsSame)
  {
    mySameBVHs = theIsSame;
  }

  //! Returns the list of accepted indices
  const std::vector<PairIDs>& Pairs() const
  {
    return myPairs;
  }

public: //! @name Rejection/Acceptance rules

  //! Basing on the bounding boxes of the nodes checks if the pair of nodes should be rejected.
  virtual Standard_Boolean RejectNode (const BVH_VecNd& theCMin1,
                                       const BVH_VecNd& theCMax1,
                                       const BVH_VecNd& theCMin2,
                                       const BVH_VecNd& theCMax2,
                                       Standard_Real&) const Standard_OVERRIDE
  {
    return BVH_Box<Standard_Real, 3> (theCMin1, theCMax1).IsOut (theCMin2, theCMax2);
  }

  //! Checks if the pair of elements should be rejected.
  Standard_Boolean RejectElement (const Standard_Integer theID1,
                                  const Standard_Integer theID2)
  {
    return (mySameBVHs && theID1 >= theID2) ||
            this->myBVHSet1->Box (theID1).IsOut(
            this->myBVHSet2->Box (theID2));
  }

  //! Checks and accepts the pair of elements.
  virtual Standard_Boolean Accept (const Standard_Integer theID1,
                                   const Standard_Integer theID2) Standard_OVERRIDE
  {
    if (!RejectElement (theID1, theID2))
    {
      myPairs.push_back (PairIDs (this->myBVHSet1->Element (theID1),
                                  this->myBVHSet2->Element (theID2)));
      return Standard_True;
    }
    return Standard_False;
  }

protected: //! @name Fields

  std::vector<PairIDs> myPairs; //!< Selected pairs of indices
  Standard_Boolean mySameBVHs;  //!< Selection is performed from the same BVH trees
};

#endif

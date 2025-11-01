// Created on: 2014-01-09
// Created by: Denis BOGOLEPOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifndef BVH_SweepPlaneBuilder_HeaderFile
#define BVH_SweepPlaneBuilder_HeaderFile

#include <BVH_QueueBuilder.hxx>
#include <BVH_QuickSorter.hxx>
#include <NCollection_Array1.hxx>

//! Performs building of BVH tree using sweep plane SAH algorithm.
template <class T, int N>
class BVH_SweepPlaneBuilder : public BVH_QueueBuilder<T, N>
{
public:
  //! Creates sweep plane SAH BVH builder.
  BVH_SweepPlaneBuilder(const Standard_Integer theLeafNodeSize = BVH_Constants_LeafNodeSizeDefault,
                        const Standard_Integer theMaxTreeDepth = BVH_Constants_MaxTreeDepth,
                        const Standard_Integer theNumOfThreads = 1)
      : BVH_QueueBuilder<T, N>(theLeafNodeSize, theMaxTreeDepth, theNumOfThreads)
  {
  }

  //! Releases resources of sweep plane SAH BVH builder.
  virtual ~BVH_SweepPlaneBuilder() {}

protected:
  //! Performs splitting of the given BVH node.
  typename BVH_QueueBuilder<T, N>::BVH_ChildNodes buildNode(BVH_Set<T, N>*         theSet,
                                                            BVH_Tree<T, N>*        theBVH,
                                                            const Standard_Integer theNode) const
    Standard_OVERRIDE
  {
    const Standard_Integer aNodeBegPrimitive = theBVH->BegPrimitive(theNode);
    const Standard_Integer aNodeEndPrimitive = theBVH->EndPrimitive(theNode);
    const Standard_Integer aNodeNbPrimitives = theBVH->NbPrimitives(theNode);
    if (aNodeEndPrimitive - aNodeBegPrimitive < BVH_Builder<T, N>::myLeafNodeSize)
    {
      // clang-format off
      return typename BVH_QueueBuilder<T, N>::BVH_ChildNodes(); // node does not require partitioning
      // clang-format on
    }

    // Parameters for storing best split
    Standard_Integer aMinSplitAxis  = -1;
    Standard_Integer aMinSplitIndex = 0;

    NCollection_Array1<Standard_Real> aLftSet(0, aNodeNbPrimitives - 1);
    NCollection_Array1<Standard_Real> aRghSet(0, aNodeNbPrimitives - 1);
    Standard_Real                     aMinSplitCost = std::numeric_limits<Standard_Real>::max();

    // Find best split
    for (Standard_Integer anAxis = 0; anAxis < (N < 4 ? N : 3); ++anAxis)
    {
      const T aNodeSize = BVH::VecComp<T, N>::Get(theBVH->MaxPoint(theNode), anAxis)
                          - BVH::VecComp<T, N>::Get(theBVH->MinPoint(theNode), anAxis);
      if (aNodeSize <= BVH::THE_NODE_MIN_SIZE)
      {
        continue;
      }

      BVH_QuickSorter<T, N>(anAxis).Perform(theSet, aNodeBegPrimitive, aNodeEndPrimitive);
      BVH_Box<T, N> aLftBox;
      BVH_Box<T, N> aRghBox;
      aLftSet.ChangeFirst() = std::numeric_limits<T>::max();
      aRghSet.ChangeFirst() = std::numeric_limits<T>::max();

      // Sweep from left
      for (Standard_Integer anIndex = 1; anIndex < aNodeNbPrimitives; ++anIndex)
      {
        aLftBox.Combine(theSet->Box(anIndex + aNodeBegPrimitive - 1));
        aLftSet(anIndex) = static_cast<Standard_Real>(aLftBox.Area());
      }

      // Sweep from right
      for (Standard_Integer anIndex = 1; anIndex < aNodeNbPrimitives; ++anIndex)
      {
        aRghBox.Combine(theSet->Box(aNodeEndPrimitive - anIndex + 1));
        aRghSet(anIndex) = static_cast<Standard_Real>(aRghBox.Area());
      }

      // Find best split using simplified SAH
      for (Standard_Integer aNbLft = 1, aNbRgh = aNodeNbPrimitives - 1; aNbLft < aNodeNbPrimitives;
           ++aNbLft, --aNbRgh)
      {
        Standard_Real aCost = (aLftSet(aNbLft) /* / aNodeArea */) * aNbLft
                              + (aRghSet(aNbRgh) /* / aNodeArea */) * aNbRgh;
        if (aCost < aMinSplitCost)
        {
          aMinSplitCost  = aCost;
          aMinSplitAxis  = anAxis;
          aMinSplitIndex = aNbLft;
        }
      }
    }

    if (aMinSplitAxis == -1)
    {
      return typename BVH_QueueBuilder<T, N>::BVH_ChildNodes(); // failed to find split axis
    }

    theBVH->SetInner(theNode);
    if (aMinSplitAxis != (N < 4 ? N - 1 : 2))
    {
      BVH_QuickSorter<T, N>(aMinSplitAxis).Perform(theSet, aNodeBegPrimitive, aNodeEndPrimitive);
    }

    BVH_Box<T, N> aMinSplitBoxLft;
    BVH_Box<T, N> aMinSplitBoxRgh;

    // Compute bounding boxes for selected split plane
    for (Standard_Integer anIndex = aNodeBegPrimitive; anIndex < aMinSplitIndex + aNodeBegPrimitive;
         ++anIndex)
    {
      aMinSplitBoxLft.Combine(theSet->Box(anIndex));
    }

    for (Standard_Integer anIndex = aNodeEndPrimitive;
         anIndex >= aMinSplitIndex + aNodeBegPrimitive;
         --anIndex)
    {
      aMinSplitBoxRgh.Combine(theSet->Box(anIndex));
    }

    const Standard_Integer aMiddle = aNodeBegPrimitive + aMinSplitIndex;
    typedef typename BVH_QueueBuilder<T, N>::BVH_PrimitiveRange Range;
    return typename BVH_QueueBuilder<T, N>::BVH_ChildNodes(aMinSplitBoxLft,
                                                           aMinSplitBoxRgh,
                                                           Range(aNodeBegPrimitive, aMiddle - 1),
                                                           Range(aMiddle, aNodeEndPrimitive));
  }
};

#endif // _BVH_SweepPlaneBuilder_Header

// Created on: 2013-12-20
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

#ifndef _BVH_BinnedBuilder_Header
#define _BVH_BinnedBuilder_Header

#include <BVH_QueueBuilder.hxx>

#include <algorithm>

#if defined (_WIN32) && defined (max)
  #undef max
#endif

#include <limits>

//! Stores parameters of single bin (slice of AABB).
template<class T, int N>
struct BVH_Bin
{
  //! Creates new node bin.
  BVH_Bin() : Count (0) {}

  Standard_Integer Count; //!< Number of primitives in the bin
  BVH_Box<T, N>    Box;   //!< AABB of primitives in the bin
};

//! Performs construction of BVH tree using binned SAH algorithm. Number
//! of bins controls BVH quality in cost of construction time (greater -
//! better). For optimal results, use 32 - 48 bins. However, reasonable
//! performance is provided even for 4 - 8 bins (it is only 10-20% lower
//! in comparison with optimal settings). Note that multiple threads can
//! be used only with thread safe BVH primitive sets.
template<class T, int N, int Bins = BVH_Constants_NbBinsOptimal>
class BVH_BinnedBuilder : public BVH_QueueBuilder<T, N>
{
public:

  //! Type of the array of bins of BVH tree node.
  typedef BVH_Bin<T, N> BVH_BinVector[Bins];

  //! Describes split plane candidate.
  struct BVH_SplitPlane
  {
    BVH_Bin<T, N> LftVoxel;
    BVH_Bin<T, N> RghVoxel;
  };

  //! Type of the array of split plane candidates.
  typedef BVH_SplitPlane BVH_SplitPlanes[Bins + 1];

public:

  //! Creates binned SAH BVH builder.
  BVH_BinnedBuilder (const Standard_Integer theLeafNodeSize = BVH_Constants_LeafNodeSizeDefault,
                     const Standard_Integer theMaxTreeDepth = BVH_Constants_MaxTreeDepth,
                     const Standard_Boolean theDoMainSplits = Standard_False,
                     const Standard_Integer theNumOfThreads = 1)
  : BVH_QueueBuilder<T, N> (theLeafNodeSize, theMaxTreeDepth, theNumOfThreads),
    myUseMainAxis (theDoMainSplits)
  {
    //
  }

  //! Releases resources of binned SAH BVH builder.
  virtual ~BVH_BinnedBuilder() {}

protected:

  //! Performs splitting of the given BVH node.
  virtual typename BVH_QueueBuilder<T, N>::BVH_ChildNodes buildNode (BVH_Set<T, N>*         theSet,
                                                                     BVH_Tree<T, N>*        theBVH,
                                                                     const Standard_Integer theNode) const Standard_OVERRIDE;

  //! Arranges node primitives into bins.
  virtual void getSubVolumes (BVH_Set<T, N>*         theSet,
                              BVH_Tree<T, N>*        theBVH,
                              const Standard_Integer theNode,
                              BVH_BinVector&         theBins,
                              const Standard_Integer theAxis) const;

private:

  Standard_Boolean myUseMainAxis; //!< Defines whether to search for the best split or use the widest axis

};

// =======================================================================
// function : getSubVolumes
// purpose  :
// =======================================================================
template<class T, int N, int Bins>
void BVH_BinnedBuilder<T, N, Bins>::getSubVolumes (BVH_Set<T, N>*         theSet,
                                                   BVH_Tree<T, N>*        theBVH,
                                                   const Standard_Integer theNode,
                                                   BVH_BinVector&         theBins,
                                                   const Standard_Integer theAxis) const
{
  const T aMin = BVH::VecComp<T, N>::Get (theBVH->MinPoint (theNode), theAxis);
  const T aMax = BVH::VecComp<T, N>::Get (theBVH->MaxPoint (theNode), theAxis);
  const T anInverseStep = static_cast<T> (Bins) / (aMax - aMin);
  for (Standard_Integer anIdx = theBVH->BegPrimitive (theNode); anIdx <= theBVH->EndPrimitive (theNode); ++anIdx)
  {
    typename BVH_Set<T, N>::BVH_BoxNt aBox = theSet->Box (anIdx);
    Standard_Integer aBinIndex = BVH::IntFloor<T> ((theSet->Center (anIdx, theAxis) - aMin) * anInverseStep);
    if (aBinIndex < 0)
    {
      aBinIndex = 0;
    }
    else if (aBinIndex >= Bins)
    {
      aBinIndex = Bins - 1;
    }

    theBins[aBinIndex].Count++;
    theBins[aBinIndex].Box.Combine (aBox);
  }
}

namespace BVH
{
  template<class T, int N>
  Standard_Integer SplitPrimitives (BVH_Set<T, N>*         theSet,
                                    const BVH_Box<T, N>&   theBox,
                                    const Standard_Integer theBeg,
                                    const Standard_Integer theEnd,
                                    const Standard_Integer theBin,
                                    const Standard_Integer theAxis,
                                    const Standard_Integer theBins)
  {
    const T aMin = BVH::VecComp<T, N>::Get (theBox.CornerMin(), theAxis);
    const T aMax = BVH::VecComp<T, N>::Get (theBox.CornerMax(), theAxis);

    const T anInverseStep = static_cast<T> (theBins) / (aMax - aMin);

    Standard_Integer aLftIdx (theBeg);
    Standard_Integer aRghIdx (theEnd);

    do
    {
      while (BVH::IntFloor<T> ((theSet->Center (aLftIdx, theAxis) - aMin) * anInverseStep) <= theBin && aLftIdx < theEnd)
      {
        ++aLftIdx;
      }
      while (BVH::IntFloor<T> ((theSet->Center (aRghIdx, theAxis) - aMin) * anInverseStep) > theBin && aRghIdx > theBeg)
      {
        --aRghIdx;
      }

      if (aLftIdx <= aRghIdx)
      {
        if (aLftIdx != aRghIdx)
        {
          theSet->Swap (aLftIdx, aRghIdx);
        }

        ++aLftIdx;
        --aRghIdx;
      }
    } while (aLftIdx <= aRghIdx);

    return aLftIdx;
  }

  template<class T, int N>
  struct BVH_AxisSelector
  {
    typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;
    static Standard_Integer MainAxis (const BVH_VecNt& theSize)
    {
      if (theSize.y() > theSize.x())
      {
        return theSize.y() > theSize.z() ? 1 : 2;
      }
      else
      {
        return theSize.z() > theSize.x() ? 2 : 0;
      }
    }
  };

  template<class T>
  struct BVH_AxisSelector<T, 2>
  {
    typedef typename BVH::VectorType<T, 2>::Type BVH_VecNt;
    static Standard_Integer MainAxis (const BVH_VecNt& theSize)
    {
      return theSize.x() > theSize.y() ? 0 : 1;
    }
  };
}

// =======================================================================
// function : buildNode
// purpose  :
// =======================================================================
template<class T, int N, int Bins>
typename BVH_QueueBuilder<T, N>::BVH_ChildNodes BVH_BinnedBuilder<T, N, Bins>::buildNode (BVH_Set<T, N>*         theSet,
                                                                                          BVH_Tree<T, N>*        theBVH,
                                                                                          const Standard_Integer theNode) const
{
  const Standard_Integer aNodeBegPrimitive = theBVH->BegPrimitive (theNode);
  const Standard_Integer aNodeEndPrimitive = theBVH->EndPrimitive (theNode);
  if (aNodeEndPrimitive - aNodeBegPrimitive < BVH_Builder<T, N>::myLeafNodeSize)
  {
    return typename BVH_QueueBuilder<T, N>::BVH_ChildNodes(); // node does not require partitioning
  }

  const BVH_Box<T, N> anAABB (theBVH->MinPoint (theNode),
                              theBVH->MaxPoint (theNode));
  const typename BVH_Box<T, N>::BVH_VecNt aSize = anAABB.Size();

  // Parameters for storing best split
  Standard_Integer aMinSplitAxis   = -1;
  Standard_Integer aMinSplitIndex  =  0;
  Standard_Integer aMinSplitNumLft =  0;
  Standard_Integer aMinSplitNumRgh =  0;

  BVH_Box<T, N> aMinSplitBoxLft;
  BVH_Box<T, N> aMinSplitBoxRgh;

  Standard_Real aMinSplitCost = std::numeric_limits<Standard_Real>::max();
  const Standard_Integer aMainAxis = BVH::BVH_AxisSelector<T, N>::MainAxis (aSize);

  // Find best split
  for (Standard_Integer anAxis = myUseMainAxis ? aMainAxis : 0; anAxis <= (myUseMainAxis ? aMainAxis : Min (N - 1, 2)); ++anAxis)
  {
    if (BVH::VecComp<T, N>::Get (aSize, anAxis) <= BVH::THE_NODE_MIN_SIZE)
    {
      continue;
    }

    BVH_BinVector aBinVector;
    getSubVolumes (theSet, theBVH, theNode, aBinVector, anAxis);

    BVH_SplitPlanes aSplitPlanes;
    for (Standard_Integer aLftSplit = 1, aRghSplit = Bins - 1; aLftSplit < Bins; ++aLftSplit, --aRghSplit)
    {
      aSplitPlanes[aLftSplit].LftVoxel.Count = aSplitPlanes[aLftSplit - 1].LftVoxel.Count + aBinVector[aLftSplit - 1].Count;
      aSplitPlanes[aRghSplit].RghVoxel.Count = aSplitPlanes[aRghSplit + 1].RghVoxel.Count + aBinVector[aRghSplit + 0].Count;

      aSplitPlanes[aLftSplit].LftVoxel.Box = aSplitPlanes[aLftSplit - 1].LftVoxel.Box;
      aSplitPlanes[aRghSplit].RghVoxel.Box = aSplitPlanes[aRghSplit + 1].RghVoxel.Box;

      aSplitPlanes[aLftSplit].LftVoxel.Box.Combine (aBinVector[aLftSplit - 1].Box);
      aSplitPlanes[aRghSplit].RghVoxel.Box.Combine (aBinVector[aRghSplit + 0].Box);
    }

    // Choose the best split (with minimum SAH cost)
    for (Standard_Integer aSplit = 1; aSplit < Bins; ++aSplit)
    {
      // Simple SAH evaluation
      Standard_Real aCost =
        (static_cast<Standard_Real> (aSplitPlanes[aSplit].LftVoxel.Box.Area()) /* / S(N) */) * aSplitPlanes[aSplit].LftVoxel.Count
      + (static_cast<Standard_Real> (aSplitPlanes[aSplit].RghVoxel.Box.Area()) /* / S(N) */) * aSplitPlanes[aSplit].RghVoxel.Count;

      if (aCost <= aMinSplitCost)
      {
        aMinSplitCost   = aCost;
        aMinSplitAxis   = anAxis;
        aMinSplitIndex  = aSplit;
        aMinSplitBoxLft = aSplitPlanes[aSplit].LftVoxel.Box;
        aMinSplitBoxRgh = aSplitPlanes[aSplit].RghVoxel.Box;
        aMinSplitNumLft = aSplitPlanes[aSplit].LftVoxel.Count;
        aMinSplitNumRgh = aSplitPlanes[aSplit].RghVoxel.Count;
      }
    }
  }

  theBVH->SetInner (theNode);
  Standard_Integer aMiddle = -1;
  if (aMinSplitNumLft == 0 || aMinSplitNumRgh == 0 || aMinSplitAxis == -1) // case of objects with the same center
  {
    aMinSplitBoxLft.Clear();
    aMinSplitBoxRgh.Clear();

    aMiddle = std::max (aNodeBegPrimitive + 1,
      static_cast<Standard_Integer> ((aNodeBegPrimitive + aNodeEndPrimitive) / 2.f));

    aMinSplitNumLft = aMiddle - aNodeBegPrimitive;
    for (Standard_Integer anIndex = aNodeBegPrimitive; anIndex < aMiddle; ++anIndex)
    {
      aMinSplitBoxLft.Combine (theSet->Box (anIndex));
    }

    aMinSplitNumRgh = aNodeEndPrimitive - aMiddle + 1;
    for (Standard_Integer anIndex = aNodeEndPrimitive; anIndex >= aMiddle; --anIndex)
    {
      aMinSplitBoxRgh.Combine (theSet->Box (anIndex));
    }
  }
  else
  {
    aMiddle = BVH::SplitPrimitives<T, N> (theSet,
                                          anAABB,
                                          aNodeBegPrimitive,
                                          aNodeEndPrimitive,
                                          aMinSplitIndex - 1,
                                          aMinSplitAxis,
                                          Bins);
  }

  typedef typename BVH_QueueBuilder<T, N>::BVH_PrimitiveRange Range;
  return typename BVH_QueueBuilder<T, N>::BVH_ChildNodes (aMinSplitBoxLft,
                                                          aMinSplitBoxRgh,
                                                          Range (aNodeBegPrimitive, aMiddle - 1),
                                                          Range (aMiddle,     aNodeEndPrimitive));
}

#endif // _BVH_BinnedBuilder_Header

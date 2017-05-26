// Created on: 2014-09-11
// Created by: Danila ULYANOV
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

#ifndef _BVH_LinearBuilder_Header
#define _BVH_LinearBuilder_Header

#include <BVH_RadixSorter.hxx>
#include <Standard_Assert.hxx>

//! Performs fast BVH construction using LBVH building approach.
//! Algorithm uses spatial Morton codes to reduce the BVH construction
//! problem to a sorting problem (radix sort -- O(N) complexity). This
//! Linear Bounding Volume Hierarchy (LBVH) builder produces BVH trees
//! of lower quality compared to SAH-based BVH builders but it is over
//! an order of magnitude faster (up to 3M triangles per second).
//! 
//! For more details see:
//! C. Lauterbach, M. Garland, S. Sengupta, D. Luebke, and D. Manocha.
//! Fast BVH construction on GPUs. Eurographics, 2009.
template<class T, int N>
class BVH_LinearBuilder : public BVH_Builder<T, N>
{
public:

  typedef typename BVH::VectorType<T, N>::Type BVH_VecNt;

public:

  //! Creates binned LBVH builder.
  BVH_LinearBuilder (const Standard_Integer theLeafNodeSize = BVH_Constants_LeafNodeSizeDefault,
                     const Standard_Integer theMaxTreeDepth = BVH_Constants_MaxTreeDepth);

  //! Releases resources of LBVH builder.
  virtual ~BVH_LinearBuilder();

  //! Builds BVH.
  virtual void Build (BVH_Set<T, N>*       theSet,
                      BVH_Tree<T, N>*      theBVH,
                      const BVH_Box<T, N>& theBox) const Standard_OVERRIDE;

protected:

  typedef NCollection_Array1<BVH_EncodedLink>::iterator LinkIterator;

protected:

  //! Emits hierarchy from sorted Morton codes.
  Standard_Integer emitHierachy (BVH_Tree<T, N>*        theBVH,
                                 const NCollection_Array1<BVH_EncodedLink>& theEncodedLinks,
                                 const Standard_Integer theBit,
                                 const Standard_Integer theShift,
                                 const Standard_Integer theStart,
                                 const Standard_Integer theFinal) const;

  //! Returns index of the first element which does not compare less than the given one.
  Standard_Integer lowerBound (const NCollection_Array1<BVH_EncodedLink>& theEncodedLinks,
                               Standard_Integer theStart,
                               Standard_Integer theFinal,
                               Standard_Integer theDigit) const;

};

// =======================================================================
// function : BVH_LinearBuilder
// purpose  :
// =======================================================================
template<class T, int N>
BVH_LinearBuilder<T, N>::BVH_LinearBuilder (const Standard_Integer theLeafNodeSize,
                                            const Standard_Integer theMaxTreeDepth)
: BVH_Builder<T, N> (theLeafNodeSize,
                     theMaxTreeDepth)
{
  //
}

// =======================================================================
// function : ~BVH_LinearBuilder
// purpose  :
// =======================================================================
template<class T, int N>
BVH_LinearBuilder<T, N>::~BVH_LinearBuilder()
{
  //
}

// =======================================================================
// function : lowerBound
// purpose  : Returns index of first element greater than the given one
// =======================================================================
template<class T, int N>
Standard_Integer BVH_LinearBuilder<T, N>::lowerBound (const NCollection_Array1<BVH_EncodedLink>& theEncodedLinks,
                                                      Standard_Integer theStart,
                                                      Standard_Integer theFinal,
                                                      Standard_Integer theDigit) const
{
  Standard_Integer aNbPrims = theFinal - theStart;
  while (aNbPrims > 0)
  {
    const Standard_Integer aStep = aNbPrims / 2;
    if (theEncodedLinks.Value (theStart + aStep).first & (1 << theDigit))
    {
      aNbPrims = aStep;
    }
    else
    {
      theStart += aStep + 1;
      aNbPrims -= aStep + 1;
    }
  }

  return theStart;
}

// =======================================================================
// function : emitHierachy
// purpose  : Emits hierarchy from sorted Morton codes
// =======================================================================
template<class T, int N>
Standard_Integer BVH_LinearBuilder<T, N>::emitHierachy (BVH_Tree<T, N>*        theBVH,
                                                        const NCollection_Array1<BVH_EncodedLink>& theEncodedLinks,
                                                        const Standard_Integer theBit,
                                                        const Standard_Integer theShift,
                                                        const Standard_Integer theStart,
                                                        const Standard_Integer theFinal) const
{
  if (theFinal - theStart > BVH_Builder<T, N>::myLeafNodeSize)
  {
    const Standard_Integer aPosition = theBit < 0 ?
      (theStart + theFinal) / 2 : lowerBound (theEncodedLinks, theStart, theFinal, theBit);
    if (aPosition == theStart || aPosition == theFinal)
    {
      return emitHierachy (theBVH, theEncodedLinks, theBit - 1, theShift, theStart, theFinal);
    }

    // Build inner node
    const Standard_Integer aNode = theBVH->AddInnerNode (0, 0);
    const Standard_Integer aRghNode = theShift + aPosition - theStart;

    const Standard_Integer aLftChild = emitHierachy (theBVH, theEncodedLinks, theBit - 1, theShift, theStart, aPosition);
    const Standard_Integer aRghChild = emitHierachy (theBVH, theEncodedLinks, theBit - 1, aRghNode, aPosition, theFinal);

    theBVH->NodeInfoBuffer()[aNode].y() = aLftChild;
    theBVH->NodeInfoBuffer()[aNode].z() = aRghChild;
    return aNode;
  }
  else
  {
    // Build leaf node
    return theBVH->AddLeafNode (theShift, theShift + theFinal - theStart - 1);
  }
}

namespace BVH
{
  //! Calculates bounding boxes (AABBs) for the given BVH tree.
  template<class T, int N>
  Standard_Integer UpdateBounds (BVH_Set<T, N>* theSet, BVH_Tree<T, N>* theTree, const Standard_Integer theNode = 0)
  {
    const BVH_Vec4i aData = theTree->NodeInfoBuffer()[theNode];
    if (aData.x() == 0)
    {
      const Standard_Integer aLftChild = theTree->NodeInfoBuffer()[theNode].y();
      const Standard_Integer aRghChild = theTree->NodeInfoBuffer()[theNode].z();

      const Standard_Integer aLftDepth = UpdateBounds (theSet, theTree, aLftChild);
      const Standard_Integer aRghDepth = UpdateBounds (theSet, theTree, aRghChild);

      typename BVH_Box<T, N>::BVH_VecNt aLftMinPoint = theTree->MinPointBuffer()[aLftChild];
      typename BVH_Box<T, N>::BVH_VecNt aLftMaxPoint = theTree->MaxPointBuffer()[aLftChild];
      typename BVH_Box<T, N>::BVH_VecNt aRghMinPoint = theTree->MinPointBuffer()[aRghChild];
      typename BVH_Box<T, N>::BVH_VecNt aRghMaxPoint = theTree->MaxPointBuffer()[aRghChild];

      BVH::BoxMinMax<T, N>::CwiseMin (aLftMinPoint, aRghMinPoint);
      BVH::BoxMinMax<T, N>::CwiseMax (aLftMaxPoint, aRghMaxPoint);

      theTree->MinPointBuffer()[theNode] = aLftMinPoint;
      theTree->MaxPointBuffer()[theNode] = aLftMaxPoint;
      return Max (aLftDepth, aRghDepth) + 1;
    }
    else
    {
      typename BVH_Box<T, N>::BVH_VecNt& aMinPoint = theTree->MinPointBuffer()[theNode];
      typename BVH_Box<T, N>::BVH_VecNt& aMaxPoint = theTree->MaxPointBuffer()[theNode];
      for (Standard_Integer aPrimIdx = aData.y(); aPrimIdx <= aData.z(); ++aPrimIdx)
      {
        const BVH_Box<T, N> aBox = theSet->Box (aPrimIdx);
        if (aPrimIdx == aData.y())
        {
          aMinPoint = aBox.CornerMin();
          aMaxPoint = aBox.CornerMax();
        }
        else
        {
          BVH::BoxMinMax<T, N>::CwiseMin (aMinPoint, aBox.CornerMin());
          BVH::BoxMinMax<T, N>::CwiseMax (aMaxPoint, aBox.CornerMax());
        }
      }
    }
    return 0;
  }

#ifdef HAVE_TBB

  //! TBB task for parallel bounds updating.
  template<class T, int N>
  class UpdateBoundTask: public tbb::task
  {

    BVH_Set<T, N>*    mySet;    //!< Set of geometric objects
    BVH_Tree<T, N>*   myBVH;    //!< BVH tree built over the set
    Standard_Integer  myNode;   //!< BVH node to update bounding box
    Standard_Integer  myLevel;  //!< Level of the processed BVH node
    Standard_Integer* myHeight; //!< Height of the processed BVH node

  public:

    //! Creates new TBB parallel bound update task.
    UpdateBoundTask (BVH_Set<T, N>*    theSet,
                     BVH_Tree<T, N>*   theBVH,
                     Standard_Integer  theNode,
                     Standard_Integer  theLevel,
                     Standard_Integer* theHeight)
    : mySet (theSet), myBVH (theBVH), myNode (theNode), myLevel (theLevel), myHeight (theHeight) {}

    //! Executes the task.
    tbb::task* execute()
    {
      if (myBVH->IsOuter (myNode) || myLevel > 2)
      {
        *myHeight = BVH::UpdateBounds (mySet, myBVH, myNode);
      }
      else
      {
        Standard_Integer aLftHeight = 0;
        Standard_Integer aRghHeight = 0;

        const Standard_Integer aLftChild = myBVH->NodeInfoBuffer()[myNode].y();
        const Standard_Integer aRghChild = myBVH->NodeInfoBuffer()[myNode].z();

        Standard_Integer aCount = 1;
        tbb::task_list aList;
        if (!myBVH->IsOuter (aLftChild))
        {
          ++aCount;
          aList.push_back (*new ( allocate_child() )
            UpdateBoundTask (mySet, myBVH, aLftChild, myLevel + 1, &aLftHeight));
        }
        else
        {
          aLftHeight = BVH::UpdateBounds (mySet, myBVH, aLftChild);
        }

        if (!myBVH->IsOuter (aRghChild))
        {
          ++aCount;
          aList.push_back (*new( allocate_child() )
            UpdateBoundTask (mySet, myBVH, aRghChild, myLevel + 1, &aRghHeight));
        }
        else
        {
          aRghHeight = BVH::UpdateBounds (mySet, myBVH, aRghChild);
        }

        if (aCount > 1)
        {
          set_ref_count (aCount);
          spawn_and_wait_for_all (aList);
        }

        typename BVH_Box<T, N>::BVH_VecNt aLftMinPoint = myBVH->MinPointBuffer()[aLftChild];
        typename BVH_Box<T, N>::BVH_VecNt aLftMaxPoint = myBVH->MaxPointBuffer()[aLftChild];
        typename BVH_Box<T, N>::BVH_VecNt aRghMinPoint = myBVH->MinPointBuffer()[aRghChild];
        typename BVH_Box<T, N>::BVH_VecNt aRghMaxPoint = myBVH->MaxPointBuffer()[aRghChild];

        BVH::BoxMinMax<T, N>::CwiseMin (aLftMinPoint, aRghMinPoint);
        BVH::BoxMinMax<T, N>::CwiseMax (aLftMaxPoint, aRghMaxPoint);

        myBVH->MinPointBuffer()[myNode] = aLftMinPoint;
        myBVH->MaxPointBuffer()[myNode] = aLftMaxPoint;

        *myHeight = Max (aLftHeight, aRghHeight) + 1;
      }
      return NULL;
    }
  };

#endif
}

// =======================================================================
// function : Build
// purpose  :
// =======================================================================
template<class T, int N>
void BVH_LinearBuilder<T, N>::Build (BVH_Set<T, N>*       theSet,
                                     BVH_Tree<T, N>*      theBVH,
                                     const BVH_Box<T, N>& theBox) const
{
  Standard_STATIC_ASSERT (N == 3 || N == 4);
  const Standard_Integer aSetSize = theSet->Size();
  if (theBVH == NULL || aSetSize == 0)
  {
    return;
  }

  theBVH->Clear();

  // Step 0 -- Initialize parameter of virtual grid
  BVH_RadixSorter<T, N> aRadixSorter (theBox);

  // Step 1 - Perform radix sorting of primitive set
  aRadixSorter.Perform (theSet);

  // Step 2 -- Emitting BVH hierarchy from sorted Morton codes
  emitHierachy (theBVH, aRadixSorter.EncodedLinks(), 29, 0, 0, theSet->Size());

  // Step 3 -- Compute bounding boxes of BVH nodes
  theBVH->MinPointBuffer().resize (theBVH->NodeInfoBuffer().size());
  theBVH->MaxPointBuffer().resize (theBVH->NodeInfoBuffer().size());

  Standard_Integer aHeight = 0;

#ifdef HAVE_TBB

  // Note: Although TBB tasks are allocated using placement
  // new, we do not need to delete them explicitly
  BVH::UpdateBoundTask<T, N>& aRootTask = *new ( tbb::task::allocate_root() )
    BVH::UpdateBoundTask<T, N> (theSet, theBVH, 0, 0, &aHeight);

  tbb::task::spawn_root_and_wait (aRootTask);

#else

  aHeight = BVH::UpdateBounds (theSet, theBVH, 0);

#endif

  BVH_Builder<T, N>::updateDepth (theBVH, aHeight);
}

#endif // _BVH_LinearBuilder_Header

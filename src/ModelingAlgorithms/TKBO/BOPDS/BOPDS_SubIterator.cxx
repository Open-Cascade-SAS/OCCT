// Created by: Peter KURNEV
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

#include <BOPDS_SubIterator.hxx>

#include <Bnd_Tools.hxx>

#include <BOPDS_DS.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_MapOfPair.hxx>

#include <BOPTools_BoxTree.hxx>

#include <algorithm>

//=================================================================================================

BOPDS_SubIterator::BOPDS_SubIterator()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      myList(1, myAllocator)
{
  myDS = NULL;
}

//=================================================================================================

BOPDS_SubIterator::BOPDS_SubIterator(const Handle(NCollection_BaseAllocator)& theAllocator)
    : myAllocator(theAllocator),
      myList(1, myAllocator)
{
  myDS = NULL;
}

//=================================================================================================

BOPDS_SubIterator::~BOPDS_SubIterator() {}

//=================================================================================================

void BOPDS_SubIterator::Initialize()
{
  // sort interfering pairs for constant order of intersection
  std::stable_sort(myList.begin(), myList.end());
  // initialize iterator to access the pairs
  myIterator.Init(myList);
}

//=================================================================================================

void BOPDS_SubIterator::Value(Standard_Integer& theI1, Standard_Integer& theI2) const
{
  Standard_Integer iT1, iT2, n1, n2;
  //
  const BOPDS_Pair& aPKB = myIterator.Value();
  aPKB.Indices(n1, n2);
  //
  iT1 = (Standard_Integer)(myDS->ShapeInfo(n1).ShapeType());
  iT2 = (Standard_Integer)(myDS->ShapeInfo(n2).ShapeType());
  //
  theI1 = n1;
  theI2 = n2;
  if (iT1 < iT2)
  {
    theI1 = n2;
    theI2 = n1;
  }
}

//=================================================================================================

void BOPDS_SubIterator::Prepare()
{
  myList.Clear();
  //
  if (!myDS)
  {
    return;
  }
  //
  if (!mySubSet1->Extent() || !mySubSet2->Extent())
  {
    return;
  }
  //
  myList.SetIncrement(2 * (mySubSet1->Extent() + mySubSet2->Extent()));
  //
  Intersect();
}

//=================================================================================================

void BOPDS_SubIterator::Intersect()
{
  if (!mySubSet1->Extent() || !mySubSet2->Extent())
    return;

  // Construct BVH tree for each sub-set
  BOPTools_BoxTree aBBTree[2];
  for (Standard_Integer i = 0; i < 2; ++i)
  {
    const TColStd_ListOfInteger* aSubSet = !i ? mySubSet1 : mySubSet2;
    aBBTree[i].SetSize(aSubSet->Extent());
    for (TColStd_ListOfInteger::Iterator it(*aSubSet); it.More(); it.Next())
    {
      const Standard_Integer nS     = it.Value();
      const BOPDS_ShapeInfo& aSI    = myDS->ShapeInfo(nS);
      const Bnd_Box&         aBoxEx = aSI.Box();
      aBBTree[i].Add(nS, Bnd_Tools::Bnd2BVH(aBoxEx));
    }
    aBBTree[i].Build();
  }

  // Perform selection of the interfering pairs
  BOPTools_BoxPairSelector aPairSelector;
  aPairSelector.SetBVHSets(&aBBTree[0], &aBBTree[1]);
  aPairSelector.Select();
  aPairSelector.Sort();

  // Treat the selected pairs
  const std::vector<BOPTools_BoxPairSelector::PairIDs>& aPairs = aPairSelector.Pairs();
  const Standard_Integer aNbPairs = static_cast<Standard_Integer>(aPairs.size());

  // Fence map
  BOPDS_MapOfPair aMPKFence;

  for (Standard_Integer iPair = 0; iPair < aNbPairs; ++iPair)
  {
    const BOPTools_BoxPairSelector::PairIDs& aPair = aPairs[iPair];
    if (aPair.ID1 == aPair.ID2)
      continue;

    BOPDS_Pair aDSPair(Min(aPair.ID1, aPair.ID2), Max(aPair.ID1, aPair.ID2));
    if (!aMPKFence.Add(aDSPair))
      continue;

    const BOPDS_ShapeInfo& aSI1 = myDS->ShapeInfo(aPair.ID1);
    const BOPDS_ShapeInfo& aSI2 = myDS->ShapeInfo(aPair.ID2);

    const TopAbs_ShapeEnum aType1 = aSI1.ShapeType();
    const TopAbs_ShapeEnum aType2 = aSI2.ShapeType();

    Standard_Integer iType1 = BOPDS_Tools::TypeToInteger(aType1);
    Standard_Integer iType2 = BOPDS_Tools::TypeToInteger(aType2);

    // avoid interfering of the shape with its sub-shapes
    if (((iType1 < iType2) && aSI1.HasSubShape(aPair.ID2))
        || ((iType1 > iType2) && aSI2.HasSubShape(aPair.ID1)))
      continue;

    myList.Append(aDSPair);
  }
}

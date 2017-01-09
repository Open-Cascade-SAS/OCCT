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

#include <Bnd_Box.hxx>

#include <BOPCol_BoxBndTree.hxx>

#include <BOPDS_DS.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_MapOfPair.hxx>

#include <NCollection_UBTreeFiller.hxx>

#include <TopoDS_Shape.hxx>

#include <algorithm>

//=======================================================================
//function : BOPDS_SubIterator
//purpose  : 
//=======================================================================
  BOPDS_SubIterator::BOPDS_SubIterator()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myList(1, myAllocator)
{
  myDS=NULL;
}
//=======================================================================
//function : BOPDS_SubIterator
//purpose  : 
//=======================================================================
  BOPDS_SubIterator::BOPDS_SubIterator(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myList(1, myAllocator)
{
  myDS=NULL;
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BOPDS_SubIterator::~BOPDS_SubIterator()
{
}
//=======================================================================
// function: Initialize
// purpose: 
//=======================================================================
void BOPDS_SubIterator::Initialize()
{
  // sort interfering pairs for constant order of intersection
  std::stable_sort(myList.begin(), myList.end());
  // initialize iterator to access the pairs
  myIterator.Init(myList);
}
//=======================================================================
// function: Value
// purpose: 
//=======================================================================
  void BOPDS_SubIterator::Value(Standard_Integer& theI1,
                                Standard_Integer& theI2) const
{
  Standard_Integer iT1, iT2, n1, n2;
  //
  const BOPDS_Pair& aPKB = myIterator.Value();
  aPKB.Indices(n1, n2);
  //
  iT1=(Standard_Integer)(myDS->ShapeInfo(n1).ShapeType());
  iT2=(Standard_Integer)(myDS->ShapeInfo(n2).ShapeType());
  //
  theI1=n1;
  theI2=n2;
  if (iT1<iT2) {
    theI1=n2;
    theI2=n1;
  }
}
//=======================================================================
// function: Prepare
// purpose: 
//=======================================================================
  void BOPDS_SubIterator::Prepare()
{
  myList.Clear();
  //
  if (!myDS){
    return;
  }
  //
  if (!mySubSet1->Extent() || !mySubSet2->Extent()) {
    return;
  }
  //
  myList.SetIncrement(2 * (mySubSet1->Extent() + mySubSet2->Extent()));
  //
  Intersect();
}
//=======================================================================
// function: Intersect
// purpose: 
//=======================================================================
  void BOPDS_SubIterator::Intersect()
{
  Standard_Integer i, j, iTi, iTj;
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  BOPCol_ListIteratorOfListOfInteger aIt(*mySubSet1);
  for (; aIt.More(); aIt.Next()) {
    i = aIt.Value();
    //
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    const Bnd_Box& aBoxEx = aSI.Box();
    //
    aTreeFiller.Add(i, aBoxEx);
  }
  //
  aTreeFiller.Fill();
  //
  BOPDS_MapOfPair aMPKFence;
  //
  aIt.Initialize(*mySubSet2);
  for (; aIt.More(); aIt.Next()) {
    i = aIt.Value();
    //
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    const Bnd_Box& aBoxEx = aSI.Box();
    //
    BOPCol_BoxBndTreeSelector aSelector;
    aSelector.SetBox(aBoxEx);
    Standard_Integer aNbSD = aBBTree.Select(aSelector);
    if (!aNbSD) {
      continue;
    }
    //
    iTi = BOPDS_Tools::TypeToInteger(aSI.ShapeType());
    //
    const BOPCol_ListOfInteger& aLI = aSelector.Indices();
    BOPCol_ListIteratorOfListOfInteger aItLI(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      j = aItLI.Value();
      //
      const BOPDS_ShapeInfo& aSJ = myDS->ShapeInfo(j);
      iTj = BOPDS_Tools::TypeToInteger(aSJ.ShapeType());
      //
      // avoid interfering of the same shapes and shape with its sub-shapes
      if ((i == j) || ((iTi < iTj) && aSI.HasSubShape(j)) ||
                      ((iTi > iTj) && aSJ.HasSubShape(i))) {
        continue;
      }
      //
      BOPDS_Pair aPair(j, i);
      if (aMPKFence.Add(aPair)) {
        myList.Append(aPair);
      }
    }
  }
}

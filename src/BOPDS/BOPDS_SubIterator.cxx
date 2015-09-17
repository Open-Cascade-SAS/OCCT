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


#include <Bnd_Box.hxx>
#include <BOPCol_BoxBndTree.hxx>
#include <BOPCol_DataMapOfIntegerInteger.hxx>
#include <BOPCol_DataMapOfIntegerMapOfInteger.hxx>
#include <BOPCol_IndexedDataMapOfShapeBox.hxx>
#include <BOPCol_MapOfInteger.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_MapOfPassKeyBoolean.hxx>
#include <BOPDS_PassKeyBoolean.hxx>
#include <BOPDS_SubIterator.hxx>
#include <BOPDS_Tools.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

//
//
//
//
//
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPDS_SubIterator::BOPDS_SubIterator()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myList(myAllocator)
{
  myDS=NULL; 
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPDS_SubIterator::BOPDS_SubIterator(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myList(myAllocator)
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
// function: SetDS
// purpose: 
//=======================================================================
  void BOPDS_SubIterator::SetDS(const BOPDS_PDS& aDS)
{
  myDS=aDS;
}
//=======================================================================
// function: DS
// purpose: 
//=======================================================================
  const BOPDS_DS&  BOPDS_SubIterator::DS()const
{
  return *myDS;
}
//=======================================================================
//function : SetSubSet1
//purpose  : 
//=======================================================================
  void BOPDS_SubIterator::SetSubSet1(const BOPCol_ListOfInteger& theLI)
{
  mySubSet1=(BOPCol_PListOfInteger)&theLI;
}
//=======================================================================
//function : SubSet1
//purpose  : 
//=======================================================================
  const BOPCol_ListOfInteger& BOPDS_SubIterator::SubSet1()const 
{
  return *mySubSet1;
}
//=======================================================================
//function : SetSubSet2
//purpose  : 
//=======================================================================
  void BOPDS_SubIterator::SetSubSet2(const BOPCol_ListOfInteger& theLI)
{
  mySubSet2=(BOPCol_PListOfInteger)&theLI;
}
//=======================================================================
//function : SubSet2
//purpose  : 
//=======================================================================
  const BOPCol_ListOfInteger& BOPDS_SubIterator::SubSet2()const 
{
  return *mySubSet2;
}
//=======================================================================
// function: Initialize
// purpose: 
//=======================================================================
  void BOPDS_SubIterator::Initialize()
{
  myIterator.Initialize(myList);
}
//=======================================================================
// function: More
// purpose: 
//=======================================================================
  Standard_Boolean BOPDS_SubIterator::More()const
{
  return myIterator.More();
}
//=======================================================================
// function: Next
// purpose: 
//=======================================================================
  void BOPDS_SubIterator::Next()
{
  myIterator.Next();
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
  const BOPDS_PassKeyBoolean& aPKB=myIterator.Value();
  aPKB.Ids(n1, n2);
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
  if (!mySubSet1->Extent() || !mySubSet2->Extent()) {
    return;
  }
  Intersect();
}
//=======================================================================
// function: Intersect
// purpose: 
//=======================================================================
  void BOPDS_SubIterator::Intersect()
{
  Standard_Integer i, aNbB, aNbSD, j, iDS, jB;
  Handle(NCollection_BaseAllocator) aAllocator;
  BOPCol_ListIteratorOfListOfInteger aIt, aIt1, aIt2;
  BOPDS_PassKeyBoolean aPKXB; 
  //
  //-----------------------------------------------------scope_1 f
  aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  //
  BOPCol_DataMapOfShapeInteger aMSI(100, aAllocator);
  BOPCol_DataMapOfIntegerInteger aMII(100, aAllocator);
  BOPDS_MapOfPassKeyBoolean aMPKXB(100, aAllocator);
  BOPCol_IndexedDataMapOfShapeBox aMSB(100, aAllocator);
  //
  BOPCol_BoxBndTreeSelector aSelector;
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  aIt1.Initialize(*mySubSet1);
  for (; aIt1.More(); aIt1.Next()) {
    i=aIt1.Value();
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    const TopoDS_Shape& aS=aSI.Shape();
    const Bnd_Box& aBoxEx=aSI.Box();
    aMSI.Bind(aS, i);
    aMSB.Add(aS, aBoxEx);
  }
  //
  aNbB=aMSB.Extent();
  for (i=1; i<=aNbB; ++i) {
    const TopoDS_Shape& aS=aMSB.FindKey(i);
    const Bnd_Box& aBoxEx=aMSB(i);
    //
    aTreeFiller.Add(i, aBoxEx);
    //
    iDS=aMSI.Find(aS);
    aMII.Bind(i, iDS);
  }
  //
  aTreeFiller.Fill();
  //
  aIt2.Initialize(*mySubSet2);
  for (; aIt2.More(); aIt2.Next()) {
    i=aIt2.Value();
    //
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    const Bnd_Box& aBoxEx=aSI.Box();
    //
    aSelector.Clear();
    aSelector.SetBox(aBoxEx);
    aNbSD=aBBTree.Select(aSelector);
    if (!aNbSD){
      continue;
    }
    //const Bnd_Box& aBoxi=myDS->ShapeInfo(i).Box();
    //
    const BOPCol_ListOfInteger& aLI=aSelector.Indices();
    aIt.Initialize(aLI);
    for (; aIt.More(); aIt.Next()) {
      jB=aIt.Value();  // box index in MII
      j=aMII.Find(jB); // DS index
      //
      aPKXB.SetIds(i, j);
      if (aMPKXB.Add(aPKXB)) {
        myList.Append(aPKXB);
      }// if (aMPKXB.Add(aPKXB)) {
    }// for (; aIt.More(); aIt.Next()) {
  }
  //
  aMSI.Clear();
  aMII.Clear();
  aMPKXB.Clear();
  aMSB.Clear();
  //
  //-----------------------------------------------------scope_1 t
}

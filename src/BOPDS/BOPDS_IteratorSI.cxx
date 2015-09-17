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
#include <BOPCol_DataMapOfIntegerListOfInteger.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPCol_IndexedDataMapOfShapeBox.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_IteratorSI.hxx>
#include <BOPDS_MapOfPassKeyBoolean.hxx>
#include <BOPDS_PassKeyBoolean.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_Tools.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

//
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPDS_IteratorSI::BOPDS_IteratorSI()
:
  BOPDS_Iterator()
{
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPDS_IteratorSI::BOPDS_IteratorSI
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPDS_Iterator(theAllocator)
{
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPDS_IteratorSI::~BOPDS_IteratorSI()
{
}
//=======================================================================
// function: UpdateByLevelOfCheck
// purpose: 
//=======================================================================
void BOPDS_IteratorSI::UpdateByLevelOfCheck(const Standard_Integer theLevel)
{
  Standard_Integer i, aNbInterfTypes;
  //
  aNbInterfTypes=BOPDS_DS::NbInterfTypes();
  for (i=theLevel+1; i<aNbInterfTypes; ++i) {
    myLists(i).Clear();
  }
}
//=======================================================================
// function: Intersect
// purpose: 
//=======================================================================
void BOPDS_IteratorSI::Intersect()
{
  Standard_Boolean bFlag;
  Standard_Integer aNbS, i, aNbB;
  Standard_Integer aNbSD, iX, j, iDS, jB;
  TopAbs_ShapeEnum aTi, aTj;
  Handle(NCollection_BaseAllocator) aAllocator;
  BOPCol_ListIteratorOfListOfInteger aIt;
  //
  //-----------------------------------------------------scope_1 f
  aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  //
  BOPCol_DataMapOfShapeInteger aMSI(100, aAllocator);
  BOPCol_DataMapOfIntegerInteger aMII(100, aAllocator);
  BOPDS_MapOfPassKeyBoolean aMPA(100, aAllocator);
  BOPDS_MapOfPassKeyBoolean aMPKXB(100, aAllocator);
  BOPCol_IndexedDataMapOfShapeBox aMSB(100, aAllocator);
  BOPDS_PassKeyBoolean aPKXB; 
  //
  BOPCol_BoxBndTreeSelector aSelector;
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  // myPairsAvoid, aMSI, aMSB
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    //
    if (!aSI.IsInterfering()) { 
      continue;
    }
    //
    const TopoDS_Shape& aSi=aSI.Shape();
    aTi=aSI.ShapeType();
    if (aTi!=TopAbs_VERTEX) {
      const BOPCol_ListOfInteger& aLA=aSI.SubShapes();
      aIt.Initialize(aLA);
      for (; aIt.More(); aIt.Next()) {
        iX=aIt.Value();
        aPKXB.Clear();
        aPKXB.SetIds(i, iX);
        aMPA.Add(aPKXB);
      }
    }
    //
    aPKXB.Clear();
    aPKXB.SetIds(i, i);
    aMPA.Add(aPKXB);
    //
    const Bnd_Box& aBoxEx=aSI.Box();
    //
    aMSI.Bind(aSi, i);
    aMSB.Add(aSi, aBoxEx);
  } // for (i=0; i<aNbS; ++i) {
  // 
  // aMII
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
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    aTi=aSI.ShapeType();
    if (!aSI.IsInterfering()){
      continue;
    }
    //
    const TopoDS_Shape& aSi=myDS->Shape(i);
    aTi=aSi.ShapeType();
    const Bnd_Box& aBoxEx=aMSB.FindFromKey(aSi);
    aSelector.Clear();
    aSelector.SetBox(aBoxEx);
    //
    aNbSD=aBBTree.Select(aSelector);
    if (!aNbSD){
      continue;
    }
    //
    const BOPCol_ListOfInteger& aLI=aSelector.Indices();
    //
    aIt.Initialize(aLI);
    for (; aIt.More(); aIt.Next()) {
      jB=aIt.Value();  // box index in MII
      j=aMII.Find(jB); // DS index
      //
      aPKXB.SetIds(i, j);
      if (aMPA.Contains(aPKXB)) {
        continue;
      }
      //
      if (aMPKXB.Add(aPKXB)) {
        bFlag=Standard_False;// Bounding boxes are intersected
        const Bnd_Box& aBoxi=myDS->ShapeInfo(i).Box();
        const Bnd_Box& aBoxj=myDS->ShapeInfo(j).Box();
        if (aBoxi.IsOut(aBoxj)) {
          bFlag=!bFlag; //Bounding boxes of Sub-shapes are intersected
        }
        aTj=myDS->ShapeInfo(j).ShapeType();//
        iX=BOPDS_Tools::TypeToInteger(aTi, aTj);
        aPKXB.SetFlag(bFlag);
        myLists(iX).Append(aPKXB);
      }// if (aMPKXB.Add(aPKXB)) {
    }// for (; aIt.More(); aIt.Next()) {
  }//for (i=1; i<=aNbS; ++i) {
  //
  aMSI.Clear();
  aMII.Clear();
  aMPA.Clear();
  aMPKXB.Clear();
  aMSB.Clear();
  //
  //-----------------------------------------------------scope_1 t
}

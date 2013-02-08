// Created by: Peter KURNEV
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <BOPDS_IteratorSI.ixx>

#include <TopAbs_ShapeEnum.hxx>
#include <gp_Pnt.hxx>
#include <Bnd_Box.hxx>
//
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

#include <NCollection_UBTreeFiller.hxx>
#include <BOPDS_BoxBndTree.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_PassKeyBoolean.hxx>
#include <BOPDS_MapOfPassKeyBoolean.hxx>
#include <BOPDS_DS.hxx>

#include <BOPCol_IndexedDataMapOfShapeBox.hxx>
#include <BOPDS_Tools.hxx>
#include <BOPCol_DataMapOfShapeInteger.hxx>
#include <BOPCol_DataMapOfIntegerInteger.hxx>
#include <BOPCol_DataMapOfIntegerListOfInteger.hxx>
#include <BOPCol_IndexedMapOfInteger.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPCol_IndexedMapOfInteger.hxx>

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
  BOPDS_IteratorSI::BOPDS_IteratorSI(const Handle(NCollection_BaseAllocator)& theAllocator)
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
// function: Intersect
// purpose: 
//=======================================================================
  void BOPDS_IteratorSI::Intersect()
{
  Standard_Boolean bFlag;
  Standard_Integer aNbS, i, aNbB;//, iFlag, aNbLV, aNbA
  Standard_Integer aNbSD, iX, j, iDS, jB, k;;
  TopAbs_ShapeEnum aTi, aTj;
  Handle(NCollection_IncAllocator) aAllocator;
  BOPCol_ListIteratorOfListOfInteger aIt;
  BOPCol_DataMapIteratorOfDataMapOfIntegerListOfInteger aItVSD;
  //
  //-----------------------------------------------------scope_1 f
  aAllocator=new NCollection_IncAllocator();
  //
  BOPCol_DataMapOfShapeInteger aMSI(100, aAllocator);
  BOPCol_DataMapOfIntegerInteger aMII(100, aAllocator);
  BOPDS_MapOfPassKeyBoolean aMPA(100, aAllocator);
  BOPDS_MapOfPassKeyBoolean aMPKXB(100, aAllocator);
  BOPCol_IndexedDataMapOfShapeBox aMSB(100, aAllocator);
  BOPDS_PassKeyBoolean aPKXB; 
  //
  BOPDS_BoxBndTreeSelector aSelector;
  BOPDS_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  // myPairsAvoid, aMSI, aMSB
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    //
    if (aSI.HasBRep()) {
      const TopoDS_Shape& aSi=aSI.Shape();
      aTi=aSI.ShapeType();
      if (aTi!=TopAbs_VERTEX) {
        //--
        const BOPCol_ListOfInteger& aLA=aSI.SubShapes();
        aIt.Initialize(aLA);
        for (; aIt.More(); aIt.Next()) {
          iX=aIt.Value();
          aPKXB.Clear();
          aPKXB.SetIds(i, iX);
          aMPA.Add(aPKXB);
        }
        //--t
      }
      else {
        aPKXB.Clear();
        aPKXB.SetIds(i, i);
        aMPA.Add(aPKXB);
      }
      //
      const Bnd_Box& aBoxEx=aSI.Box();
      //
      aMSI.Bind(aSi, i);
      aMSB.Add(aSi, aBoxEx);
    }
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
    if (!aSI.HasBRep()){
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
    k=0;
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
  aAllocator.Nullify();
  //-----------------------------------------------------scope_1 t
}


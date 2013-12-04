// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPDS_Iterator.ixx>
//
#include <Bnd_Box.hxx>
//
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_UBTreeFiller.hxx>
//
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Shape.hxx>
//
#include <BOPCol_IndexedDataMapOfShapeBox.hxx>
#include <BOPCol_DataMapOfIntegerInteger.hxx>
#include <BOPCol_DataMapOfIntegerMapOfInteger.hxx>
#include <BOPCol_MapOfInteger.hxx>
//
#include <BOPDS_BoxBndTree.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_PassKeyBoolean.hxx>
#include <BOPDS_MapOfPassKeyBoolean.hxx>
#include <BOPDS_Tools.hxx>
#include <BOPDS_MapOfPassKeyBoolean.hxx>


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPDS_Iterator::BOPDS_Iterator()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator())
{
  myDS=NULL; 
  myLength=0;
  //
  myLists.SetStartSize(6);
  myLists.Init();
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPDS_Iterator::BOPDS_Iterator(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myLists(theAllocator)
{
  myDS=NULL; 
  myLength=0;
  //
  myLists.SetStartSize(6);
  myLists.Init();
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BOPDS_Iterator::~BOPDS_Iterator()
{
}
//=======================================================================
// function: SetDS
// purpose: 
//=======================================================================
  void BOPDS_Iterator::SetDS(const BOPDS_PDS& aDS)
{
  myDS=aDS;
}
//=======================================================================
// function: DS
// purpose: 
//=======================================================================
  const BOPDS_DS&  BOPDS_Iterator::DS()const
{
  return *myDS;
}
//=======================================================================
// function: ExpectedLength
// purpose: 
//=======================================================================
  Standard_Integer BOPDS_Iterator::ExpectedLength() const
{
  return myLength;
}
//=======================================================================
// function: BlockLength
// purpose: 
//=======================================================================
  Standard_Integer BOPDS_Iterator::BlockLength() const
{
  Standard_Integer aNbIIs;
  Standard_Real aCfPredict=.5;
  
  aNbIIs=ExpectedLength();
  
  if (aNbIIs<=1) {
    return 1;
  }
  //
  aNbIIs=(Standard_Integer) (aCfPredict*(Standard_Real)aNbIIs);
  return aNbIIs;
}
//=======================================================================
// function: Initialize
// purpose: 
//=======================================================================
  void BOPDS_Iterator::Initialize(const TopAbs_ShapeEnum aType1,
				  const TopAbs_ShapeEnum aType2)
{
  Standard_Integer iX;
  //
  myLength=0;
  iX=BOPDS_Tools::TypeToInteger(aType1, aType2);
  if (iX>=0) {
    myIterator.Initialize(myLists(iX));
    myLength=myLists(iX).Extent();
  }
}
//=======================================================================
// function: More
// purpose: 
//=======================================================================
  Standard_Boolean BOPDS_Iterator::More()const
{
  return myIterator.More();
}
//=======================================================================
// function: Next
// purpose: 
//=======================================================================
  void BOPDS_Iterator::Next()
{
  myIterator.Next();
}
//=======================================================================
// function: Value
// purpose: 
//=======================================================================
  void BOPDS_Iterator::Value(Standard_Integer& theI1,
			     Standard_Integer& theI2,
			     Standard_Boolean& theWithSubShape) const
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
  //
  theWithSubShape=aPKB.Flag();
}

//=======================================================================
// function: Prepare
// purpose: 
//=======================================================================
  void BOPDS_Iterator::Prepare()
{
  Standard_Integer i;
  //
  myLength=0;
  for (i=0; i<6; ++i) {
    myLists(i).Clear();
  }
  //
  if (myDS==NULL){
    return;
  }
  Intersect();
}
//=======================================================================
// function: Intersect
// purpose: 
//=======================================================================
  void BOPDS_Iterator::Intersect()
{
  Standard_Boolean bFlag;
  Standard_Integer aNb, i, aNbB, aNbR, iTi, iTj;
  Standard_Integer i1, i2, aNbSD, iX, j, iDS, jB, iR;
  TopAbs_ShapeEnum aTi, aTj;
  Handle(NCollection_IncAllocator) aAllocator;
  //
  BOPCol_ListIteratorOfListOfInteger aIt;
  //
  //-----------------------------------------------------scope_1 f
  aAllocator=new NCollection_IncAllocator();
  //
  BOPCol_DataMapOfShapeInteger aMSI(100, aAllocator);
  BOPCol_DataMapOfIntegerInteger aMII(100, aAllocator);
  BOPDS_MapOfPassKeyBoolean aMPKXB(100, aAllocator);
  BOPCol_IndexedDataMapOfShapeBox aMSB(100, aAllocator);
  BOPDS_PassKeyBoolean aPKXB; 
  //
  BOPDS_BoxBndTreeSelector aSelector;
  BOPDS_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  aNb=myDS->NbSourceShapes();
  for (i=0; i<aNb; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (aSI.HasBRep()) {
      const TopoDS_Shape& aS=aSI.Shape();
      const Bnd_Box& aBoxEx=aSI.Box();
      aMSI.Bind(aS, i);
      aMSB.Add(aS, aBoxEx);
    }
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
  aNbR=myDS->NbRanges()-1;
  for (iR=0; iR<aNbR; ++iR) {
    const BOPDS_IndexRange& aR=myDS->Range(iR);
    i1=aR.First();
    i2=aR.Last();
    for (i=i1; i<=i2; ++i) {
      const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
      //
      if (!aSI.HasBRep()){
	continue;
      }
      //
      aTi=aSI.ShapeType();
      const TopoDS_Shape& aSi=aSI.Shape();
      const Bnd_Box& aBoxEx=aMSB.FindFromKey(aSi);
      aSelector.Clear();
      aSelector.SetBox(aBoxEx);
      //
      aNbSD=aBBTree.Select(aSelector);
      if (!aNbSD){
	continue;
      }
      //
      const Bnd_Box& aBoxi=myDS->ShapeInfo(i).Box();
      //
      const BOPCol_ListOfInteger& aLI=aSelector.Indices();
      aIt.Initialize(aLI);
      for (; aIt.More(); aIt.Next()) {
	jB=aIt.Value();  // box index in MII
	j=aMII.Find(jB); // DS index
	if (j>=i1 && j<=i2) {
	  continue;// same range
	}
	//
	const BOPDS_ShapeInfo& aSIj=myDS->ShapeInfo(j);
	aTj=aSIj.ShapeType();
	iTi=BOPDS_Tools::TypeToInteger(aTi);
	iTj=BOPDS_Tools::TypeToInteger(aTj);
	bFlag=Standard_False;
	if (iTi<iTj) {
	  bFlag=aSI.HasSubShape(j);
	} 
	else if (iTj<iTi) {
	   bFlag=aSIj.HasSubShape(i);
	}
	if (bFlag) {
	  continue; 
	}
	//
	aPKXB.SetIds(i, j);
	if (aMPKXB.Add(aPKXB)) {
	  bFlag=Standard_False;// Bounding boxes are intersected
	  const Bnd_Box& aBoxj=myDS->ShapeInfo(j).Box();
	  if (aBoxi.IsOut(aBoxj)) {
	    bFlag=!bFlag; //Bounding boxes of Sub-shapes are intersected
	  }
	  //
	  iX=BOPDS_Tools::TypeToInteger(aTi, aTj);
	  aPKXB.SetFlag(bFlag);
	  myLists(iX).Append(aPKXB);
	}// if (aMPKXB.Add(aPKXB)) {
      }// for (; aIt.More(); aIt.Next()) {
    }//for (i=i1; i<=i2; ++i) {
  }//for (iR=1; iR<aNbR; ++iR) {
  //
  aMSI.Clear();
  aMII.Clear();
  aMPKXB.Clear();
  aMSB.Clear();
  //
  aAllocator.Nullify();
  //-----------------------------------------------------scope_1 t
}

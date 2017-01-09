// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include <BOPCol_NCVector.hxx>
#include <BOPCol_Parallel.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Tools.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <TopoDS_Shape.hxx>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPDS_TreeSelector
//purpose  : 
//=======================================================================
class BOPDS_TSR : public BOPCol_BoxBndTreeSelector{
 public:
  BOPDS_TSR() : 
    BOPCol_BoxBndTreeSelector(), 
    myHasBRep(Standard_False), 
    myTree(NULL) {
  }
  //
  virtual ~BOPDS_TSR() {
  }
  //
  void SetHasBRep(const Standard_Boolean bFlag) {
    myHasBRep=bFlag;
  }
  //
  void SetTree(BOPCol_BoxBndTree& aTree) {
    myTree=&aTree;
  }
  //
  void Perform() {
    if (myHasBRep) {
      myTree->Select(*this);
    }
  }
  //
 protected:
  Standard_Boolean myHasBRep;
  BOPCol_BoxBndTree *myTree;
};
//
//=======================================================================
typedef BOPCol_NCVector <BOPDS_TSR> BOPDS_VectorOfTSR; 
typedef BOPCol_Functor <BOPDS_TSR,BOPDS_VectorOfTSR> BOPDS_TSRFunctor;
typedef BOPCol_Cnt <BOPDS_TSRFunctor, BOPDS_VectorOfTSR> BOPDS_TSRCnt;
/////////////////////////////////////////////////////////////////////////


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPDS_Iterator::BOPDS_Iterator()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myRunParallel(Standard_False)
{
  Standard_Integer i, aNb;
  //
  myDS=NULL; 
  myLength=0;
  //
  aNb=BOPDS_DS::NbInterfTypes();
  myLists.SetIncrement(aNb);
  for (i=0; i<aNb; ++i) {
    myLists.Append1();
  }
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPDS_Iterator::BOPDS_Iterator
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myLists(0, theAllocator),
  myRunParallel(Standard_False)
{
  Standard_Integer i, aNb;
  //
  myDS=NULL; 
  myLength=0;
  //
  aNb=BOPDS_DS::NbInterfTypes();
  myLists.SetIncrement(aNb);
  for (i=0; i<aNb; ++i) {
    myLists.Append1();
  }
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPDS_Iterator::~BOPDS_Iterator()
{
}
//=======================================================================
//function : SetRunParallel
//purpose  : 
//=======================================================================
void BOPDS_Iterator::SetRunParallel(const Standard_Boolean theFlag)
{
  myRunParallel=theFlag;
}
//=======================================================================
//function : RunParallel
//purpose  : 
//=======================================================================
Standard_Boolean BOPDS_Iterator::RunParallel()const
{
  return myRunParallel;
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
    // sort interfering pairs for constant order of intersection
    std::stable_sort(myLists(iX).begin(), myLists(iX).end());
    // initialize iterator to access the pairs
    myIterator.Init(myLists(iX));
    myLength = myLists(iX).Extent();
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
                           Standard_Integer& theI2) const
{
  Standard_Integer iT1, iT2, n1, n2;
  //
  const BOPDS_Pair& aPair = myIterator.Value();
  aPair.Indices(n1, n2);
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
void BOPDS_Iterator::Prepare()
{
  Standard_Integer i, aNbInterfTypes;
  //
  aNbInterfTypes=BOPDS_DS::NbInterfTypes();
  myLength=0;
  for (i=0; i<aNbInterfTypes; ++i) {
    myLists(i).Clear();
  }
  //
  if (myDS==NULL){
    return;
  }
  Intersect();
}
//
//=======================================================================
// function: Intersect
// purpose: 
//=======================================================================
void BOPDS_Iterator::Intersect()
{
  Standard_Integer i, j, iX, i1, i2, iR, aNb, aNbR;
  Standard_Integer iTi, iTj;
  TopAbs_ShapeEnum aTi, aTj;
  //
  BOPCol_BoxBndTree aBBTree;
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(aBBTree);
  //
  aNb = myDS->NbSourceShapes();
  BOPDS_VectorOfTSR aVTSR(aNb);
  //
  for (i=0; i<aNb; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    Standard_Boolean bHasBrep = aSI.IsInterfering() && !(aSI.ShapeType() == TopAbs_SOLID);
    //
    BOPDS_TSR& aTSR=aVTSR.Append1();
    //
    aTSR.SetHasBRep(bHasBrep);
    if (!bHasBrep) {
      continue;
    }
    //
    const Bnd_Box& aBoxEx=aSI.Box();
    aTSR.SetTree(aBBTree);
    aTSR.SetBox(aBoxEx);
    //
    aTreeFiller.Add(i, aBoxEx);
  }
  //
  aTreeFiller.Fill();
  //
  //===========================================
  BOPDS_TSRCnt::Perform(myRunParallel, aVTSR);
  //===========================================
  //
  BOPDS_MapOfPair aMPFence;
  //
  aNbR = myDS->NbRanges() - 1;
  for (iR = 0; iR < aNbR; ++iR) {
    const BOPDS_IndexRange& aR = myDS->Range(iR);
    i1 = aR.First();
    i2 = aR.Last();
    for (i = i1; i <= i2; ++i) {
      const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
      //
      if (!aSI.IsInterfering() || (aSI.ShapeType() == TopAbs_SOLID)) {
        continue;
      }
      //
      BOPDS_TSR& aTSRi = aVTSR(i);
      const BOPCol_ListOfInteger& aLI = aTSRi.Indices();
      Standard_Integer aNbSD = aLI.Extent();
      if (!aNbSD) {
        continue;
      }
      //
      aTi = aSI.ShapeType();
      iTi = BOPDS_Tools::TypeToInteger(aTi);
      //
      BOPCol_ListIteratorOfListOfInteger aIt(aLI);
      for (; aIt.More(); aIt.Next()) {
        j = aIt.Value(); // DS index
        if (j >= i1 && j <= i2) {
          continue;// same range
        }
        //
        const BOPDS_ShapeInfo& aSJ = myDS->ShapeInfo(j);
        aTj = aSJ.ShapeType();
        iTj = BOPDS_Tools::TypeToInteger(aTj);
        //
        // avoid interfering of the same shapes and shape with its sub-shapes
        if (((iTi < iTj) && aSI.HasSubShape(j)) ||
            ((iTi > iTj) && aSJ.HasSubShape(i))) {
          continue;
        }
        //
        BOPDS_Pair aPair(i, j);
        if (aMPFence.Add(aPair)) {
          iX = BOPDS_Tools::TypeToInteger(aTi, aTj);
          myLists(iX).Append(aPair);
        }// if (aMPFence.Add(aPair)) {
      }// for (; aIt.More(); aIt.Next()) {
    }//for (i=i1; i<=i2; ++i) {
  }//for (iR=1; iR<aNbR; ++iR) {
  //
  aMPFence.Clear();
  aVTSR.Clear();
  //-----------------------------------------------------scope_1 t
}

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
#include <Bnd_OBB.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Tools.hxx>
#include <BOPTools_BoxBndTree.hxx>
#include <BOPTools_Parallel.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Vector.hxx>
#include <TopoDS_Shape.hxx>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPDS_TreeSelector
//purpose  : 
//=======================================================================
class BOPDS_TSR : public BOPTools_BoxBndTreeSelector{
 public:
  BOPDS_TSR() : 
    BOPTools_BoxBndTreeSelector(), 
    myHasBRep(Standard_False), 
    myTree(NULL),
    myIndex(-1) {}
  //
  virtual ~BOPDS_TSR() {
  }
  //
  void SetHasBRep(const Standard_Boolean bFlag) {
    myHasBRep=bFlag;
  }
  //
  void SetTree(BOPTools_BoxBndTree& aTree) {
    myTree=&aTree;
  }
  //
  void SetIndex(const Standard_Integer theIndex) { myIndex = theIndex; }
  //
  Standard_Integer Index() const { return myIndex; }
  //
  void Perform() {
    if (myHasBRep) {
      myTree->Select(*this);
    }
  }
  //
 protected:
  Standard_Boolean myHasBRep;
  BOPTools_BoxBndTree *myTree;
  Standard_Integer myIndex;
};
//
//=======================================================================
typedef NCollection_Vector<BOPDS_TSR> BOPDS_VectorOfTSR;
/////////////////////////////////////////////////////////////////////////


//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPDS_Iterator::BOPDS_Iterator()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myRunParallel(Standard_False),
  myUseExt(Standard_False)
{
  Standard_Integer i, aNb;
  //
  myDS=NULL;
  myLength=0;
  //
  aNb=BOPDS_DS::NbInterfTypes();
  myLists.SetIncrement(aNb);
  for (i=0; i<aNb; ++i) {
    myLists.Appended();
  }

  const Standard_Integer aNbExt = BOPDS_Iterator::NbExtInterfs();
  myExtLists.SetIncrement(aNbExt);
  for (i = 0; i < aNbExt; ++i) {
    myExtLists.Appended();
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
  myRunParallel(Standard_False),
  myExtLists(0, theAllocator),
  myUseExt(Standard_False)
{
  Standard_Integer i, aNb;
  //
  myDS=NULL; 
  myLength=0;
  //
  aNb=BOPDS_DS::NbInterfTypes();
  myLists.SetIncrement(aNb);
  for (i=0; i<aNb; ++i) {
    myLists.Appended();
  }

  const Standard_Integer aNbExt = BOPDS_Iterator::NbExtInterfs();
  myExtLists.SetIncrement(aNbExt);
  for (i = 0; i < aNbExt; ++i) {
    myExtLists.Appended();
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
  if (iX >= 0)
  {
    BOPDS_VectorOfPair& aPairs = (myUseExt && iX < BOPDS_Iterator::NbExtInterfs()) ?
      myExtLists(iX) : myLists(iX);
    // sort interfering pairs for constant order of intersection
    std::stable_sort(aPairs.begin(), aPairs.end());
    // initialize iterator to access the pairs
    myIterator.Init(aPairs);
    myLength = aPairs.Length();
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
void BOPDS_Iterator::Prepare(const Handle(IntTools_Context)& theCtx,
                             const Standard_Boolean theCheckOBB,
                             const Standard_Real theFuzzyValue)
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
  Intersect(theCtx, theCheckOBB, theFuzzyValue);
}
//
//=======================================================================
// function: Intersect
// purpose: 
//=======================================================================
void BOPDS_Iterator::Intersect(const Handle(IntTools_Context)& theCtx,
                               const Standard_Boolean theCheckOBB,
                               const Standard_Real theFuzzyValue)
{
  Standard_Integer i, j, iX, i1, i2, iR, aNb, aNbR;
  Standard_Integer iTi, iTj;
  TopAbs_ShapeEnum aTi, aTj;
  //
  myBoxTree.Clear();
  NCollection_UBTreeFiller <Standard_Integer, Bnd_Box> aTreeFiller(myBoxTree);
  //
  aNb = myDS->NbSourceShapes();
  BOPDS_VectorOfTSR aVTSR(aNb);
  //
  for (i=0; i<aNb; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    Standard_Boolean bHasBrep = aSI.IsInterfering() && !(aSI.ShapeType() == TopAbs_SOLID);
    //
    BOPDS_TSR& aTSR=aVTSR.Appended();
    //
    aTSR.SetHasBRep(bHasBrep);
    if (!bHasBrep) {
      continue;
    }
    //
    const Bnd_Box& aBoxEx=aSI.Box();
    aTSR.SetTree(myBoxTree);
    aTSR.SetBox(aBoxEx);
    aTSR.SetIndex(i);
    //
    aTreeFiller.Add(i, aBoxEx);
  }
  //
  aTreeFiller.Fill();
  //
  //===========================================
  BOPTools_Parallel::Perform (myRunParallel, aVTSR);
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
      const TColStd_ListOfInteger& aLI = aTSRi.Indices();
      Standard_Integer aNbSD = aLI.Extent();
      if (!aNbSD) {
        continue;
      }
      //
      aTi = aSI.ShapeType();
      iTi = BOPDS_Tools::TypeToInteger(aTi);
      //
      TColStd_ListIteratorOfListOfInteger aIt(aLI);
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
          if (theCheckOBB)
          {
            // Check intersection of Oriented bounding boxes of the shapes
            Bnd_OBB& anOBBi = theCtx->OBB(aSI.Shape(), theFuzzyValue);
            Bnd_OBB& anOBBj = theCtx->OBB(aSJ.Shape(), theFuzzyValue);

            if (anOBBi.IsOut(anOBBj))
              continue;
          }

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

//=======================================================================
// function: PrepareExt
// purpose: 
//=======================================================================
void BOPDS_Iterator::PrepareExt(const TColStd_MapOfInteger& theIndices)
{
  if (!myDS)
    return;

  // Update UB tree of bounding boxes with the increased shapes.
  // It is expected that not too many shapes will be modified during
  // the intersection, so after updating the tree it should not become
  // too unbalanced.
  TColStd_MapIteratorOfMapOfInteger itM(theIndices);
  for (; itM.More(); itM.Next())
  {
    Standard_Integer nV = itM.Value();
    myBoxTree.Remove(nV);

    // Add with new box
    Standard_Integer nVSD = nV;
    myDS->HasShapeSD(nV, nVSD);
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nVSD);
    const Bnd_Box& aBox = aSI.Box();
    myBoxTree.Add(nV, aBox);
  }

  // Clear the extra lists
  const Standard_Integer aNbExt = BOPDS_Iterator::NbExtInterfs();
  myLength = 0;
  for (Standard_Integer i = 0; i < aNbExt; ++i)
    myExtLists(i).Clear();

  IntersectExt(theIndices);

  myUseExt = Standard_True;
}

//=======================================================================
// function: IntersectExt
// purpose: 
//=======================================================================
void BOPDS_Iterator::IntersectExt(const TColStd_MapOfInteger& theIndices)
{
  // Prepare vector for parallel selection
  BOPDS_VectorOfTSR aVTSR(theIndices.Extent());

  TColStd_MapIteratorOfMapOfInteger itM(theIndices);
  for (; itM.More(); itM.Next())
  {
    Standard_Integer nV = itM.Value();
    Standard_Integer nVSD = nV;
    myDS->HasShapeSD(nV, nVSD);
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(nVSD);
    const Bnd_Box& aBox = aSI.Box();
    BOPDS_TSR& aTSR = aVTSR.Appended();
    aTSR.SetHasBRep(Standard_True);
    aTSR.SetTree(myBoxTree);
    aTSR.SetBox(aBox);
    aTSR.SetIndex(nV);
  }

  // Perform selection
  BOPTools_Parallel::Perform (myRunParallel, aVTSR);

  // Treat selections

  // Fence map to avoid duplicating pairs
  BOPDS_MapOfPair aMPFence;

  const Standard_Integer aNb = aVTSR.Length();
  for (Standard_Integer k = 0; k < aNb; ++k)
  {
    BOPDS_TSR& aTSRi = aVTSR(k);
    const TColStd_ListOfInteger& aLI = aTSRi.Indices();
    if (aLI.IsEmpty())
      continue;

    const Standard_Integer i = aTSRi.Index();
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    const Standard_Integer iRankI = myDS->Rank(i);
    const TopAbs_ShapeEnum aTI = aSI.ShapeType();
    const Standard_Integer iTI = BOPDS_Tools::TypeToInteger(aTI);

    TColStd_ListIteratorOfListOfInteger itLI(aLI);
    for (; itLI.More(); itLI.Next())
    {
      const Standard_Integer j = itLI.Value(); // Index in DS
      const Standard_Integer iRankJ = myDS->Rank(j);
      if (iRankI == iRankJ)
        continue;

      const BOPDS_ShapeInfo& aSJ = myDS->ShapeInfo(j);
      const TopAbs_ShapeEnum aTJ = aSJ.ShapeType();
      const Standard_Integer iTJ = BOPDS_Tools::TypeToInteger(aTJ);

      // avoid interfering of the same shapes and shape with its sub-shapes
      if (((iTI < iTJ) && aSI.HasSubShape(j)) ||
          ((iTI > iTJ) && aSJ.HasSubShape(i)))
        continue;

      BOPDS_Pair aPair(i, j);
      if (aMPFence.Add(aPair))
      {
        const Standard_Integer iX = BOPDS_Tools::TypeToInteger(aTI, aTJ);
        if (iX < BOPDS_Iterator::NbExtInterfs())
          myExtLists(iX).Append(aPair);
      }
    }
  }
}

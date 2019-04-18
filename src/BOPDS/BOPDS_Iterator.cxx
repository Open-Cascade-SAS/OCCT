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
#include <Bnd_Tools.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_Pair.hxx>
#include <BOPDS_MapOfPair.hxx>
#include <BOPDS_Tools.hxx>
#include <BOPTools_BoxTree.hxx>
#include <BOPTools_Parallel.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Vector.hxx>
#include <TopoDS_Shape.hxx>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////
//=======================================================================
//class    : BOPDS_TreeSelector
//purpose  : 
//=======================================================================
class BOPDS_TSR : public BOPTools_BoxTreeSelector
{
 public:
  BOPDS_TSR() : 
    BOPTools_BoxTreeSelector(), 
    myHasBRep(Standard_False), 
    myIndex(-1) {}
  //
  virtual ~BOPDS_TSR() {
  }
  //
  void SetHasBRep(const Standard_Boolean bFlag) {
    myHasBRep=bFlag;
  }
  //
  void SetIndex(const Standard_Integer theIndex) { myIndex = theIndex; }
  //
  Standard_Integer Index() const { return myIndex; }
  //
  void Perform() {
    if (myHasBRep) {
      Select();
    }
  }
  //
 protected:
  Standard_Boolean myHasBRep;
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
  const Standard_Integer aNb = myDS->NbSourceShapes();

  // Prepare BVH
  BOPTools_BoxTree aBoxTree;
  aBoxTree.SetSize (aNb);
  for (Standard_Integer i = 0; i < aNb; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (!aSI.HasBRep())
      continue;
    const Bnd_Box& aBox = aSI.Box();
    aBoxTree.Add (i, Bnd_Tools::Bnd2BVH (aBox));
  }

  // Build BVH
  aBoxTree.Build();

  // Select pairs of shapes with interfering bounding boxes
  BOPTools_BoxPairSelector aPairSelector;
  aPairSelector.SetBVHSets (&aBoxTree, &aBoxTree);
  aPairSelector.SetSame (Standard_True);
  aPairSelector.Select();
  aPairSelector.Sort();

  // Treat the selected pairs
  const std::vector<BOPTools_BoxPairSelector::PairIDs>& aPairs = aPairSelector.Pairs();
  const Standard_Integer aNbPairs = static_cast<Standard_Integer> (aPairs.size());

  Standard_Integer iPair = 0;

  const Standard_Integer aNbR = myDS->NbRanges();
  for (Standard_Integer iR = 0; iR < aNbR; ++iR)
  {
    const BOPDS_IndexRange& aRange = myDS->Range(iR);

    for (; iPair < aNbPairs; ++iPair)
    {
      const BOPTools_BoxPairSelector::PairIDs& aPair = aPairs[iPair];
      if (!aRange.Contains (aPair.ID1))
        // Go to the next range
        break;

      if (aRange.Contains (aPair.ID2))
        // Go to the next pair
        continue;

      const BOPDS_ShapeInfo& aSI1 = myDS->ShapeInfo (aPair.ID1);
      const BOPDS_ShapeInfo& aSI2 = myDS->ShapeInfo (aPair.ID2);

      const TopAbs_ShapeEnum aType1 = aSI1.ShapeType();
      const TopAbs_ShapeEnum aType2 = aSI2.ShapeType();

      Standard_Integer iType1 = BOPDS_Tools::TypeToInteger (aType1);
      Standard_Integer iType2 = BOPDS_Tools::TypeToInteger (aType2);

      // avoid interfering of the shape with its sub-shapes
      if (((iType1 < iType2) && aSI1.HasSubShape (aPair.ID2)) ||
          ((iType1 > iType2) && aSI2.HasSubShape (aPair.ID1)))
        continue;

      if (theCheckOBB)
      {
        // Check intersection of Oriented bounding boxes of the shapes
        const Bnd_OBB& anOBB1 = theCtx->OBB (aSI1.Shape(), theFuzzyValue);
        const Bnd_OBB& anOBB2 = theCtx->OBB (aSI2.Shape(), theFuzzyValue);

        if (anOBB1.IsOut (anOBB2))
          continue;
      }

      Standard_Integer iX = BOPDS_Tools::TypeToInteger (aType1, aType2);
      myLists(iX).Append (BOPDS_Pair (Min (aPair.ID1, aPair.ID2),
                                      Max (aPair.ID1, aPair.ID2)));
    }
  }
}

//=======================================================================
// function: IntersectExt
// purpose: 
//=======================================================================
void BOPDS_Iterator::IntersectExt(const TColStd_MapOfInteger& theIndices)
{
  if (!myDS)
    return;

  const Standard_Integer aNb = myDS->NbSourceShapes();

  BOPTools_BoxTree aBoxTree;
  aBoxTree.SetSize (aNb);
  BOPDS_VectorOfTSR aVTSR(theIndices.Extent());
  //
  for (Standard_Integer i = 0; i < aNb; ++i)
  {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (!aSI.IsInterfering() || (aSI.ShapeType() == TopAbs_SOLID))
      continue;
    //
    if (theIndices.Contains (i))
    {
      Standard_Integer nVSD = i;
      myDS->HasShapeSD (i, nVSD);
      const BOPDS_ShapeInfo& aSISD = myDS->ShapeInfo (nVSD);
      const Bnd_Box& aBox = aSISD.Box ();
      aBoxTree.Add (i, Bnd_Tools::Bnd2BVH(aBox));

      BOPDS_TSR& aTSR=aVTSR.Appended();
      aTSR.SetHasBRep(Standard_True);
      aTSR.SetBVHSet (&aBoxTree);
      aTSR.SetBox (Bnd_Tools::Bnd2BVH (aBox));
      aTSR.SetIndex (i);
    }
    else
    {
      aBoxTree.Add (i, Bnd_Tools::Bnd2BVH (aSI.Box ()));
    }
  }

  aBoxTree.Build();

  // Perform selection
  BOPTools_Parallel::Perform (myRunParallel, aVTSR);

  // Treat selections

  // Fence map to avoid duplicating pairs
  BOPDS_MapOfPair aMPFence;

  const Standard_Integer aNbV = aVTSR.Length();
  for (Standard_Integer k = 0; k < aNbV; ++k)
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

      // avoid interfering of the shape with its sub-shapes
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

  myUseExt = Standard_True;
}

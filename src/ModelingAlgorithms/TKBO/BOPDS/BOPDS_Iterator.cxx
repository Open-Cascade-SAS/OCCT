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

#include <iostream> // DEBUG: For investigation output

#include <Bnd_OBB.hxx>
#include <Bnd_Tools.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexRange.hxx>
#include <BOPDS_Iterator.hxx>
#include <BOPDS_Pair.hxx>
#include <NCollection_Map.hxx>
#include <BOPDS_Tools.hxx>
#include <BOPTools_BoxTree.hxx>
#include <BOPTools_Parallel.hxx>
#include <IntTools_Context.hxx>
#include <NCollection_Vector.hxx>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////
//=================================================================================================

class BOPDS_TSR : public BOPTools_BoxTreeSelector
{
public:
  BOPDS_TSR()
      : myHasBRep(false),
        myIndex(-1)
  {
  }

  //
  ~BOPDS_TSR() override = default;

  //
  void SetHasBRep(const bool bFlag) { myHasBRep = bFlag; }

  //
  void SetIndex(const int theIndex) { myIndex = theIndex; }

  //
  int Index() const { return myIndex; }

  //
  void Perform()
  {
    if (myHasBRep)
    {
      Select();
    }
  }

  //
protected:
  bool myHasBRep;
  int  myIndex;
};

//
//=======================================================================
typedef NCollection_Vector<BOPDS_TSR> BOPDS_VectorOfTSR;

/////////////////////////////////////////////////////////////////////////

//=================================================================================================

BOPDS_Iterator::BOPDS_Iterator()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      myRunParallel(false),
      myUseExt(false)
{
  int i, aNb;
  //
  myDS     = nullptr;
  myLength = 0;
  //
  aNb = BOPDS_DS::NbInterfTypes();
  myLists.SetIncrement(aNb);
  for (i = 0; i < aNb; ++i)
  {
    myLists.Appended();
  }

  const int aNbExt = BOPDS_Iterator::NbExtInterfs();
  myExtLists.SetIncrement(aNbExt);
  for (i = 0; i < aNbExt; ++i)
  {
    myExtLists.Appended();
  }
}

//=================================================================================================

BOPDS_Iterator::BOPDS_Iterator(const occ::handle<NCollection_BaseAllocator>& theAllocator)
    : myAllocator(theAllocator),
      myLists(0, theAllocator),
      myRunParallel(false),
      myExtLists(0, theAllocator),
      myUseExt(false)
{
  int i, aNb;
  //
  myDS     = nullptr;
  myLength = 0;
  //
  aNb = BOPDS_DS::NbInterfTypes();
  myLists.SetIncrement(aNb);
  for (i = 0; i < aNb; ++i)
  {
    myLists.Appended();
  }

  const int aNbExt = BOPDS_Iterator::NbExtInterfs();
  myExtLists.SetIncrement(aNbExt);
  for (i = 0; i < aNbExt; ++i)
  {
    myExtLists.Appended();
  }
}

//=================================================================================================

BOPDS_Iterator::~BOPDS_Iterator() = default;

//=================================================================================================

void BOPDS_Iterator::SetRunParallel(const bool theFlag)
{
  myRunParallel = theFlag;
}

//=================================================================================================

bool BOPDS_Iterator::RunParallel() const
{
  return myRunParallel;
}

//=================================================================================================

void BOPDS_Iterator::SetDS(const BOPDS_PDS& aDS)
{
  myDS = aDS;
}

//=================================================================================================

const BOPDS_DS& BOPDS_Iterator::DS() const
{
  return *myDS;
}

//=================================================================================================

int BOPDS_Iterator::ExpectedLength() const
{
  return myLength;
}

//=================================================================================================

int BOPDS_Iterator::BlockLength() const
{
  int    aNbIIs;
  double aCfPredict = .5;

  aNbIIs = ExpectedLength();

  if (aNbIIs <= 1)
  {
    return 1;
  }
  //
  aNbIIs = (int)(aCfPredict * (double)aNbIIs);
  return aNbIIs;
}

//=================================================================================================

void BOPDS_Iterator::Initialize(const TopAbs_ShapeEnum aType1, const TopAbs_ShapeEnum aType2)
{
  int iX;
  //
  myLength = 0;
  iX       = BOPDS_Tools::TypeToInteger(aType1, aType2);
  if (iX >= 0)
  {
    NCollection_Vector<BOPDS_Pair>& aPairs =
      (myUseExt && iX < BOPDS_Iterator::NbExtInterfs()) ? myExtLists(iX) : myLists(iX);
    // sort interfering pairs for constant order of intersection
    std::stable_sort(aPairs.begin(), aPairs.end());
    // initialize iterator to access the pairs
    myIterator.Init(aPairs);
    myLength = aPairs.Length();
  }
}

//=================================================================================================

bool BOPDS_Iterator::More() const
{
  return myIterator.More();
}

//=================================================================================================

void BOPDS_Iterator::Next()
{
  myIterator.Next();
}

//=================================================================================================

void BOPDS_Iterator::Value(int& theI1, int& theI2) const
{
  int iT1, iT2, n1, n2;
  //
  const BOPDS_Pair& aPair = myIterator.Value();
  aPair.Indices(n1, n2);
  //
  iT1 = (int)(myDS->ShapeInfo(n1).ShapeType());
  iT2 = (int)(myDS->ShapeInfo(n2).ShapeType());
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

void BOPDS_Iterator::Prepare(const occ::handle<IntTools_Context>& theCtx,
                             const bool                           theCheckOBB,
                             const double                         theFuzzyValue)
{
  int i, aNbInterfTypes;
  //
  aNbInterfTypes = BOPDS_DS::NbInterfTypes();
  myLength       = 0;
  for (i = 0; i < aNbInterfTypes; ++i)
  {
    myLists(i).Clear();
  }
  //
  if (myDS == nullptr)
  {
    return;
  }
  Intersect(theCtx, theCheckOBB, theFuzzyValue);
}

//
//=================================================================================================

void BOPDS_Iterator::Intersect(const occ::handle<IntTools_Context>& theCtx,
                               const bool                           theCheckOBB,
                               const double                         theFuzzyValue)
{
  const int aNb = myDS->NbSourceShapes();

  // Prepare BVH
  BOPTools_BoxTree aBoxTree;
  aBoxTree.SetSize(aNb);
  for (int i = 0; i < aNb; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (!aSI.HasBRep())
      continue;
    const Bnd_Box& aBox = aSI.Box();
    aBoxTree.Add(i, Bnd_Tools::Bnd2BVH(aBox));
  }

  // Build BVH
  aBoxTree.Build();

  // Select pairs of shapes with interfering bounding boxes
  BOPTools_BoxPairSelector aPairSelector;
  aPairSelector.SetBVHSets(&aBoxTree, &aBoxTree);
  aPairSelector.SetSame(true);
  aPairSelector.Select();
  aPairSelector.Sort();

  // Treat the selected pairs
  const std::vector<BOPTools_BoxPairSelector::PairIDs>& aPairs   = aPairSelector.Pairs();
  const int                                             aNbPairs = static_cast<int>(aPairs.size());

  int iPair = 0;

  const int aNbR = myDS->NbRanges();
  for (int iR = 0; iR < aNbR; ++iR)
  {
    const BOPDS_IndexRange& aRange = myDS->Range(iR);

    for (; iPair < aNbPairs; ++iPair)
    {
      const BOPTools_BoxPairSelector::PairIDs& aPair = aPairs[iPair];
      if (!aRange.Contains(aPair.ID1))
        // Go to the next range
        break;

      if (aRange.Contains(aPair.ID2))
        // Go to the next pair
        continue;

      const BOPDS_ShapeInfo& aSI1 = myDS->ShapeInfo(aPair.ID1);
      const BOPDS_ShapeInfo& aSI2 = myDS->ShapeInfo(aPair.ID2);

      const TopAbs_ShapeEnum aType1 = aSI1.ShapeType();
      const TopAbs_ShapeEnum aType2 = aSI2.ShapeType();

      int iType1 = BOPDS_Tools::TypeToInteger(aType1);
      int iType2 = BOPDS_Tools::TypeToInteger(aType2);

      // avoid interfering of the shape with its sub-shapes
      if (((iType1 < iType2) && aSI1.HasSubShape(aPair.ID2))
          || ((iType1 > iType2) && aSI2.HasSubShape(aPair.ID1)))
      {
        continue;
      }

      if (theCheckOBB)
      {
        // Check intersection of Oriented bounding boxes of the shapes
        const Bnd_OBB& anOBB1 = theCtx->OBB(aSI1.Shape(), theFuzzyValue);
        const Bnd_OBB& anOBB2 = theCtx->OBB(aSI2.Shape(), theFuzzyValue);

        if (anOBB1.IsOut(anOBB2))
        {
          continue;
        }
      }

      int iX = BOPDS_Tools::TypeToInteger(aType1, aType2);
      myLists(iX).Append(
        BOPDS_Pair(std::min(aPair.ID1, aPair.ID2), std::max(aPair.ID1, aPair.ID2)));
    }
  }
}

//=================================================================================================

void BOPDS_Iterator::IntersectExt(const NCollection_Map<int>& theIndices)
{
  if (!myDS)
    return;

  const int aNb = myDS->NbSourceShapes();

  BOPTools_BoxTree aBoxTree;
  aBoxTree.SetSize(aNb);
  BOPDS_VectorOfTSR aVTSR(theIndices.Extent());
  //
  for (int i = 0; i < aNb; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    if (!aSI.IsInterfering() || (aSI.ShapeType() == TopAbs_SOLID))
      continue;
    //
    if (theIndices.Contains(i))
    {
      int nVSD = i;
      myDS->HasShapeSD(i, nVSD);
      const BOPDS_ShapeInfo& aSISD = myDS->ShapeInfo(nVSD);
      const Bnd_Box&         aBox  = aSISD.Box();
      aBoxTree.Add(i, Bnd_Tools::Bnd2BVH(aBox));

      BOPDS_TSR& aTSR = aVTSR.Appended();
      aTSR.SetHasBRep(true);
      aTSR.SetBVHSet(&aBoxTree);
      aTSR.SetBox(Bnd_Tools::Bnd2BVH(aBox));
      aTSR.SetIndex(i);
    }
    else
    {
      aBoxTree.Add(i, Bnd_Tools::Bnd2BVH(aSI.Box()));
    }
  }

  aBoxTree.Build();

  // Perform selection
  BOPTools_Parallel::Perform(myRunParallel, aVTSR);

  // Treat selections

  // Fence map to avoid duplicating pairs
  NCollection_Map<BOPDS_Pair> aMPFence;

  const int aNbV = aVTSR.Length();
  for (int k = 0; k < aNbV; ++k)
  {
    BOPDS_TSR&                   aTSRi = aVTSR(k);
    const NCollection_List<int>& aLI   = aTSRi.Indices();
    if (aLI.IsEmpty())
      continue;

    const int              i      = aTSRi.Index();
    const BOPDS_ShapeInfo& aSI    = myDS->ShapeInfo(i);
    const int              iRankI = myDS->Rank(i);
    const TopAbs_ShapeEnum aTI    = aSI.ShapeType();
    const int              iTI    = BOPDS_Tools::TypeToInteger(aTI);

    NCollection_List<int>::Iterator itLI(aLI);
    for (; itLI.More(); itLI.Next())
    {
      const int j      = itLI.Value(); // Index in DS
      const int iRankJ = myDS->Rank(j);
      if (iRankI == iRankJ)
        continue;

      const BOPDS_ShapeInfo& aSJ = myDS->ShapeInfo(j);
      const TopAbs_ShapeEnum aTJ = aSJ.ShapeType();
      const int              iTJ = BOPDS_Tools::TypeToInteger(aTJ);

      // avoid interfering of the shape with its sub-shapes
      if (((iTI < iTJ) && aSI.HasSubShape(j)) || ((iTI > iTJ) && aSJ.HasSubShape(i)))
        continue;

      BOPDS_Pair aPair(i, j);
      if (aMPFence.Add(aPair))
      {
        const int iX = BOPDS_Tools::TypeToInteger(aTI, aTJ);
        if (iX < BOPDS_Iterator::NbExtInterfs())
          myExtLists(iX).Append(aPair);
      }
    }
  }

  myUseExt = true;
}

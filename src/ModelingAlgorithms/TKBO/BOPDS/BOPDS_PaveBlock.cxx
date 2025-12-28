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

#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <algorithm>
IMPLEMENT_STANDARD_RTTIEXT(BOPDS_PaveBlock, Standard_Transient)

//=================================================================================================

BOPDS_PaveBlock::BOPDS_PaveBlock()
    : myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
      myExtPaves(myAllocator)
{
  myEdge         = -1;
  myOriginalEdge = -1;
  myTS1          = -99.;
  myTS2          = myTS1;
  myIsSplittable = false;
}

//=================================================================================================

BOPDS_PaveBlock::BOPDS_PaveBlock(const occ::handle<NCollection_BaseAllocator>& theAllocator)
    : myAllocator(theAllocator),
      myExtPaves(theAllocator),
      myMFence(100, theAllocator)
{
  myEdge         = -1;
  myOriginalEdge = -1;
  myTS1          = -99.;
  myTS2          = myTS1;
  myIsSplittable = false;
}

//=================================================================================================

void BOPDS_PaveBlock::SetEdge(const int theEdge)
{
  myEdge = theEdge;
}

//=================================================================================================

int BOPDS_PaveBlock::Edge() const
{
  return myEdge;
}

//=================================================================================================

bool BOPDS_PaveBlock::HasEdge() const
{
  return (myEdge >= 0);
}

//=================================================================================================

bool BOPDS_PaveBlock::HasEdge(int& theEdge) const
{
  theEdge = myEdge;
  return (myEdge >= 0);
}

//=================================================================================================

void BOPDS_PaveBlock::SetOriginalEdge(const int theEdge)
{
  myOriginalEdge = theEdge;
}

//=================================================================================================

int BOPDS_PaveBlock::OriginalEdge() const
{
  return myOriginalEdge;
}

//=================================================================================================

bool BOPDS_PaveBlock::IsSplitEdge() const
{
  return (myEdge != myOriginalEdge);
}

//=================================================================================================

void BOPDS_PaveBlock::SetPave1(const BOPDS_Pave& thePave)
{
  myPave1 = thePave;
}

//=================================================================================================

const BOPDS_Pave& BOPDS_PaveBlock::Pave1() const
{
  return myPave1;
}

//=================================================================================================

void BOPDS_PaveBlock::SetPave2(const BOPDS_Pave& thePave)
{
  myPave2 = thePave;
}

//=================================================================================================

const BOPDS_Pave& BOPDS_PaveBlock::Pave2() const
{
  return myPave2;
}

//=================================================================================================

void BOPDS_PaveBlock::Range(double& theT1, double& theT2) const
{
  theT1 = myPave1.Parameter();
  theT2 = myPave2.Parameter();
}

//=================================================================================================

void BOPDS_PaveBlock::Indices(int& theIndex1, int& theIndex2) const
{
  theIndex1 = myPave1.Index();
  theIndex2 = myPave2.Index();
}

//=================================================================================================

bool BOPDS_PaveBlock::HasSameBounds(const occ::handle<BOPDS_PaveBlock>& theOther) const
{
  bool bFlag1, bFlag2;
  int  n11, n12, n21, n22;
  //
  Indices(n11, n12);
  theOther->Indices(n21, n22);
  //
  bFlag1 = (n11 == n21) && (n12 == n22);
  bFlag2 = (n11 == n22) && (n12 == n21);
  //
  return (bFlag1 || bFlag2);
}

//
// Extras
//
//=================================================================================================

void BOPDS_PaveBlock::AppendExtPave(const BOPDS_Pave& thePave)
{
  if (myMFence.Add(thePave.Index()))
  {
    myExtPaves.Append(thePave);
  }
}

//=================================================================================================

void BOPDS_PaveBlock::AppendExtPave1(const BOPDS_Pave& thePave)
{
  myExtPaves.Append(thePave);
}

//=================================================================================================

void BOPDS_PaveBlock::RemoveExtPave(const int theVertNum)
{
  if (myMFence.Contains(theVertNum))
  {
    NCollection_List<BOPDS_Pave>::Iterator itPaves(myExtPaves);
    while (itPaves.More())
    {
      if (itPaves.Value().Index() == theVertNum)
        myExtPaves.Remove(itPaves);
      else
        itPaves.Next();
    }
    myMFence.Remove(theVertNum);
  }
}

//=================================================================================================

const NCollection_List<BOPDS_Pave>& BOPDS_PaveBlock::ExtPaves() const
{
  return myExtPaves;
}

//=================================================================================================

NCollection_List<BOPDS_Pave>& BOPDS_PaveBlock::ChangeExtPaves()
{
  return myExtPaves;
}

//=================================================================================================

bool BOPDS_PaveBlock::IsToUpdate() const
{
  return !myExtPaves.IsEmpty();
}

//=================================================================================================

bool BOPDS_PaveBlock::ContainsParameter(const double theT, const double theTol, int& theInd) const
{
  bool                                   bRet;
  NCollection_List<BOPDS_Pave>::Iterator aIt;
  //
  bRet = false;
  aIt.Initialize(myExtPaves);
  for (; aIt.More(); aIt.Next())
  {
    const BOPDS_Pave& aPave = aIt.Value();
    bRet                    = (std::abs(aPave.Parameter() - theT) < theTol);
    if (bRet)
    {
      theInd = aPave.Index();
      break;
    }
  }
  return bRet;
}

//=================================================================================================

void BOPDS_PaveBlock::Update(NCollection_List<occ::handle<BOPDS_PaveBlock>>& theLPB,
                             const bool                                      theFlag)
{
  int                                    i, aNb;
  BOPDS_Pave                             aPave1, aPave2;
  occ::handle<BOPDS_PaveBlock>           aPB;
  NCollection_List<BOPDS_Pave>::Iterator aIt;
  //
  aNb = myExtPaves.Extent();
  if (theFlag)
  {
    aNb = aNb + 2;
  }
  //
  if (aNb <= 1)
  {
    myExtPaves.Clear();
    myMFence.Clear();
    return;
  }
  //
  NCollection_Array1<BOPDS_Pave> pPaves(1, aNb);
  //
  i = 1;
  if (theFlag)
  {
    pPaves(i) = myPave1;
    ++i;
    pPaves(i) = myPave2;
    ++i;
  }
  //
  aIt.Initialize(myExtPaves);
  for (; aIt.More(); aIt.Next())
  {
    const BOPDS_Pave& aPave = aIt.Value();
    pPaves(i)               = aPave;
    ++i;
  }
  myExtPaves.Clear();
  myMFence.Clear();
  //
  std::sort(pPaves.begin(), pPaves.end());
  //
  for (i = 1; i <= aNb; ++i)
  {
    const BOPDS_Pave& aPave = pPaves(i);
    if (i == 1)
    {
      aPave1 = aPave;
      continue;
    }
    //
    aPave2 = aPave;
    aPB    = new BOPDS_PaveBlock;
    aPB->SetOriginalEdge(myOriginalEdge);
    aPB->SetPave1(aPave1);
    aPB->SetPave2(aPave2);
    //
    theLPB.Append(aPB);
    //
    aPave1 = aPave2;
  }
}

// ShrunkData
//=================================================================================================

bool BOPDS_PaveBlock::HasShrunkData() const
{
  return (!myShrunkBox.IsVoid());
}

//=================================================================================================

void BOPDS_PaveBlock::SetShrunkData(const double   theT1,
                                    const double   theT2,
                                    const Bnd_Box& theBox,
                                    const bool     theIsSplittable)
{
  myTS1          = theT1;
  myTS2          = theT2;
  myShrunkBox    = theBox;
  myIsSplittable = theIsSplittable;
}

//=================================================================================================

void BOPDS_PaveBlock::ShrunkData(double&  theT1,
                                 double&  theT2,
                                 Bnd_Box& theBox,
                                 bool&    theIsSplittable) const
{
  theT1           = myTS1;
  theT2           = myTS2;
  theBox          = myShrunkBox;
  theIsSplittable = myIsSplittable;
}

//=================================================================================================

void BOPDS_PaveBlock::Dump() const
{
  printf(" PB:{ E:%d orE:%d", myEdge, myOriginalEdge);
  printf(" Pave1:");
  myPave1.Dump();
  printf(" Pave2:");
  myPave2.Dump();
  printf(" }");
}

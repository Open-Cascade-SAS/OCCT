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
#include <BOPDS_VectorOfPave.hxx>
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
  myIsSplittable = Standard_False;
}

//=================================================================================================

BOPDS_PaveBlock::BOPDS_PaveBlock(const Handle(NCollection_BaseAllocator)& theAllocator)
    : myAllocator(theAllocator),
      myExtPaves(theAllocator),
      myMFence(100, theAllocator)
{
  myEdge         = -1;
  myOriginalEdge = -1;
  myTS1          = -99.;
  myTS2          = myTS1;
  myIsSplittable = Standard_False;
}

//=================================================================================================

void BOPDS_PaveBlock::SetEdge(const Standard_Integer theEdge)
{
  myEdge = theEdge;
}

//=================================================================================================

Standard_Integer BOPDS_PaveBlock::Edge() const
{
  return myEdge;
}

//=================================================================================================

Standard_Boolean BOPDS_PaveBlock::HasEdge() const
{
  return (myEdge >= 0);
}

//=================================================================================================

Standard_Boolean BOPDS_PaveBlock::HasEdge(Standard_Integer& theEdge) const
{
  theEdge = myEdge;
  return (myEdge >= 0);
}

//=================================================================================================

void BOPDS_PaveBlock::SetOriginalEdge(const Standard_Integer theEdge)
{
  myOriginalEdge = theEdge;
}

//=================================================================================================

Standard_Integer BOPDS_PaveBlock::OriginalEdge() const
{
  return myOriginalEdge;
}

//=================================================================================================

Standard_Boolean BOPDS_PaveBlock::IsSplitEdge() const
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

void BOPDS_PaveBlock::Range(Standard_Real& theT1, Standard_Real& theT2) const
{
  theT1 = myPave1.Parameter();
  theT2 = myPave2.Parameter();
}

//=================================================================================================

void BOPDS_PaveBlock::Indices(Standard_Integer& theIndex1, Standard_Integer& theIndex2) const
{
  theIndex1 = myPave1.Index();
  theIndex2 = myPave2.Index();
}

//=================================================================================================

Standard_Boolean BOPDS_PaveBlock::HasSameBounds(const Handle(BOPDS_PaveBlock)& theOther) const
{
  Standard_Boolean bFlag1, bFlag2;
  Standard_Integer n11, n12, n21, n22;
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

void BOPDS_PaveBlock::RemoveExtPave(const Standard_Integer theVertNum)
{
  if (myMFence.Contains(theVertNum))
  {
    BOPDS_ListOfPave::Iterator itPaves(myExtPaves);
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

const BOPDS_ListOfPave& BOPDS_PaveBlock::ExtPaves() const
{
  return myExtPaves;
}

//=================================================================================================

BOPDS_ListOfPave& BOPDS_PaveBlock::ChangeExtPaves()
{
  return myExtPaves;
}

//=================================================================================================

Standard_Boolean BOPDS_PaveBlock::IsToUpdate() const
{
  return !myExtPaves.IsEmpty();
}

//=================================================================================================

Standard_Boolean BOPDS_PaveBlock::ContainsParameter(const Standard_Real theT,
                                                    const Standard_Real theTol,
                                                    Standard_Integer&   theInd) const
{
  Standard_Boolean               bRet;
  BOPDS_ListIteratorOfListOfPave aIt;
  //
  bRet = Standard_False;
  aIt.Initialize(myExtPaves);
  for (; aIt.More(); aIt.Next())
  {
    const BOPDS_Pave& aPave = aIt.Value();
    bRet                    = (Abs(aPave.Parameter() - theT) < theTol);
    if (bRet)
    {
      theInd = aPave.Index();
      break;
    }
  }
  return bRet;
}

//=================================================================================================

void BOPDS_PaveBlock::Update(BOPDS_ListOfPaveBlock& theLPB, const Standard_Boolean theFlag)
{
  Standard_Integer               i, aNb;
  BOPDS_Pave                     aPave1, aPave2;
  Handle(BOPDS_PaveBlock)        aPB;
  BOPDS_ListIteratorOfListOfPave aIt;
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
  BOPDS_VectorOfPave pPaves(1, aNb);
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

Standard_Boolean BOPDS_PaveBlock::HasShrunkData() const
{
  return (!myShrunkBox.IsVoid());
}

//=================================================================================================

void BOPDS_PaveBlock::SetShrunkData(const Standard_Real    theT1,
                                    const Standard_Real    theT2,
                                    const Bnd_Box&         theBox,
                                    const Standard_Boolean theIsSplittable)
{
  myTS1          = theT1;
  myTS2          = theT2;
  myShrunkBox    = theBox;
  myIsSplittable = theIsSplittable;
}

//=================================================================================================

void BOPDS_PaveBlock::ShrunkData(Standard_Real&    theT1,
                                 Standard_Real&    theT2,
                                 Bnd_Box&          theBox,
                                 Standard_Boolean& theIsSplittable) const
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

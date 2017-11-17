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


#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BOPDS_CommonBlock,Standard_Transient)

//=======================================================================
// function:  
// purpose: 
//=======================================================================
  BOPDS_CommonBlock::BOPDS_CommonBlock()
{
}
//=======================================================================
// function:  
// purpose: 
//=======================================================================
  BOPDS_CommonBlock::BOPDS_CommonBlock(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myPaveBlocks(theAllocator),
  myFaces(theAllocator)
{
}
//=======================================================================
// function:  AddPaveBlock
// purpose: 
//=======================================================================
  void BOPDS_CommonBlock::AddPaveBlock(const Handle(BOPDS_PaveBlock)& aPB)
{
  if (myPaveBlocks.IsEmpty()) {
    myPaveBlocks.Append(aPB);
    return;
  }
  //
  // Put the pave block with the minimal index of the original edge in the first place
  if (aPB->OriginalEdge() < myPaveBlocks.First()->OriginalEdge()) {
    myPaveBlocks.Prepend(aPB);
  }
  else {
    myPaveBlocks.Append(aPB);
  }
}
//=======================================================================
// function:  SetPaveBlocks
// purpose: 
//=======================================================================
  void BOPDS_CommonBlock::SetPaveBlocks(const BOPDS_ListOfPaveBlock& aLPB)
{
  myPaveBlocks.Clear();
  BOPDS_ListIteratorOfListOfPaveBlock aIt(aLPB);
  for (; aIt.More(); aIt.Next()) {
    AddPaveBlock(aIt.Value());
  }
}
//=======================================================================
// function:  PaveBlocks
// purpose: 
//=======================================================================
  const BOPDS_ListOfPaveBlock& BOPDS_CommonBlock::PaveBlocks()const
{
  return myPaveBlocks;
}
//=======================================================================
// function:  AddFace
// purpose: 
//=======================================================================
  void BOPDS_CommonBlock::AddFace(const Standard_Integer aF)
{
  myFaces.Append(aF);
}
//=======================================================================
// function:  AddFaces
// purpose: 
//=======================================================================
  void BOPDS_CommonBlock::SetFaces(const TColStd_ListOfInteger& aLF)
{
  myFaces=aLF;
}
//=======================================================================
// function:  AppendFaces
// purpose: 
//=======================================================================
void BOPDS_CommonBlock::AppendFaces(TColStd_ListOfInteger& aLF)
{
  myFaces.Append(aLF);
}
  //=======================================================================
// function:  Faces
// purpose: 
//=======================================================================
  const TColStd_ListOfInteger& BOPDS_CommonBlock::Faces()const
{
  return myFaces;
}
//=======================================================================
// function:  PaveBlock1
// purpose: 
//=======================================================================
  const Handle(BOPDS_PaveBlock)& BOPDS_CommonBlock::PaveBlock1()const
{
  return myPaveBlocks.First();
}
//=======================================================================
// function:  SetRealPaveBlock
// purpose: 
//=======================================================================
void BOPDS_CommonBlock::SetRealPaveBlock(const Handle(BOPDS_PaveBlock)& thePB)
{
  BOPDS_ListIteratorOfListOfPaveBlock it(myPaveBlocks);
  for (; it.More(); it.Next())
  {
    if (it.Value() == thePB)
    {
      myPaveBlocks.Prepend(thePB);
      myPaveBlocks.Remove(it);
      break;
    }
  }
}
//=======================================================================
// function:  PaveBlockOnEdge
// purpose: 
//=======================================================================
  Handle(BOPDS_PaveBlock)& BOPDS_CommonBlock::PaveBlockOnEdge(const Standard_Integer aIx)
{
  static Handle(BOPDS_PaveBlock) aPBs;
  //
  Standard_Integer aIOr;
  BOPDS_ListIteratorOfListOfPaveBlock anIt;
  //
  anIt.Initialize(myPaveBlocks);
  for (; anIt.More(); anIt.Next()) {
    Handle(BOPDS_PaveBlock)& aPB=anIt.ChangeValue();
    aIOr=aPB->OriginalEdge();
    if (aIOr==aIx){
      return aPB;
    }
  }
  return aPBs;
}
//=======================================================================
// function:  IsPaveBlockOnFace
// purpose: 
//=======================================================================
  Standard_Boolean BOPDS_CommonBlock::IsPaveBlockOnFace(const Standard_Integer aIx)const
{
  Standard_Boolean bFound;
  Standard_Integer nF;
  TColStd_ListIteratorOfListOfInteger anIt;
  //
  bFound=Standard_False;
  anIt.Initialize(myFaces);
  for (; anIt.More(); anIt.Next()) {
    nF=anIt.Value();
    if (nF==aIx){
      return !bFound;
    }
  }
  return bFound;
}
//=======================================================================
// function:  IsPaveBlockOnEdge
// purpose: 
//=======================================================================
  Standard_Boolean BOPDS_CommonBlock::IsPaveBlockOnEdge(const Standard_Integer aIx)const
{
  Standard_Boolean bFound;
  Standard_Integer aIOr;
  BOPDS_ListIteratorOfListOfPaveBlock anIt;
  //
  bFound=Standard_False;
  anIt.Initialize(myPaveBlocks);
  for (; anIt.More(); anIt.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB=anIt.Value();
    aIOr=aPB->OriginalEdge();
    if (aIOr==aIx){
      return !bFound;
    }
  }
  return bFound;
}
//=======================================================================
//function : SetEdge
//purpose  : 
//=======================================================================
  void BOPDS_CommonBlock::SetEdge(const Standard_Integer theEdge)
{
  BOPDS_ListIteratorOfListOfPaveBlock anIt;
  //
  anIt.Initialize(myPaveBlocks);
  for (; anIt.More(); anIt.Next()) {
    Handle(BOPDS_PaveBlock)& aPB=anIt.ChangeValue();
    aPB->SetEdge(theEdge);
  }
}
//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
  Standard_Integer BOPDS_CommonBlock::Edge()const
{
  const Handle(BOPDS_PaveBlock)& aPB1=myPaveBlocks.First();
  if(!aPB1.IsNull()) {
    return aPB1->Edge();
  }
  return -1;
}
//=======================================================================
// function:  Contains
// purpose: 
//=======================================================================
  Standard_Boolean BOPDS_CommonBlock::Contains(const Handle(BOPDS_PaveBlock)& aPBx)const
{
  Standard_Boolean bFound;
  Standard_Integer aNb1;
  BOPDS_ListIteratorOfListOfPaveBlock anIt;
  //
  bFound=Standard_False;
  aNb1=myPaveBlocks.Extent();
  //
  if (!aNb1) {
    return bFound;
  }
  //
  anIt.Initialize(myPaveBlocks);
  for (; anIt.More(); anIt.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB=anIt.Value();
    if (aPB==aPBx) {
      return !bFound;
    }
  }
  return bFound;
}
//=======================================================================
// function:  Contains
// purpose: 
//=======================================================================
  Standard_Boolean BOPDS_CommonBlock::Contains(const Standard_Integer theF)const
{
  Standard_Boolean bFound;
  TColStd_ListIteratorOfListOfInteger aIt;
  //
  bFound=Standard_False;
  aIt.Initialize(myFaces);
  for (; aIt.More(); aIt.Next()) {
    if (aIt.Value()==theF) {
      return !bFound;
    }
  }
  return bFound;
}
//=======================================================================
// function:  Dump
// purpose: 
//=======================================================================
  void BOPDS_CommonBlock::Dump()const
{
  Standard_Integer nF;
  TColStd_ListIteratorOfListOfInteger aIt;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  //
  printf(" -- CB:\n");
  aItPB.Initialize(myPaveBlocks);
  for (; aItPB.More(); aItPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB=aItPB.Value();
    aPB->Dump();
    printf("\n");
  }
  //
  if (myFaces.Extent()) {
    printf(" Faces:");
    aIt.Initialize(myFaces);
    for (; aIt.More(); aIt.Next()) {
      nF=aIt.Value();
      printf(" %d", nF);
    }
    printf("\n");
  }
}

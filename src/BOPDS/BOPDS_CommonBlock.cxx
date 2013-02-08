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


#include <BOPDS_CommonBlock.ixx>

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
  myPaveBlocks.Append(aPB);
}
//=======================================================================
// function:  AddPaveBlocks
// purpose: 
//=======================================================================
  void BOPDS_CommonBlock::AddPaveBlocks(const BOPDS_ListOfPaveBlock& aLPB)
{
  myPaveBlocks=aLPB;
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
  void BOPDS_CommonBlock::AddFaces(const BOPCol_ListOfInteger& aLF)
{
  myFaces=aLF;
}
//=======================================================================
// function:  Faces
// purpose: 
//=======================================================================
  const BOPCol_ListOfInteger& BOPDS_CommonBlock::Faces()const
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
  BOPCol_ListIteratorOfListOfInteger anIt;
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
  BOPCol_ListIteratorOfListOfInteger aIt;
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
  BOPCol_ListIteratorOfListOfInteger aIt;
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

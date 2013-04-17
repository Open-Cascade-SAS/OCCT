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


#include <BOPDS_PaveBlock.ixx>
#include <BOPDS_ListOfPave.hxx>
#include <Standard.hxx>
#include <NCollection_BaseAllocator.hxx>


#ifdef WNT
#pragma warning ( disable : 4291 )
#endif

static
  void SortShell(const int n, BOPDS_Pave *a);

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPDS_PaveBlock::BOPDS_PaveBlock()
:
  myAllocator(NCollection_BaseAllocator::CommonBaseAllocator()),
  myExtPaves(myAllocator)
{
  myEdge=-1;
  myOriginalEdge=-1;
  myTS1=-99.;
  myTS2=myTS1;
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BOPDS_PaveBlock::BOPDS_PaveBlock(const Handle(NCollection_BaseAllocator)& theAllocator)
:
  myAllocator(theAllocator),
  myExtPaves(theAllocator),
  myMFence(100, theAllocator)
{
  myEdge=-1;
  myOriginalEdge=-1;
  myTS1=-99.;
  myTS2=myTS1;
}

//=======================================================================
//function : SetEdge
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::SetEdge(const Standard_Integer theEdge)
{
  myEdge=theEdge;
}
//=======================================================================
//function : Edge
//purpose  : 
//=======================================================================
  Standard_Integer BOPDS_PaveBlock::Edge()const
{
  return myEdge;
}
//=======================================================================
//function : HasEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::HasEdge()const
{
  return (myEdge>=0);
}
//=======================================================================
//function : HasEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::HasEdge(Standard_Integer& theEdge)const
{
  theEdge=myEdge;
  return (myEdge>=0);
}

//=======================================================================
//function : SetOriginalEdge
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::SetOriginalEdge(const Standard_Integer theEdge)
{
  myOriginalEdge=theEdge;
}
//=======================================================================
//function : OriginalEdge
//purpose  : 
//=======================================================================
  Standard_Integer BOPDS_PaveBlock::OriginalEdge()const
{
  return myOriginalEdge;
}
//=======================================================================
//function : IsSplitEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::IsSplitEdge()const
{
  return (myEdge!=myOriginalEdge);
}
//=======================================================================
//function : SetPave1
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::SetPave1(const BOPDS_Pave& thePave)
{
  myPave1=thePave;
}
//=======================================================================
//function : Pave1
//purpose  : 
//=======================================================================
  const BOPDS_Pave& BOPDS_PaveBlock::Pave1()const
{
  return myPave1;
}
//=======================================================================
//function : SetPave2
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::SetPave2(const BOPDS_Pave& thePave)
{
  myPave2=thePave;
}
//=======================================================================
//function : Pave2
//purpose  : 
//=======================================================================
  const BOPDS_Pave& BOPDS_PaveBlock::Pave2()const
{
  return myPave2;
}
//=======================================================================
//function : Range
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::Range(Standard_Real& theT1,
                              Standard_Real& theT2)const
{
  theT1=myPave1.Parameter();
  theT2=myPave2.Parameter();
}
//=======================================================================
//function : Indices
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::Indices(Standard_Integer& theIndex1,
                                Standard_Integer& theIndex2)const
{
  theIndex1=myPave1.Index();
  theIndex2=myPave2.Index();
}
//=======================================================================
//function : HasSameBounds
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::HasSameBounds(const Handle(BOPDS_PaveBlock)& theOther)const
{
  Standard_Boolean bFlag1, bFlag2;
  Standard_Integer n11, n12, n21, n22;
  //
  Indices(n11, n12);
  theOther->Indices(n21, n22);
  //
  bFlag1=(n11==n21) && (n12==n22);
  bFlag2=(n11==n22) && (n12==n21);
  //
  return (bFlag1 || bFlag2);
}


//
// Extras
//
//=======================================================================
//function : AppendExtPave
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::AppendExtPave(const BOPDS_Pave& thePave)
{
  if (myMFence.Add(thePave.Index())) {
    myExtPaves.Append(thePave);
  }
}
//=======================================================================
//function : AppendExtPave1
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::AppendExtPave1(const BOPDS_Pave& thePave)
{
  myExtPaves.Append(thePave);
}
//=======================================================================
//function : ExtPaves
//purpose  : 
//=======================================================================
  const BOPDS_ListOfPave& BOPDS_PaveBlock::ExtPaves()const 
{
  return myExtPaves;
}
//=======================================================================
//function : ChangeExtPaves
//purpose  : 
//=======================================================================
  BOPDS_ListOfPave& BOPDS_PaveBlock::ChangeExtPaves() 
{
  return myExtPaves;
}
//=======================================================================
//function : IsToUpdate
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::IsToUpdate()const 
{
  return !myExtPaves.IsEmpty();
}
//=======================================================================
//function : ContainsParameter
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::ContainsParameter(const Standard_Real theT,
                                                      const Standard_Real theTol)const
{
  Standard_Boolean bRet;
  Standard_Real dT;
  BOPDS_ListIteratorOfListOfPave aIt;
  //
  bRet=Standard_False;
  aIt.Initialize(myExtPaves);
  for (; aIt.More(); aIt.Next()) {
    dT=aIt.Value().Parameter()-theT;
    if (dT<0.) {
      dT=-dT;
    }
    if (dT<theTol) {
      bRet=!bRet;
      break;
    }
  }
  return bRet;
}
//=======================================================================
//function : Update
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::Update(BOPDS_ListOfPaveBlock& theLPB,
                               const Standard_Boolean theFlag)
{
  Standard_Integer i, aNb;
  BOPDS_Pave *pPaves;
  BOPDS_Pave aPave1, aPave2;
  Handle(BOPDS_PaveBlock) aPB;
  BOPDS_ListIteratorOfListOfPave aIt;
  //
  aNb=myExtPaves.Extent();
  if (theFlag) {
    aNb=aNb+2;
  }
  //
  pPaves=(BOPDS_Pave *)myAllocator->Allocate(aNb*sizeof(BOPDS_Pave));
  for (i=0; i<aNb; ++i) {
    new (pPaves+i) BOPDS_Pave();
  }
  //
  i=0;
  if (theFlag) {
    pPaves[i]=myPave1; 
    ++i;
    pPaves[i]=myPave2; 
    ++i;
  }
  //
  aIt.Initialize(myExtPaves);
  for (; aIt.More(); aIt.Next()) {
    const BOPDS_Pave& aPave=aIt.Value();
    pPaves[i]=(aPave);
    ++i;
  }
  myExtPaves.Clear();
  myMFence.Clear();
  //
  SortShell(aNb, pPaves);
  //
  for (i=0; i<aNb; ++i) {
    const BOPDS_Pave& aPave=pPaves[i];
    if (!i) {
      aPave1=aPave;
      continue;
    }
    //
    aPave2=aPave;
    aPB=new BOPDS_PaveBlock;
    aPB->SetOriginalEdge(myOriginalEdge);
    aPB->SetPave1(aPave1);
    aPB->SetPave2(aPave2);
    //
    theLPB.Append(aPB);
    //
    aPave1=aPave2;
  }
  //
  for (i=0; i<aNb; ++i) {
    pPaves[i].~BOPDS_Pave();
  }
  myAllocator->Free((Standard_Address&)pPaves); 
}

//=======================================================================
// function: SortShell
// purpose : 
//=======================================================================
void SortShell(const int n, BOPDS_Pave *a) 
{
  int nd, i, j, l, d=1;
  BOPDS_Pave x;
  //
  while(d<=n) {
    d*=2;
  }
  //
  while (d) {
    d=(d-1)/2;
    //
    nd=n-d;
    for (i=0; i<nd; ++i) {
      j=i;
    m30:;
      l=j+d;
      if (a[l] < a[j]){
        x=a[j];
        a[j]=a[l];
        a[l]=x;
        j-=d;
        if (j > -1) goto m30;
      }//if (a[l] < a[j]){
    }//for (i=0; i<nd; ++i) 
  }//while (1)
}
//
// Common block   
//
//=======================================================================
//function : SetCommonBlock
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::SetCommonBlock(const Handle(BOPDS_CommonBlock)& theCB)
{
  myCommonBlock=theCB;
}
//=======================================================================
//function : CommonBlock
//purpose  : 
//=======================================================================
  const Handle(BOPDS_CommonBlock)& BOPDS_PaveBlock::CommonBlock()const
{
  return myCommonBlock;
}
//=======================================================================
//function : IsCommonBlock
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::IsCommonBlock()const
{
  return (!myCommonBlock.IsNull());
}
//=======================================================================
//function : IsCommonBlockOnEdge
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::IsCommonBlockOnEdge()const
{
  if (!myCommonBlock.IsNull()) {
    return (myCommonBlock->PaveBlocks().Extent()>1);
  }
  return Standard_False;
}
//=======================================================================
//function : RealPaveBlock
//purpose  : 
//=======================================================================
  Handle(BOPDS_PaveBlock) BOPDS_PaveBlock::RealPaveBlock()const
{
  if (IsCommonBlock()) {
    const Handle(BOPDS_PaveBlock)& aPB1=myCommonBlock->PaveBlock1();
    return aPB1;
  }
  return this;
}
// ShrunkData
//=======================================================================
//function : HasShrunkData
//purpose  : 
//=======================================================================
  Standard_Boolean BOPDS_PaveBlock::HasShrunkData()const
{
  return (!myShrunkBox.IsVoid());
}
//=======================================================================
//function : SetShrunkData
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::SetShrunkData(const Standard_Real theT1,
                                      const Standard_Real theT2,
                                      const Bnd_Box& theBox)
{
  myTS1=theT1;
  myTS2=theT2;
  myShrunkBox=theBox;
}
//=======================================================================
//function : ShrunkData
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::ShrunkData(Standard_Real& theT1,
                                   Standard_Real& theT2,
                                   Bnd_Box& theBox)const
{
  theT1=myTS1;
  theT2=myTS2;
  theBox=myShrunkBox;
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
  void BOPDS_PaveBlock::Dump()const
{
  printf(" PB:{ E:%d orE:%d", myEdge, myOriginalEdge);
  printf(" Pave1:");
  myPave1.Dump();
  printf(" Pave2:");
  myPave2.Dump();
  printf(" }");
}


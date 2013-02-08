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

#include <BOPAlgo_Tools.ixx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_IndexedMapOfPaveBlock.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPCol_IndexedMapOfInteger.hxx>

//=======================================================================
//function : MakeBlocksCnx
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::MakeBlocksCnx(const BOPCol_IndexedDataMapOfIntegerListOfInteger& aMILI,
                                    BOPCol_DataMapOfIntegerListOfInteger& aMBlocks,
                                    Handle(NCollection_BaseAllocator)& aAllocator)
{
  Standard_Integer aNbV, nV, aNbVS, nVP, nVx, aNbVP, aNbEC, k, i, j;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  //
  BOPCol_MapOfInteger aMVS(100, aAllocator);
  BOPCol_IndexedMapOfInteger aMEC(100, aAllocator);
  BOPCol_IndexedMapOfInteger aMVP(100, aAllocator);
  BOPCol_IndexedMapOfInteger aMVAdd(100, aAllocator);
  //
  aNbV=aMILI.Extent();
  //
  for (k=0,i=1; i<=aNbV; ++i) {
    aNbVS=aMVS.Extent();
    if (aNbVS==aNbV) {
      break;
    }
    //
    nV = aMILI.FindKey(i);
    if (aMVS.Contains(nV)){
      continue;
    }
    aMVS.Add(nV);
    //
    aMEC.Clear();
    aMVP.Clear();
    aMVAdd.Clear();
    //
    aMVP.Add(nV);
    while(1) {
      aNbVP=aMVP.Extent();
      for (j=1; j<=aNbVP; ++j) {
        nVP=aMVP(j);
        const BOPCol_ListOfInteger& aLV=aMILI.FindFromKey(nVP);
        aItLI.Initialize(aLV);
        for (; aItLI.More(); aItLI.Next()) {
          nVx=aItLI.Value();
          if (aMEC.Contains(nVx)) {
            continue;
          }
          //
          aMVS.Add(nVx);
          aMEC.Add(nVx);
          aMVAdd.Add(nVx);
        }
      }
      //
      aNbVP=aMVAdd.Extent();
      if (!aNbVP) {
        break; // from while(1)
      }
      //
      aMVP.Clear();
      for (j=1; j<=aNbVP; ++j) {
        aMVP.Add(aMVAdd(j));
      }
      aMVAdd.Clear();
    }//while(1) {
    //
    BOPCol_ListOfInteger aLIx(aAllocator);
    //
    aNbEC = aMEC.Extent();
    for (j=1; j<=aNbEC; ++j) {
      nVx=aMEC(j);
      aLIx.Append(nVx);
    }
    //
    aMBlocks.Bind(k, aLIx);
    ++k;
  }//for (k=0,i=1; i<=aNbV; ++i)
  aMVAdd.Clear();
  aMVP.Clear();
  aMEC.Clear();
  aMVS.Clear();
}
//=======================================================================
//function : FillMap
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::FillMap(const Standard_Integer n1,
                              const Standard_Integer n2,
                              BOPCol_IndexedDataMapOfIntegerListOfInteger& aMILI,
                              Handle(NCollection_BaseAllocator)& aAllocator)
{
  if (aMILI.Contains(n1)) {
    BOPCol_ListOfInteger& aLI=aMILI.ChangeFromKey(n1);
    aLI.Append(n2);
  }
  else {
    BOPCol_ListOfInteger aLI(aAllocator);
    aLI.Append(n2);
    aMILI.Add(n1, aLI);
  }
  if (aMILI.Contains(n2)) {
    BOPCol_ListOfInteger& aLI=aMILI.ChangeFromKey(n2);
    aLI.Append(n1);
  }
  else {
    BOPCol_ListOfInteger aLI(aAllocator);
    aLI.Append(n1);
    aMILI.Add(n2, aLI);
  }
}
//=======================================================================
//function : FillMap
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::FillMap(const Handle(BOPDS_PaveBlock)& aPB1,
                              const Handle(BOPDS_PaveBlock)& aPB2,
                              BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock& aMPBLPB,
                              Handle(NCollection_BaseAllocator)& aAllocator)
{
  if (aMPBLPB.Contains(aPB1)) {
    BOPDS_ListOfPaveBlock& aLPB=aMPBLPB.ChangeFromKey(aPB1);
    aLPB.Append(aPB2);
  }
  else {
    BOPDS_ListOfPaveBlock aLPB(aAllocator);
    aLPB.Append(aPB2);
    aMPBLPB.Add(aPB1, aLPB);
  }
  if (aMPBLPB.Contains(aPB2)) {
    BOPDS_ListOfPaveBlock& aLPB=aMPBLPB.ChangeFromKey(aPB2);
    aLPB.Append(aPB1);
  }
  else {
    BOPDS_ListOfPaveBlock aLPB(aAllocator);
    aLPB.Append(aPB1);
    aMPBLPB.Add(aPB2, aLPB);
  }
}
//=======================================================================
//function : FillMap
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::FillMap(const Handle(BOPDS_PaveBlock)& aPB,
                              const Standard_Integer nF,
                              BOPDS_IndexedDataMapOfPaveBlockListOfInteger& aMPBLI,
                              Handle(NCollection_BaseAllocator)& aAllocator)
{
  if (aMPBLI.Contains(aPB)) {
    BOPCol_ListOfInteger& aLI=aMPBLI.ChangeFromKey(aPB);
    aLI.Append(nF);
  }
  else {
    BOPCol_ListOfInteger aLI(aAllocator);
    aLI.Append(nF);
    aMPBLI.Add(aPB, aLI);
  }
}
//=======================================================================
//function : MakeBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::MakeBlocks(const BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock& aMILI,
                                 BOPDS_DataMapOfIntegerListOfPaveBlock& aMBlocks,
                                 Handle(NCollection_BaseAllocator)& aAllocator)
{
  Standard_Integer aNbV,  aNbVS, aNbVP, aNbEC, k, i, j;
  BOPDS_ListIteratorOfListOfPaveBlock aItLI;
  //
  BOPDS_MapOfPaveBlock aMVS(100, aAllocator);
  BOPDS_IndexedMapOfPaveBlock aMEC(100, aAllocator);
  BOPDS_IndexedMapOfPaveBlock aMVP(100, aAllocator);
  BOPDS_IndexedMapOfPaveBlock aMVAdd(100, aAllocator);
  //
  aNbV=aMILI.Extent();
  //
  for (k=0, i=1; i<=aNbV; ++i) {
    aNbVS=aMVS.Extent();
    if (aNbVS==aNbV) {
      break;
    }
    //
    const Handle(BOPDS_PaveBlock)& nV=aMILI.FindKey(i);
    if (aMVS.Contains(nV)){
      continue;
    }
    aMVS.Add(nV);
    //
    aMEC.Clear();
    aMVP.Clear();
    aMVAdd.Clear();
    //
    aMVP.Add(nV);
    while(1) {
      aNbVP=aMVP.Extent();
      for (j=1; j<=aNbVP; ++j) {
        const Handle(BOPDS_PaveBlock)& nVP=aMVP(j);
        const BOPDS_ListOfPaveBlock& aLV=aMILI.FindFromKey(nVP);
        aItLI.Initialize(aLV);
        for (; aItLI.More(); aItLI.Next()) {
          const Handle(BOPDS_PaveBlock)& nVx=aItLI.Value();
          if (aMEC.Contains(nVx)) {
            continue;
          }
          //
          aMVS.Add(nVx);
          aMEC.Add(nVx);
          aMVAdd.Add(nVx);
        }
      }
      //
      aNbVP=aMVAdd.Extent();
      if (!aNbVP) {
        break; // from while(1)
      }
      //
      aMVP.Clear();
      for (j=1; j<=aNbVP; ++j) {
        aMVP.Add(aMVAdd(j));
      }
      aMVAdd.Clear();
    }//while(1) {
    //
    BOPDS_ListOfPaveBlock aLIx(aAllocator);
    //
    aNbEC = aMEC.Extent();
    for (j=1; j<=aNbEC; ++j) {
      const Handle(BOPDS_PaveBlock)& nVx=aMEC(j);
      aLIx.Append(nVx);
    }
    //
    aMBlocks.Bind(k, aLIx);
    ++k;
  }//for (k=0, i=1; i<=aNbV; ++i)
  aMVAdd.Clear();
  aMVP.Clear();
  aMEC.Clear();
  aMVS.Clear();
}
//=======================================================================
//function : PerformCommonBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::PerformCommonBlocks(BOPDS_IndexedDataMapOfPaveBlockListOfPaveBlock& aMPBLPB,
                                          Handle(NCollection_BaseAllocator)& aAllocator)
{
  Standard_Integer aNbCB;
  //
  aNbCB=aMPBLPB.Extent();
  if (!aNbCB) {
    return;
  }
  //
  Standard_Integer aNbPB, aNbBlocks, k;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  Handle(BOPDS_CommonBlock) aCB;
  BOPDS_DataMapOfIntegerListOfPaveBlock aMBlocks(100, aAllocator);
  //
  BOPAlgo_Tools::MakeBlocks(aMPBLPB, aMBlocks, aAllocator);
  //
  aNbBlocks = aMBlocks.Extent();
  for (k=0; k<aNbBlocks; ++k) {
    const BOPDS_ListOfPaveBlock& aLPB=aMBlocks.Find(k);
    aNbPB=aLPB.Extent();
    if (aNbPB>1) {
      aCB=new BOPDS_CommonBlock;
      //
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPBx=aItLPB.Value();
        aCB->AddPaveBlock(aPBx);
      }
      //
      aItLPB.Initialize(aLPB);
      for (; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPBx=aItLPB.Value();
        aPBx->SetCommonBlock(aCB);
      }
    }//if (aNbPB>1) {
  }
}
//=======================================================================
//function : PerformCommonBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::PerformCommonBlocks(const BOPDS_IndexedDataMapOfPaveBlockListOfInteger& aMPBLI,
                                          Handle(NCollection_BaseAllocator)& )//aAllocator)
{
  Standard_Integer nF, i, aNb;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  Handle(BOPDS_PaveBlock) aPB;
  Handle(BOPDS_CommonBlock) aCB;
  //
  aNb=aMPBLI.Extent();
  for (i=1; i<=aNb; ++i) {
    aPB=aMPBLI.FindKey(i);
    if (aPB->IsCommonBlock()) {
      aCB=aPB->CommonBlock();
    }
    else {
      aCB=new BOPDS_CommonBlock;
      aCB->AddPaveBlock(aPB);
    }
    //
    const BOPCol_ListOfInteger& aLI=aMPBLI.FindFromKey(aPB);
    aItLI.Initialize(aLI);
    for (; aItLI.More(); aItLI.Next()) {
      nF=aItLI.Value();
      aCB->AddFace(nF);
    }
    aPB->SetCommonBlock(aCB);
  }
}

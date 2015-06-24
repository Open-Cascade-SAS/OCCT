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


#include <BOPAlgo_Tools.hxx>
#include <BOPCol_IndexedMapOfInteger.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_DataMapOfPaveBlockListOfPaveBlock.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_IndexedMapOfPaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>

//=======================================================================
//function : MakeBlocksCnx
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::MakeBlocksCnx(const BOPCol_IndexedDataMapOfIntegerListOfInteger& aMILI,
                                    BOPCol_DataMapOfIntegerListOfInteger& aMBlocks,
                                    const Handle(NCollection_BaseAllocator)& aAllocator)
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
    for(;;) {
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
                              const Handle(NCollection_BaseAllocator)& aAllocator)
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
                              const Handle(NCollection_BaseAllocator)& aAllocator)
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
                              const Handle(NCollection_BaseAllocator)& aAllocator)
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
                                 const Handle(NCollection_BaseAllocator)& aAllocator)
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
    for(;;) {
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
                                          const Handle(NCollection_BaseAllocator)& aAllocator,
                                          BOPDS_PDS& pDS)
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
        pDS->SetCommonBlock(aPBx, aCB);
      }
    }//if (aNbPB>1) {
  }
}
//=======================================================================
//function : PerformCommonBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_Tools::PerformCommonBlocks(const BOPDS_IndexedDataMapOfPaveBlockListOfInteger& aMPBLI,
                                          const Handle(NCollection_BaseAllocator)& ,//aAllocator
                                          BOPDS_PDS& pDS)
{
  Standard_Integer nF, i, aNb;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  Handle(BOPDS_PaveBlock) aPB;
  Handle(BOPDS_CommonBlock) aCB;
  //
  aNb=aMPBLI.Extent();
  for (i=1; i<=aNb; ++i) {
    aPB=aMPBLI.FindKey(i);
    if (pDS->IsCommonBlock(aPB)) {
      aCB=pDS->CommonBlock(aPB);
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
    pDS->SetCommonBlock(aPB, aCB);
  }
}

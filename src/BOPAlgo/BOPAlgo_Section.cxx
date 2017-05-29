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


#include <BOPAlgo_BuilderSolid.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_Section.hxx>
#include <BOPAlgo_Alerts.hxx>
#include <BOPCol_DataMapOfShapeShape.hxx>
#include <BOPCol_IndexedDataMapOfShapeListOfShape.hxx>
#include <BOPCol_IndexedMapOfShape.hxx>
#include <BOPCol_ListOfShape.hxx>
#include <BOPCol_MapOfShape.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_VectorOfFaceInfo.hxx>
#include <BOPDS_VectorOfListOfPaveBlock.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//
//
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_Section::BOPAlgo_Section()
:
  BOPAlgo_Builder()
{
  Clear();
}
//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BOPAlgo_Section::BOPAlgo_Section
  (const Handle(NCollection_BaseAllocator)& theAllocator)
:
  BOPAlgo_Builder(theAllocator)
{
  Clear();
}
//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
BOPAlgo_Section::~BOPAlgo_Section()
{
}
//=======================================================================
//function : CheckData
//purpose  : 
//=======================================================================
void BOPAlgo_Section::CheckData()
{
  Standard_Integer aNbArgs;
  //
  aNbArgs=myArguments.Extent();
  if (!aNbArgs) {
    AddError (new BOPAlgo_AlertTooFewArguments);
    return;
  }
  //
  CheckFiller();
}
//=======================================================================
//function : PerformInternal1
//purpose  : 
//=======================================================================
void BOPAlgo_Section::PerformInternal1
  (const BOPAlgo_PaveFiller& theFiller)
{
  myPaveFiller=(BOPAlgo_PaveFiller*)&theFiller;
  myDS=myPaveFiller->PDS();
  myContext=myPaveFiller->Context();
  //
  // 1. CheckData
  CheckData();
  if (HasErrors()) {
    return;
  }
  //
  // 2. Prepare
  Prepare();
  if (HasErrors()) {
    return;
  }
  //
  // 3. Fill Images
  // 3.1 Vertices
  FillImagesVertices();
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_VERTEX);
  if (HasErrors()) {
    return;
  }
  // 3.2 Edges
  FillImagesEdges();
  if (HasErrors()) {
    return;
  }
  //
  BuildResult(TopAbs_EDGE);
  if (HasErrors()) {
    return;
  }
  // 4. Section
  BuildSection();
  //
  if (HasErrors()) {
    return;
  }
  // 5.History
  PrepareHistory();
  //
  if (HasErrors()) {
    return;
  } 
  // 6. Post-treatment
  PostTreat();
}
//=======================================================================
//function : BuildSection
//purpose  : 
//=======================================================================
void BOPAlgo_Section::BuildSection()
{
  Standard_Integer i, aNbMS, aNbLE;
  Standard_Integer j,  nE, nV, aNb, aNbF, aNbPBSc;
  TopoDS_Shape aRC, aRC1;
  BRep_Builder aBB;
  TopExp_Explorer aExp;
  BOPCol_ListOfShape aLSA, aLS;
  BOPCol_ListIteratorOfListOfShape aIt, aItIm, aItLS;
  BOPCol_IndexedDataMapOfShapeInteger aMSI(100, myAllocator);
  BOPCol_IndexedMapOfShape aMS(100, myAllocator);
  BOPCol_MapOfShape aMFence(100, myAllocator);
  BOPCol_MapIteratorOfMapOfInteger aItMI; 
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  //
  GetReport()->Clear();
  //
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aRC1);
  //
  // 1. aRC1
  aNb=myDS->NbSourceShapes();
  for (i=0; i<aNb; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    if (aSI.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    //
    const BOPDS_FaceInfo& aFI=myDS->FaceInfo(i);
    //
    // 1.1 Vertices that are section vertices 
    const BOPCol_MapOfInteger& aMVSc=aFI.VerticesSc();
    aItMI.Initialize(aMVSc);
    for(; aItMI.More(); aItMI.Next()) {
      nV=aItMI.Key();
      const TopoDS_Shape& aV=myDS->Shape(nV);
      aBB.Add(aRC1, aV);
    }
    //
    // 1.2 Vertices that are in a face 
    const BOPCol_MapOfInteger& aMI=aFI.VerticesIn();
    aItMI.Initialize(aMI);
    for(; aItMI.More(); aItMI.Next()) {
      nV=aItMI.Key();
      if (nV<0) {
        continue;
      }  
      if (myDS->IsNewShape(nV) || myDS->HasInterf(nV)) { 
        const TopoDS_Shape& aV=myDS->Shape(nV);
        aBB.Add(aRC1, aV);
      }
    }
    //
    // 1.3 Section edges
    const BOPDS_IndexedMapOfPaveBlock& aMPBSc=aFI.PaveBlocksSc();
    //
    aNbPBSc=aMPBSc.Extent();
    for (j=1; j<=aNbPBSc; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB=aMPBSc(j);
      nE=aPB->Edge();
      const TopoDS_Shape& aE=myDS->Shape(nE);
      aBB.Add(aRC1, aE);
    }
  }
  //
  // 2. Common blocks between an edge and a face
  const BOPDS_VectorOfListOfPaveBlock& aPBP=myDS->PaveBlocksPool();
  //
  aNb=aPBP.Size();
  for (i=0; i<aNb; ++i) {
    const BOPDS_ListOfPaveBlock& aLPB=aPBP(i); 
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB=aItPB.Value();
      Handle(BOPDS_CommonBlock) aCB=myDS->CommonBlock(aPB);
      if (!aCB.IsNull()) {
        const BOPCol_ListOfInteger& aLF=aCB->Faces();
        aNbF=aLF.Extent();
        if (aNbF) {
          const Handle(BOPDS_PaveBlock)& aPBR=aCB->PaveBlock1();
          nE=aPBR->Edge();
          const TopoDS_Shape& aE=myDS->Shape(nE);
          aBB.Add(aRC1, aE);
        }
      }
    }
  }
  //
  aIt.Initialize(myArguments);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSA=aIt.Value();
    if (aMFence.Add(aSA)) {
      aLSA.Append(aSA);
    }
  }
  //
  aMFence.Clear();
  //
  // 3. Treatment boundaries of arguments
  //
  // 3.1 Set to treat => aLS
  aIt.Initialize(aLSA);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSA=aIt.Value();
    //
    aLS.Clear();
    aMS.Clear();
    aMFence.Clear();
    //
    aExp.Init (aSA, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aE=aExp.Current();
      if (aMFence.Add(aE)) {
        aLS.Append(aE);
      }
    }
    aExp.Init (aSA, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Shape& aE=aExp.Current();
      if (aMFence.Add(aE)) {
        aLS.Append(aE);
      }
    }
    //
    // 3.2 aMSI
    aItLS.Initialize(aLS);
    for (; aItLS.More(); aItLS.Next()) { 
      const TopoDS_Shape& aS=aItLS.Value();
      //
      if (myImages.IsBound(aS)){
        const BOPCol_ListOfShape& aLSIm=myImages.Find(aS);
        aItIm.Initialize(aLSIm);
        for (; aItIm.More(); aItIm.Next()) {
          const TopoDS_Shape& aSIm=aItIm.Value();
          BOPTools::MapShapes(aSIm, TopAbs_VERTEX, aMS);
          BOPTools::MapShapes(aSIm, TopAbs_EDGE  , aMS);
        }
      }// if (myImages.IsBound(aF)){
      else {
        BOPTools::MapShapes(aS, TopAbs_VERTEX, aMS);
        BOPTools::MapShapes(aS, TopAbs_EDGE  , aMS);
      }
    }//for (; aItLS.More(); aItLS.Next()) { 
    //
    aNbMS=aMS.Extent();
    for (i=1; i<=aNbMS; ++i) {
      const TopoDS_Shape& aS=aMS(i);
      if (aMSI.Contains(aS)) {
        Standard_Integer& iCnt=aMSI.ChangeFromKey(aS);
        ++iCnt;
      }
      else {
        aMSI.Add(aS, 1);
      }
    }
  } //for (; aIt.More(); aIt.Next()) {
  //
  aMS.Clear();
  aMFence.Clear();
  //
  // 4. Build the result
  BOPCol_IndexedDataMapOfShapeListOfShape aMVE(100, myAllocator);
  // 
  BOPTools::MapShapesAndAncestors(aRC1, 
                                  TopAbs_VERTEX, 
                                  TopAbs_EDGE, 
                                  aMVE);
  //
  aNbMS=aMSI.Extent();
  for (i=1; i<=aNbMS; ++i) {
    const TopoDS_Shape& aV=aMSI.FindKey(i);
    const Standard_Integer& iCnt=aMSI.FindFromIndex(i);
    if (iCnt>1) {
      BOPTools::MapShapesAndAncestors(aV, 
                                      TopAbs_VERTEX, 
                                      TopAbs_EDGE, 
                                      aMVE);
    }
  }
  //
  BOPTools_AlgoTools::MakeContainer(TopAbs_COMPOUND, aRC);
  //
  aNbMS=aMVE.Extent();
  for (i=1; i<=aNbMS; ++i) {
    const TopoDS_Shape& aV=aMVE.FindKey(i);
    const BOPCol_ListOfShape& aLE=aMVE.FindFromIndex(i);
    aNbLE=aLE.Extent();
    if (!aNbLE) {
      // alone vertices
      if (aMFence.Add(aV)) {
        aBB.Add(aRC, aV); 
      }
    }
    else {
      // edges 
      aIt.Initialize(aLE);
      for (; aIt.More(); aIt.Next()) {
        const TopoDS_Shape& aE=aIt.Value();
        if (aMFence.Add(aE)) {
          aBB.Add(aRC, aE);
        }
      }
    }
  }
  //
  myShape=aRC;
}
//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================
const TopTools_ListOfShape& BOPAlgo_Section::Generated
  (const TopoDS_Shape& theS)
{
  myHistShapes.Clear();
  if (theS.IsNull()) {
    return myHistShapes;
  }
  //
  TopAbs_ShapeEnum aType = theS.ShapeType();
  if (aType != TopAbs_FACE) {
    return myHistShapes;
  }
  //
  Standard_Integer nS = myDS->Index(theS);
  if (nS < 0) {
    return myHistShapes;
  }
  //
  if (!myDS->HasFaceInfo(nS)) {
    return myHistShapes;
  }
  //
  //collect section edges of the face theS
  Standard_Integer i, aNb, nSp;
  //
  const BOPDS_FaceInfo& aFI = myDS->FaceInfo(nS);
  const BOPDS_IndexedMapOfPaveBlock& aMPBSc = aFI.PaveBlocksSc();
  aNb = aMPBSc.Extent();
  for (i = 1; i <= aNb; ++i) {
    const Handle(BOPDS_PaveBlock)& aPB = aMPBSc(i);
    nSp = aPB->Edge();
    const TopoDS_Shape& aSp = myDS->Shape(nSp);
    myHistShapes.Append(aSp);
  }
  //
  return myHistShapes;
}

// Created on: 2012-06-01
// Created by: Eugeny MALTCHIKOV
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


#include <BOPAlgo_BuilderFace.hxx>
#include <BOPCol_DataMapOfShapeListOfShape.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_ListOfPave.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPTools.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_MapOfSet.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepFeat_Builder.hxx>
#include <Geom_Curve.hxx>
#include <IntTools_Tools.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
  BRepFeat_Builder::BRepFeat_Builder()
:
  BOPAlgo_BOP()
{
  Clear();
}

//=======================================================================
//function : ~
//purpose  : 
//=======================================================================
  BRepFeat_Builder::~BRepFeat_Builder()
{
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::Clear()
{
  myShapes.Clear();
  myRemoved.Clear();
  BOPAlgo_BOP::Clear();
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::Init(const TopoDS_Shape& theShape)
{
  Clear();
  //
  AddArgument(theShape);
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::Init(const TopoDS_Shape& theShape,
                              const TopoDS_Shape& theTool)
{
  Clear();
  //
  AddArgument(theShape);
  AddTool(theTool);
}

//=======================================================================
//function : SetOperation
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::SetOperation(const Standard_Integer theFuse)
{
  myFuse = theFuse;
  myOperation = myFuse ? BOPAlgo_FUSE : BOPAlgo_CUT;
}

//=======================================================================
//function : SetOperation
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::SetOperation(const Standard_Integer theFuse,
                                      const Standard_Boolean theFlag)
{
  myFuse = theFuse;
  if (!theFlag) {
    myOperation = myFuse ? BOPAlgo_FUSE : BOPAlgo_CUT;
  } else {  
    myOperation = myFuse ? BOPAlgo_CUT21 : BOPAlgo_COMMON;
  }
}

//=======================================================================
//function : PartsOfTool
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::PartsOfTool(TopTools_ListOfShape& aLT)
{
  TopExp_Explorer aExp;
  //
  aLT.Clear();
  aExp.Init(myShape, TopAbs_SOLID);
  for (;aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aS = aExp.Current();
    aLT.Append(aS);
  }
}

//=======================================================================
//function : KeepPartsOfTool
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::KeepParts(const TopTools_ListOfShape& theIm)
{
  TopTools_ListIteratorOfListOfShape aItT;
  aItT.Initialize(theIm);
  for (; aItT.More(); aItT.Next()) {
    const TopoDS_Shape& aTIm=aItT.Value();
    KeepPart(aTIm);
  }
}

//=======================================================================
//function : KeepPart
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::KeepPart(const TopoDS_Shape& thePart)
{
  TopoDS_Shape aF, aFOr;
  TopExp_Explorer aExp;
  //
  BOPTools::MapShapes(thePart, myShapes);
}

//=======================================================================
//function : Prepare
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::Prepare()
{
  GetReport()->Clear();
  //
  BRep_Builder aBB;
  TopoDS_Compound aC;
  aBB.MakeCompound(aC);
  myShape=aC;
  //
  myFlagHistory=Standard_True;
  //
  FillRemoved();
}

//=======================================================================
//function : FillRemoved
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::FillRemoved()
{
  TopExp_Explorer aExp;
  //
  const TopoDS_Shape& aArgs0=myArguments.First();
  const TopoDS_Shape& aArgs1=myTools.First();
  //
  aExp.Init(aArgs0, TopAbs_SOLID);
  for (; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aS = aExp.Current();
    myImages.UnBind(aS);
  }
  //
  if (!myImages.IsBound(aArgs1)) {
    return;
  }
  //
  BOPCol_ListIteratorOfListOfShape aItIm;
  //
  BOPCol_ListOfShape& aLS = myImages.ChangeFind(aArgs1);
  aItIm.Initialize(aLS);
  for (; aItIm.More(); aItIm.Next()) {
    const TopoDS_Shape& aS = aItIm.Value();
    FillRemoved(aS, myRemoved);
  }
}

//=======================================================================
//function : PerformResult
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::PerformResult()
{
  myOperation = myFuse ? BOPAlgo_FUSE : BOPAlgo_CUT;
  //
  if (!myShapes.IsEmpty()) {
    //
    Prepare(); 
    //
    RebuildFaces();
    //
    FillImagesContainers(TopAbs_SHELL);
    if (HasErrors()) {
      return;
    }
    //
    FillImagesSolids();
    if (HasErrors()) {
      return;
    }
    //
    CheckSolidImages();
    //
    BuildResult(TopAbs_SOLID);
    if (HasErrors()) {
      return;
    }
    // 
    FillImagesCompounds();
    if (HasErrors()) {
      return;
    }
    //
    BuildResult(TopAbs_COMPOUND);
    if (HasErrors()) {
      return;
    }
  }
  //
  BuildShape();
}

//=======================================================================
//function : RebuildFaces
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::RebuildFaces()
{
  Standard_Integer aNbS, i, iRank, nSp, j;
  Standard_Boolean bIsClosed, bIsDegenerated, bToReverse,
                   bRem, bIm, bFlagSD, bVInShapes;
  TopAbs_Orientation anOriF, anOriE;
  TopoDS_Face aFF, aFSD;
  TopoDS_Edge aSp;
  TopoDS_Shape aSx;
  TopExp_Explorer aExp, aExpE;
  BOPCol_MapOfShape aME, aMESplit;
  BOPCol_ListIteratorOfListOfShape aItIm;
  BOPDS_MapIteratorOfMapOfPaveBlock aItMPB;
  BOPCol_MapIteratorOfMapOfShape aItM;
  BOPTools_MapOfSet aMST;
  BOPCol_ListOfShape aLE;
  //
  aItM.Initialize(myShapes);
  for (; aItM.More(); aItM.Next()) {
    const TopoDS_Shape& aS = aItM.Value();
    if (aS.ShapeType() == TopAbs_FACE) {
      BOPTools_Set aST;
      aST.Add(aS, TopAbs_EDGE);
      aMST.Add(aST);
    }
  }
  //
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    //
    iRank = myDS->Rank(i);
    if (iRank == 1) {
      const TopoDS_Shape& aS = aSI.Shape();
      //
      if (myImages.IsBound(aS)) {
        BOPCol_ListOfShape& aLIm = myImages.ChangeFind(aS);
        aItIm.Initialize(aLIm);
        for (; aItIm.More(); ) {
          const TopoDS_Shape& aSIm = aItIm.Value();
          if (!myShapes.Contains(aSIm)) {
            aLIm.Remove(aItIm);
            continue;
          }
          aItIm.Next();
        }
      }
      continue;
    }
    //
    if (aSI.ShapeType()!=TopAbs_FACE) {
      continue;
    }
    //
    const BOPDS_FaceInfo& aFI=myDS->FaceInfo(i);
    const TopoDS_Face& aF=(*(TopoDS_Face*)(&aSI.Shape()));
    //
    if (!myImages.IsBound(aF)) {
      continue;
    }
    //
    anOriF=aF.Orientation();
    aFF=aF;
    aFF.Orientation(TopAbs_FORWARD);

    const BOPDS_IndexedMapOfPaveBlock& aMPBIn=aFI.PaveBlocksIn();
    const BOPDS_IndexedMapOfPaveBlock& aMPBSc=aFI.PaveBlocksSc();

    aLE.Clear();

    //bounding edges
    aExp.Init(aFF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      const TopoDS_Edge& aE=(*(TopoDS_Edge*)(&aExp.Current()));
      anOriE=aE.Orientation();
      bIsDegenerated=BRep_Tool::Degenerated(aE);
      bIsClosed=BRep_Tool::IsClosed(aE, aF);
      if (myImages.IsBound(aE)) {
        BOPCol_ListOfShape& aLEIm = myImages.ChangeFind(aE);
        //
        bRem = Standard_False;
        bIm = Standard_False;
        aME.Clear();
        BOPCol_ListOfShape aLEImNew;
        //
        aItIm.Initialize(aLEIm);
        for (; aItIm.More(); aItIm.Next()) {
          const TopoDS_Shape& aS = aItIm.Value();

          bVInShapes = Standard_False;
          if (myShapes.Contains(aS)) {
            bVInShapes = Standard_True;
          } else {
            aExpE.Init(aS, TopAbs_VERTEX);
            for(;aExpE.More(); aExpE.Next()) {
              const TopoDS_Shape& aV = aExpE.Current();
              if (myShapes.Contains(aV)) {
                bVInShapes = Standard_True;
                break;
              }
            }
          }
          //
          if (bVInShapes) {
            bIm = Standard_True;
            aLEImNew.Append(aS);
          } else {
            bRem = Standard_True;
            aME.Add(aS);
          }
        }
        //
        if (!bIm) {
          aLE.Append(aE);
          continue;
        }
        //
        if (bRem && bIm) {
          if (aLEIm.Extent() == 2) {
            aLE.Append(aE);
            continue;
          }
          if (aMESplit.Add(aE)) {
            RebuildEdge(aE, aFF, aME, aLEImNew);
            aLEIm.Assign(aLEImNew);
            if (aLEIm.Extent() == 1) {
              aLE.Append(aE);
              continue;
            }
          }
        }
        //
        aItIm.Initialize(aLEIm);
        for (; aItIm.More(); aItIm.Next()) {
          aSp = *(TopoDS_Edge*)&aItIm.Value();

          if (bIsDegenerated) {
            aSp.Orientation(anOriE);
            aLE.Append(aSp);
            continue;
          }
          //
          if (anOriE==TopAbs_INTERNAL) {
            aSp.Orientation(TopAbs_FORWARD);
            aLE.Append(aSp);
            aSp.Orientation(TopAbs_REVERSED);
            aLE.Append(aSp);
            continue;
          }
          //
          if (bIsClosed) {
            if (!BRep_Tool::IsClosed(aSp, aFF)){
              BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSp, aFF);
            }
            //
            aSp.Orientation(TopAbs_FORWARD);
            aLE.Append(aSp);
            aSp.Orientation(TopAbs_REVERSED);
            aLE.Append(aSp);
            continue;
          }// if (bIsClosed){
          //
          aSp.Orientation(anOriE);
          bToReverse=BOPTools_AlgoTools::IsSplitToReverse(aSp, aE, myContext);
          if (bToReverse) {
            aSp.Reverse();
          }
          aLE.Append(aSp);
        }
      }
      else {
        aLE.Append(aE);
      }
    }

    Standard_Integer aNbPBIn, aNbPBSc;
    aNbPBIn = aMPBIn.Extent();
    aNbPBSc = aMPBSc.Extent();
    //
    //in edges
    for (j=1; j<=aNbPBIn; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB=aMPBIn(j);
      nSp=aPB->Edge();
      aSp=(*(TopoDS_Edge*)(&myDS->Shape(nSp)));
      if (myRemoved.Contains(aSp)) {
        continue;
      }
      //
      aSp.Orientation(TopAbs_FORWARD);
      aLE.Append(aSp);
      aSp.Orientation(TopAbs_REVERSED);
      aLE.Append(aSp);
    }
    //section edges
    for (j=1; j<=aNbPBSc; ++j) {
      const Handle(BOPDS_PaveBlock)& aPB=aMPBSc(j);
      nSp=aPB->Edge();
      aSp=(*(TopoDS_Edge*)(&myDS->Shape(nSp)));
      if (myRemoved.Contains(aSp)) {
        continue;
      }
      //
      aSp.Orientation(TopAbs_FORWARD);
      aLE.Append(aSp);
      aSp.Orientation(TopAbs_REVERSED);
      aLE.Append(aSp);
    }
    
    //build new faces
    BOPAlgo_BuilderFace aBF;
    aBF.SetFace(aFF);
    aBF.SetShapes(aLE);
    
    aBF.Perform();

    BOPCol_ListOfShape& aLFIm = myImages.ChangeFind(aF);
    aLFIm.Clear();

    const BOPCol_ListOfShape& aLFR=aBF.Areas();
    aItIm.Initialize(aLFR);
    for (; aItIm.More(); aItIm.Next()) {
      TopoDS_Shape& aFR=aItIm.ChangeValue();
      //
      BOPTools_Set aST;
      aST.Add(aFR, TopAbs_EDGE);
      bFlagSD=aMST.Contains(aST);
      //
      const BOPTools_Set& aSTx=aMST.Added(aST);
      aSx=aSTx.Shape();
      aSx.Orientation(anOriF);
      aLFIm.Append(aSx);
      //
      BOPCol_ListOfShape* pLOr = myOrigins.ChangeSeek(aSx);
      if (!pLOr) {
        pLOr = myOrigins.Bound(aSx, BOPCol_ListOfShape());
      }
      pLOr->Append(aF);
      //
      if (bFlagSD) {
        myShapesSD.Bind(aFR, aSx);
      }
    }
    //
    mySplits.Bind(aF, aLFIm); 
    if (aLFIm.Extent() == 0) {
      mySplits.UnBind(aF);
      myImages.UnBind(aF);
    }
  }
}

//=======================================================================
//function : RebuildEdge
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::RebuildEdge(const TopoDS_Shape& theE,
                                     const TopoDS_Face& theF,
                                     const BOPCol_MapOfShape& aME,
                                     BOPCol_ListOfShape& aLIm)
{
  Standard_Integer nE, nSp, nV1, nV2, nE1, nV, nVx, nVSD;
  Standard_Integer nV11, nV21;
  Standard_Boolean bOld;
  Standard_Real aT11, aT21;
  Standard_Real aT1, aT2;
  TopoDS_Edge aSp, aE;
  BOPDS_ShapeInfo aSI;
  TopoDS_Vertex aV1, aV2;
  Handle(BOPDS_PaveBlock) aPBNew;
  BOPCol_MapOfInteger aMI, aMAdd, aMV, aMVOr;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB;
  BOPCol_ListIteratorOfListOfShape aIt;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  BOPCol_MapIteratorOfMapOfShape aItM;
  BOPDS_MapOfPaveBlock aMPB;
  BOPDS_MapIteratorOfMapOfPaveBlock aItMPB;
  //
  aSI.SetShapeType(TopAbs_EDGE);

  //1. collect origin vertices to aMV map.
  nE = myDS->Index(theE);
  const BOPDS_ShapeInfo& aSIE = myDS->ShapeInfo(nE);
  const BOPCol_ListOfInteger& aLS = aSIE.SubShapes();
  aItLI.Initialize(aLS);
  for(; aItLI.More(); aItLI.Next()) {
    nV = aItLI.Value();
    nVx=nV;
    if (myDS->HasShapeSD(nV, nVSD)) {
      nVx=nVSD;
    }
    aMV.Add(nVx);
    aMVOr.Add(nVx);
  }
  //
  //2. collect vertices that should be removed to aMI map.
  aPBNew = new BOPDS_PaveBlock;
  BOPDS_ListOfPave& aLPExt = aPBNew->ChangeExtPaves();
  BOPDS_ListOfPaveBlock& aLPB = myDS->ChangePaveBlocks(nE);
  //
  for (aItPB.Initialize(aLPB); aItPB.More(); aItPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
    nE1 = aPB->Edge();
    const TopoDS_Shape& aE1 = myDS->Shape(nE1);
    //
    if (aME.Contains(aE1)) {
      aPB->Indices(nV1, nV2);
      aMI.Add(nV1);
      aMI.Add(nV2);
    }
    else {
      aMPB.Add(aPB);
    }
  }
  //3. collect vertices that split the source shape.
  for (aItPB.Initialize(aLPB); aItPB.More(); aItPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
    aPB->Indices(nV1, nV2);
    //
    if (!aMI.Contains(nV1)) {
      aMV.Add(nV1);
    }
    if (!aMI.Contains(nV2)) {
      aMV.Add(nV2);
    }
  }
  //4. collect ext paves.
  for (aItPB.Initialize(aLPB); aItPB.More(); aItPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB = aItPB.Value();
    aPB->Indices(nV1, nV2);
    //
    if (aMV.Contains(nV1)) {
      if (aMAdd.Add(nV1) || aMVOr.Contains(nV1)) {
        aLPExt.Append(aPB->Pave1());
      }
    }
    //
    if (aMV.Contains(nV2)) {
      if (aMAdd.Add(nV2) || aMVOr.Contains(nV2)) {
        aLPExt.Append(aPB->Pave2());
      }
    }      
  }

  aE = (*(TopoDS_Edge *)(&theE));
  aE.Orientation(TopAbs_FORWARD);
  //
  aLIm.Clear();
  //
  //5. split edge by new set of vertices.
  aLPB.Clear();
  aPBNew->SetOriginalEdge(nE);
  aPBNew->Update(aLPB, Standard_False);
  //
  for (aItPB.Initialize(aLPB); aItPB.More(); aItPB.Next()) {
    Handle(BOPDS_PaveBlock)& aPB = aItPB.ChangeValue();
    const BOPDS_Pave& aPave1=aPB->Pave1();
    aPave1.Contents(nV1, aT1);
    //
    const BOPDS_Pave& aPave2=aPB->Pave2();
    aPave2.Contents(nV2, aT2);
    //
    aItMPB.Initialize(aMPB);
    //check if it is the old pave block.
    bOld = Standard_False;
    for (; aItMPB.More(); aItMPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB1 = aItMPB.Value();
      aPB1->Indices(nV11, nV21);
      aPB1->Range(aT11, aT21);
      if (nV1 == nV11 && nV2 == nV21 &&
          aT1 == aT11 && aT2 == aT21) {
        const TopoDS_Shape& aEIm = myDS->Shape(aPB1->Edge());
        aLIm.Append(aEIm);
        bOld = Standard_True;
        break;
      }
    }
    if (bOld) {
      continue;
    }
    //
    aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
    aV1.Orientation(TopAbs_FORWARD); 
    //
    aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
    aV2.Orientation(TopAbs_REVERSED); 
    //
    BOPTools_AlgoTools::MakeSplitEdge(aE, aV1, aT1, aV2, aT2, aSp);
    BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aSp, theF, myContext);
    //
    aSI.SetShape(aSp);
    //
    Bnd_Box& aBox=aSI.ChangeBox();
    BRepBndLib::Add(aSp, aBox);
    //
    nSp=myDS->Append(aSI);
    //
    aPB->SetEdge(nSp);
    aLIm.Append(aSp);
  }
}

//=======================================================================
//function : CheckSolidImages
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::CheckSolidImages()
{
  BOPTools_MapOfSet aMST;
  BOPCol_ListOfShape aLSImNew;
  BOPCol_MapOfShape aMS;
  BOPCol_ListIteratorOfListOfShape aIt;
  TopExp_Explorer aExp, aExpF;
  Standard_Boolean bFlagSD;
  // 
  const TopoDS_Shape& aArgs0=myArguments.First();
  const TopoDS_Shape& aArgs1=myTools.First();
  //
  const BOPCol_ListOfShape& aLSIm = myImages.Find(aArgs1);
  aIt.Initialize(aLSIm);
  for(;aIt.More();aIt.Next()) {
    const TopoDS_Shape& aSolIm = aIt.Value();
    //
    BOPTools_Set aST;
    aST.Add(aSolIm, TopAbs_FACE);
    aMST.Add(aST);
  }
  //
  aExp.Init(aArgs0, TopAbs_SOLID);
  for(; aExp.More(); aExp.Next()) {
    const TopoDS_Shape& aSolid = aExp.Current();
    if (myImages.IsBound(aSolid)) {
      BOPCol_ListOfShape& aLSImSol = myImages.ChangeFind(aSolid);
      aIt.Initialize(aLSImSol);
      for(;aIt.More();aIt.Next()) {
        const TopoDS_Shape& aSolIm = aIt.Value();
        //
        BOPTools_Set aST;
        aST.Add(aSolIm, TopAbs_FACE);
        bFlagSD=aMST.Contains(aST);
        //
        const BOPTools_Set& aSTx=aMST.Added(aST);
        const TopoDS_Shape& aSx=aSTx.Shape();
        aLSImNew.Append(aSx);
        //
        if (bFlagSD) {
          myShapesSD.Bind(aSolIm, aSx);
        }
      }
      aLSImSol.Assign(aLSImNew);
    }
  }
}

//=======================================================================
//function : MapShapes
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::FillRemoved(const TopoDS_Shape& S,  
                                     BOPCol_MapOfShape& M)
{
  if (myShapes.Contains(S)) {
    return;
  }
  //
  M.Add(S);
  TopoDS_Iterator It(S);
  while (It.More()) {
    FillRemoved(It.Value(),M);
    It.Next();
  }
}

//=======================================================================
//function : FillIn3DParts
//purpose  : 
//=======================================================================
  void BRepFeat_Builder::FillIn3DParts(BOPCol_DataMapOfShapeListOfShape& theInParts,
                                       BOPCol_DataMapOfShapeShape& theDraftSolids,
                                       const Handle(NCollection_BaseAllocator)& theAllocator)
{
  GetReport()->Clear();
  //
  Standard_Boolean bIsIN, bHasImage;
  Standard_Integer aNbS, i, j, aNbFP, aNbFPx, aNbFIN, aNbLIF, aNbEFP;
  TopAbs_ShapeEnum aType;  
  TopAbs_State aState;
  TopoDS_Iterator aIt, aItF;
  BRep_Builder aBB;
  TopoDS_Solid aSolidSp; 
  TopoDS_Face aFP;
  BOPCol_ListIteratorOfListOfShape aItS, aItFP, aItEx;	
  BOPCol_MapIteratorOfMapOfShape aItMS, aItMS1;
  //
  BOPCol_ListOfShape aLIF(theAllocator);
  BOPCol_MapOfShape aMFDone(100, theAllocator);
  BOPCol_MapOfShape aMSolids(100, theAllocator);
  BOPCol_MapOfShape aMFaces(100, theAllocator);
  BOPCol_MapOfShape aMFIN(100, theAllocator);
  BOPCol_IndexedMapOfShape aMS(100, theAllocator);
  BOPCol_IndexedDataMapOfShapeListOfShape aMEF(100, theAllocator);
  //
  theDraftSolids.Clear();
  //
  aNbS=myDS->NbSourceShapes();
  for (i=0; i<aNbS; ++i) {
    const BOPDS_ShapeInfo& aSI=myDS->ShapeInfo(i);
    const TopoDS_Shape& aS=aSI.Shape();
    //
    aType=aSI.ShapeType();
    switch(aType) {
      case TopAbs_SOLID: {
        aMSolids.Add(aS);
        break;
      }
      //
      case TopAbs_FACE: {
        // all faces (originals or images)
        if (myImages.IsBound(aS)) {
          const BOPCol_ListOfShape& aLS=myImages.Find(aS);
          aItS.Initialize(aLS);
          for (; aItS.More(); aItS.Next()) {
            const TopoDS_Shape& aFx=aItS.Value();
            if (!myRemoved.Contains(aFx)) {
              aMFaces.Add(aFx);
            }
          }
        }
        else {
          if (!myRemoved.Contains(aS)) {
            aMFaces.Add(aS);
          }
        }
        break;
      }
      //
      default:
        break;
    }
  }
  //
  aItMS.Initialize(aMSolids);
  for (; aItMS.More(); aItMS.Next()) {
    const TopoDS_Solid& aSolid=(*(TopoDS_Solid*)(&aItMS.Value()));
    //
    aMFDone.Clear();
    aMFIN.Clear();
    aMEF.Clear();
    //
    aBB.MakeSolid(aSolidSp);
    // 
    // Draft solid and its pure internal faces => aSolidSp, aLIF
    aLIF.Clear();
    BuildDraftSolid(aSolid, aSolidSp, aLIF);
    aNbLIF=aLIF.Extent();
    //
    // 1 all faces/edges from aSolid [ aMS ]
    bHasImage=Standard_False;
    aMS.Clear();
    aIt.Initialize(aSolid);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aShell=aIt.Value();
      //
      if (myImages.IsBound(aShell)) {
        bHasImage=Standard_True;
        //
        const BOPCol_ListOfShape& aLS=myImages.Find(aShell);
        aItS.Initialize(aLS);
        for (; aItS.More(); aItS.Next()) {
          const TopoDS_Shape& aSx=aItS.Value();
          aMS.Add(aSx);
          BOPTools::MapShapes(aSx, TopAbs_FACE, aMS);
          BOPTools::MapShapes(aSx, TopAbs_EDGE, aMS);
          BOPTools::MapShapesAndAncestors(aSx, TopAbs_EDGE, TopAbs_FACE, aMEF);
        }
      }
      else {
        //aMS.Add(aShell);
        BOPTools::MapShapes(aShell, TopAbs_FACE, aMS);
        BOPTools::MapShapesAndAncestors(aShell, TopAbs_EDGE, TopAbs_FACE, aMEF);
      }
    }
    //
    // 2 all faces that are not from aSolid [ aLFP1 ]
    BOPCol_IndexedDataMapOfShapeListOfShape aMEFP(100, theAllocator);
    BOPCol_ListOfShape aLFP1(theAllocator);
    BOPCol_ListOfShape aLFP(theAllocator);
    BOPCol_ListOfShape aLCBF(theAllocator);
    BOPCol_ListOfShape aLFIN(theAllocator);
    BOPCol_ListOfShape aLEx(theAllocator);
    //
    // for all non-solid faces build EF map [ aMEFP ]
    aItMS1.Initialize(aMFaces);
    for (; aItMS1.More(); aItMS1.Next()) {
      const TopoDS_Shape& aFace=aItMS1.Value();
      if (!aMS.Contains(aFace)) {
        BOPTools::MapShapesAndAncestors(aFace, TopAbs_EDGE, TopAbs_FACE, aMEFP);
      }
    }
    //
    // among all faces from aMEFP select these that have same edges
    // with the solid (i.e aMEF). These faces will be treated first 
    // to prevent the usage of 3D classifier.
    // The full list of faces to process is aLFP1. 
    aNbEFP=aMEFP.Extent();
    for (j=1; j<=aNbEFP; ++j) {
      const TopoDS_Shape& aE=aMEFP.FindKey(j);
      //
      if (aMEF.Contains(aE)) { // !!
        const BOPCol_ListOfShape& aLF=aMEFP(j);
        aItFP.Initialize(aLF);
        for (; aItFP.More(); aItFP.Next()) {
          const TopoDS_Shape& aF=aItFP.Value();
          if (aMFDone.Add(aF)) {
            aLFP1.Append(aF);
          }
        }
      }
      else {
        aLEx.Append(aE);
      }
    }
    //
    aItEx.Initialize(aLEx);
    for (; aItEx.More(); aItEx.Next()) {
      const TopoDS_Shape& aE=aItEx.Value();
      const BOPCol_ListOfShape& aLF=aMEFP.FindFromKey(aE);
      aItFP.Initialize(aLF);
      for (; aItFP.More(); aItFP.Next()) {
        const TopoDS_Shape& aF=aItFP.Value();
        if (aMFDone.Add(aF)) {
          //aLFP2.Append(aF);
          aLFP1.Append(aF);
        }
      }
    }
    //
    //==========
    //
    // 3 Process faces aLFP1
    aMFDone.Clear();
    aNbFP=aLFP1.Extent();
    aItFP.Initialize(aLFP1);
    for (; aItFP.More(); aItFP.Next()) {
      const TopoDS_Shape& aSP=aItFP.Value();
      if (!aMFDone.Add(aSP)) {
        continue;
      }
      
      //
      // first face to process
      aFP=(*(TopoDS_Face*)(&aSP));
      bIsIN=BOPTools_AlgoTools::IsInternalFace(aFP, aSolidSp, aMEF, 1.e-14, myContext);
      aState=(bIsIN) ? TopAbs_IN : TopAbs_OUT;
      //
      // collect faces to process [ aFP is the first ]
      aLFP.Clear();
      aLFP.Append(aFP);
      aItS.Initialize(aLFP1);
      for (; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aSk=aItS.Value();
        if (!aMFDone.Contains(aSk)) {
          aLFP.Append(aSk);
        }
      }
      //
      // Connexity Block that spreads from aFP the Bound 
      // or till the end of the block itself
      aLCBF.Clear();
      BOPTools_AlgoTools::MakeConnexityBlock(aLFP, aMS, aLCBF, theAllocator);
      //
      // fill states for the Connexity Block 
      aItS.Initialize(aLCBF);
      for (; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aSx=aItS.Value();
        aMFDone.Add(aSx);
        if (aState==TopAbs_IN) {
          aMFIN.Add(aSx);
        }
      }
      //
      aNbFPx=aMFDone.Extent();
      if (aNbFPx==aNbFP) {
        break;
      }
    }//for (; aItFP.More(); aItFP.Next())
    //
    // faces Inside aSolid
    aLFIN.Clear();
    aNbFIN=aMFIN.Extent();
    if (aNbFIN || aNbLIF) {
      aItMS1.Initialize(aMFIN);
      for (; aItMS1.More(); aItMS1.Next()) {
        const TopoDS_Shape& aFIn=aItMS1.Value();
        aLFIN.Append(aFIn);
      }
      //
      aItS.Initialize(aLIF);
      for (; aItS.More(); aItS.Next()) {
        const TopoDS_Shape& aFIN=aItS.Value();
        aLFIN.Append(aFIN);
      }
      //
      theInParts.Bind(aSolid, aLFIN);
    }
    if (aNbFIN || bHasImage) {
      theDraftSolids.Bind(aSolid, aSolidSp);
    }
  }// for (; aItMS.More(); aItMS.Next()) {
}

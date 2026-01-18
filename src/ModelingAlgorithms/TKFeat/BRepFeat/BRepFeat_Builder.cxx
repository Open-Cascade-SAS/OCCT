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
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <NCollection_List.hxx>
#include <BOPDS_Pave.hxx>
#include <Standard_Handle.hxx>
#include <NCollection_Map.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_Set.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepFeat_Builder.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
class BOPDS_PaveBlock;

//=================================================================================================

BRepFeat_Builder::BRepFeat_Builder()

{
  Clear();
}

//=================================================================================================

BRepFeat_Builder::~BRepFeat_Builder() = default;

//=================================================================================================

void BRepFeat_Builder::Clear()
{
  myShapes.Clear();
  myRemoved.Clear();
  BOPAlgo_BOP::Clear();
}

//=================================================================================================

void BRepFeat_Builder::Init(const TopoDS_Shape& theShape)
{
  Clear();
  //
  AddArgument(theShape);
}

//=================================================================================================

void BRepFeat_Builder::Init(const TopoDS_Shape& theShape, const TopoDS_Shape& theTool)
{
  Clear();
  //
  AddArgument(theShape);
  AddTool(theTool);
}

//=================================================================================================

void BRepFeat_Builder::SetOperation(const int theFuse)
{
  myFuse      = theFuse;
  myOperation = myFuse ? BOPAlgo_FUSE : BOPAlgo_CUT;
}

//=================================================================================================

void BRepFeat_Builder::SetOperation(const int theFuse, const bool theFlag)
{
  myFuse = theFuse;
  if (!theFlag)
  {
    myOperation = myFuse ? BOPAlgo_FUSE : BOPAlgo_CUT;
  }
  else
  {
    myOperation = myFuse ? BOPAlgo_CUT21 : BOPAlgo_COMMON;
  }
}

//=================================================================================================

void BRepFeat_Builder::PartsOfTool(NCollection_List<TopoDS_Shape>& aLT)
{
  TopExp_Explorer aExp;
  //
  aLT.Clear();
  aExp.Init(myShape, TopAbs_SOLID);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aS = aExp.Current();
    aLT.Append(aS);
  }
}

//=================================================================================================

void BRepFeat_Builder::KeepParts(const NCollection_List<TopoDS_Shape>& theIm)
{
  NCollection_List<TopoDS_Shape>::Iterator aItT;
  aItT.Initialize(theIm);
  for (; aItT.More(); aItT.Next())
  {
    const TopoDS_Shape& aTIm = aItT.Value();
    KeepPart(aTIm);
  }
}

//=================================================================================================

void BRepFeat_Builder::KeepPart(const TopoDS_Shape& thePart)
{
  TopoDS_Shape    aF, aFOr;
  TopExp_Explorer aExp;
  //
  TopExp::MapShapes(thePart, myShapes);
}

//=================================================================================================

void BRepFeat_Builder::Prepare()
{
  GetReport()->Clear();
  //
  BRep_Builder    aBB;
  TopoDS_Compound aC;
  aBB.MakeCompound(aC);
  myShape = aC;
  //
  FillRemoved();
}

//=================================================================================================

void BRepFeat_Builder::FillRemoved()
{
  TopExp_Explorer aExp;
  //
  const TopoDS_Shape& aArgs0 = myArguments.First();
  const TopoDS_Shape& aArgs1 = myTools.First();
  //
  aExp.Init(aArgs0, TopAbs_SOLID);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aS = aExp.Current();
    myImages.UnBind(aS);
  }
  //
  if (!myImages.IsBound(aArgs1))
  {
    return;
  }
  //
  NCollection_List<TopoDS_Shape>::Iterator aItIm;
  //
  NCollection_List<TopoDS_Shape>& aLS = myImages.ChangeFind(aArgs1);
  aItIm.Initialize(aLS);
  for (; aItIm.More(); aItIm.Next())
  {
    const TopoDS_Shape& aS = aItIm.Value();
    FillRemoved(aS, myRemoved);
  }
}

//=================================================================================================

void BRepFeat_Builder::PerformResult(const Message_ProgressRange& theRange)
{
  myOperation = myFuse ? BOPAlgo_FUSE : BOPAlgo_CUT;
  if (myShapes.IsEmpty())
  {
    BuildShape(theRange);
    return;
  }

  double                aWhole = 100.;
  Message_ProgressScope aPS(theRange, "BRepFeat_Builder", aWhole);
  double                aBSPart = 15;
  aWhole -= aBSPart;

  // Compute PI steps
  const int                  aSize = 4;
  NCollection_Array1<double> aSteps(0, aSize - 1);
  {
    for (int i = 0; i < aSize; ++i)
      aSteps(i) = 0.;

    NbShapes aNbShapes       = getNbShapes();
    double   aTreatFaces     = 5 * aNbShapes.NbFaces();
    double   aTreatShells    = aNbShapes.NbShells();
    double   aTreatSolids    = 20 * aNbShapes.NbSolids();
    double   aTreatCompounds = aNbShapes.NbCompounds();

    double aSum = aTreatFaces + aTreatShells + aTreatSolids + aTreatCompounds;
    if (aSum > 0)
    {
      aSteps(0) = aTreatFaces * aWhole / aSum;
      aSteps(1) = aTreatShells * aWhole / aSum;
      aSteps(2) = aTreatSolids * aWhole / aSum;
      aSteps(3) = aTreatCompounds * aWhole / aSum;
    }
  }
  //
  Prepare();
  //
  RebuildFaces();
  aPS.Next(aSteps(0));
  //
  FillImagesContainers(TopAbs_SHELL, aPS.Next(aSteps(1)));
  if (HasErrors())
  {
    return;
  }
  //
  FillImagesSolids(aPS.Next(aSteps(2)));
  if (HasErrors())
  {
    return;
  }
  //
  CheckSolidImages();
  //
  BuildResult(TopAbs_SOLID);
  if (HasErrors())
  {
    return;
  }
  //
  FillImagesCompounds(aPS.Next(aSteps(3)));
  if (HasErrors())
  {
    return;
  }
  //
  BuildResult(TopAbs_COMPOUND);
  if (HasErrors())
  {
    return;
  }
  //
  BuildShape(aPS.Next(aBSPart));
}

//=================================================================================================

void BRepFeat_Builder::RebuildFaces()
{
  int                aNbS, i, iRank, nSp, j;
  bool               bIsClosed, bIsDegenerated, bToReverse, bRem, bIm, bFlagSD, bVInShapes;
  TopAbs_Orientation anOriF, anOriE;
  TopoDS_Face        aFF, aFSD;
  TopoDS_Edge        aSp;
  TopoDS_Shape       aSx;
  TopExp_Explorer    aExp, aExpE;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>           aME, aMESplit;
  NCollection_List<TopoDS_Shape>::Iterator                         aItIm;
  NCollection_Map<occ::handle<BOPDS_PaveBlock>>::Iterator          aItMPB;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aItM;
  NCollection_Map<BOPTools_Set>                                    aMST;
  NCollection_List<TopoDS_Shape>                                   aLE;
  //
  aItM.Initialize(myShapes);
  for (; aItM.More(); aItM.Next())
  {
    const TopoDS_Shape& aS = aItM.Value();
    if (aS.ShapeType() == TopAbs_FACE)
    {
      BOPTools_Set aST;
      aST.Add(aS, TopAbs_EDGE);
      aMST.Add(aST);
    }
  }
  //
  aNbS = myDS->NbSourceShapes();
  for (i = 0; i < aNbS; ++i)
  {
    const BOPDS_ShapeInfo& aSI = myDS->ShapeInfo(i);
    //
    iRank = myDS->Rank(i);
    if (iRank == 1)
    {
      const TopoDS_Shape& aS = aSI.Shape();
      //
      NCollection_List<TopoDS_Shape>* pLIm = myImages.ChangeSeek(aS);
      if (pLIm)
      {
        NCollection_List<TopoDS_Shape>& aLIm = *pLIm;
        aItIm.Initialize(aLIm);
        for (; aItIm.More();)
        {
          const TopoDS_Shape& aSIm = aItIm.Value();
          if (!myShapes.Contains(aSIm))
          {
            aLIm.Remove(aItIm);
            continue;
          }
          aItIm.Next();
        }
      }
      continue;
    }
    //
    if (aSI.ShapeType() != TopAbs_FACE)
    {
      continue;
    }
    //
    const BOPDS_FaceInfo& aFI = myDS->FaceInfo(i);
    const TopoDS_Face&    aF  = (*(TopoDS_Face*)(&aSI.Shape()));
    //
    if (!myImages.IsBound(aF))
    {
      continue;
    }
    //
    anOriF = aF.Orientation();
    aFF    = aF;
    aFF.Orientation(TopAbs_FORWARD);

    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aMPBIn = aFI.PaveBlocksIn();
    const NCollection_IndexedMap<occ::handle<BOPDS_PaveBlock>>& aMPBSc = aFI.PaveBlocksSc();

    aLE.Clear();

    // bounding edges
    aExp.Init(aFF, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next())
    {
      const TopoDS_Edge& aE                 = (*(TopoDS_Edge*)(&aExp.Current()));
      anOriE                                = aE.Orientation();
      bIsDegenerated                        = BRep_Tool::Degenerated(aE);
      bIsClosed                             = BRep_Tool::IsClosed(aE, aF);
      NCollection_List<TopoDS_Shape>* pLEIm = myImages.ChangeSeek(aE);
      if (pLEIm)
      {
        NCollection_List<TopoDS_Shape>& aLEIm = *pLEIm;
        //
        bRem = false;
        bIm  = false;
        aME.Clear();
        NCollection_List<TopoDS_Shape> aLEImNew;
        //
        aItIm.Initialize(aLEIm);
        for (; aItIm.More(); aItIm.Next())
        {
          const TopoDS_Shape& aS = aItIm.Value();

          bVInShapes = false;
          if (myShapes.Contains(aS))
          {
            bVInShapes = true;
          }
          else
          {
            aExpE.Init(aS, TopAbs_VERTEX);
            for (; aExpE.More(); aExpE.Next())
            {
              const TopoDS_Shape& aV = aExpE.Current();
              if (myShapes.Contains(aV))
              {
                bVInShapes = true;
                break;
              }
            }
          }
          //
          if (bVInShapes)
          {
            bIm = true;
            aLEImNew.Append(aS);
          }
          else
          {
            bRem = true;
            aME.Add(aS);
          }
        }
        //
        if (!bIm)
        {
          aLE.Append(aE);
          continue;
        }
        //
        if (bRem && bIm)
        {
          if (aLEIm.Extent() == 2)
          {
            aLE.Append(aE);
            continue;
          }
          if (aMESplit.Add(aE))
          {
            RebuildEdge(aE, aFF, aME, aLEImNew);
            aLEIm.Assign(aLEImNew);
            if (aLEIm.Extent() == 1)
            {
              aLE.Append(aE);
              continue;
            }
          }
        }
        //
        aItIm.Initialize(aLEIm);
        for (; aItIm.More(); aItIm.Next())
        {
          aSp = *(TopoDS_Edge*)&aItIm.Value();

          if (bIsDegenerated)
          {
            aSp.Orientation(anOriE);
            aLE.Append(aSp);
            continue;
          }
          //
          if (anOriE == TopAbs_INTERNAL)
          {
            aSp.Orientation(TopAbs_FORWARD);
            aLE.Append(aSp);
            aSp.Orientation(TopAbs_REVERSED);
            aLE.Append(aSp);
            continue;
          }
          //
          if (bIsClosed)
          {
            if (!BRep_Tool::IsClosed(aSp, aFF))
            {
              BOPTools_AlgoTools3D::DoSplitSEAMOnFace(aSp, aFF);
            }
            //
            aSp.Orientation(TopAbs_FORWARD);
            aLE.Append(aSp);
            aSp.Orientation(TopAbs_REVERSED);
            aLE.Append(aSp);
            continue;
          } // if (bIsClosed){
          //
          aSp.Orientation(anOriE);
          bToReverse = BOPTools_AlgoTools::IsSplitToReverse(aSp, aE, myContext);
          if (bToReverse)
          {
            aSp.Reverse();
          }
          aLE.Append(aSp);
        }
      }
      else
      {
        aLE.Append(aE);
      }
    }

    int aNbPBIn, aNbPBSc;
    aNbPBIn = aMPBIn.Extent();
    aNbPBSc = aMPBSc.Extent();
    //
    // in edges
    for (j = 1; j <= aNbPBIn; ++j)
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aMPBIn(j);
      nSp                                     = aPB->Edge();
      aSp                                     = (*(TopoDS_Edge*)(&myDS->Shape(nSp)));
      if (myRemoved.Contains(aSp))
      {
        continue;
      }
      //
      aSp.Orientation(TopAbs_FORWARD);
      aLE.Append(aSp);
      aSp.Orientation(TopAbs_REVERSED);
      aLE.Append(aSp);
    }
    // section edges
    for (j = 1; j <= aNbPBSc; ++j)
    {
      const occ::handle<BOPDS_PaveBlock>& aPB = aMPBSc(j);
      nSp                                     = aPB->Edge();
      aSp                                     = (*(TopoDS_Edge*)(&myDS->Shape(nSp)));
      if (myRemoved.Contains(aSp))
      {
        continue;
      }
      //
      aSp.Orientation(TopAbs_FORWARD);
      aLE.Append(aSp);
      aSp.Orientation(TopAbs_REVERSED);
      aLE.Append(aSp);
    }

    // build new faces
    BOPAlgo_BuilderFace aBF;
    aBF.SetFace(aFF);
    aBF.SetShapes(aLE);

    aBF.Perform();

    NCollection_List<TopoDS_Shape>& aLFIm = myImages.ChangeFind(aF);
    aLFIm.Clear();

    const NCollection_List<TopoDS_Shape>& aLFR = aBF.Areas();
    aItIm.Initialize(aLFR);
    for (; aItIm.More(); aItIm.Next())
    {
      TopoDS_Shape& aFR = aItIm.ChangeValue();
      //
      BOPTools_Set aST;
      aST.Add(aFR, TopAbs_EDGE);
      bFlagSD = aMST.Contains(aST);
      //
      const BOPTools_Set& aSTx = aMST.Added(aST);
      aSx                      = aSTx.Shape();
      aSx.Orientation(anOriF);
      aLFIm.Append(aSx);
      //
      NCollection_List<TopoDS_Shape>* pLOr = myOrigins.ChangeSeek(aSx);
      if (!pLOr)
      {
        pLOr = myOrigins.Bound(aSx, NCollection_List<TopoDS_Shape>());
      }
      pLOr->Append(aF);
      //
      if (bFlagSD)
      {
        myShapesSD.Bind(aFR, aSx);
      }
    }
    //
    if (aLFIm.Extent() == 0)
    {
      myImages.UnBind(aF);
    }
  }
}

//=================================================================================================

void BRepFeat_Builder::RebuildEdge(
  const TopoDS_Shape&                                           theE,
  const TopoDS_Face&                                            theF,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aME,
  NCollection_List<TopoDS_Shape>&                               aLIm)
{
  int                                                      nE, nSp, nV1, nV2, nE1, nV, nVx, nVSD;
  int                                                      nV11, nV21;
  bool                                                     bOld;
  double                                                   aT11, aT21;
  double                                                   aT1, aT2;
  TopoDS_Edge                                              aSp, aE;
  BOPDS_ShapeInfo                                          aSI;
  TopoDS_Vertex                                            aV1, aV2;
  occ::handle<BOPDS_PaveBlock>                             aPBNew;
  NCollection_Map<int>                                     aMI, aMAdd, aMV, aMVOr;
  NCollection_List<occ::handle<BOPDS_PaveBlock>>::Iterator aItPB;
  NCollection_List<TopoDS_Shape>::Iterator                 aIt;
  NCollection_List<int>::Iterator                          aItLI;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aItM;
  NCollection_Map<occ::handle<BOPDS_PaveBlock>>                    aMPB;
  NCollection_Map<occ::handle<BOPDS_PaveBlock>>::Iterator          aItMPB;
  //
  aSI.SetShapeType(TopAbs_EDGE);

  // 1. collect origin vertices to aMV map.
  nE                                = myDS->Index(theE);
  const BOPDS_ShapeInfo&       aSIE = myDS->ShapeInfo(nE);
  const NCollection_List<int>& aLS  = aSIE.SubShapes();
  aItLI.Initialize(aLS);
  for (; aItLI.More(); aItLI.Next())
  {
    nV  = aItLI.Value();
    nVx = nV;
    if (myDS->HasShapeSD(nV, nVSD))
    {
      nVx = nVSD;
    }
    aMV.Add(nVx);
    aMVOr.Add(nVx);
  }
  //
  // 2. collect vertices that should be removed to aMI map.
  aPBNew                                                 = new BOPDS_PaveBlock;
  NCollection_List<BOPDS_Pave>&                   aLPExt = aPBNew->ChangeExtPaves();
  NCollection_List<occ::handle<BOPDS_PaveBlock>>& aLPB   = myDS->ChangePaveBlocks(nE);
  //
  for (aItPB.Initialize(aLPB); aItPB.More(); aItPB.Next())
  {
    const occ::handle<BOPDS_PaveBlock>& aPB = aItPB.Value();
    nE1                                     = aPB->Edge();
    const TopoDS_Shape& aE1                 = myDS->Shape(nE1);
    //
    if (aME.Contains(aE1))
    {
      aPB->Indices(nV1, nV2);
      aMI.Add(nV1);
      aMI.Add(nV2);
    }
    else
    {
      aMPB.Add(aPB);
    }
  }
  // 3. collect vertices that split the source shape.
  for (aItPB.Initialize(aLPB); aItPB.More(); aItPB.Next())
  {
    const occ::handle<BOPDS_PaveBlock>& aPB = aItPB.Value();
    aPB->Indices(nV1, nV2);
    //
    if (!aMI.Contains(nV1))
    {
      aMV.Add(nV1);
    }
    if (!aMI.Contains(nV2))
    {
      aMV.Add(nV2);
    }
  }
  // 4. collect ext paves.
  for (aItPB.Initialize(aLPB); aItPB.More(); aItPB.Next())
  {
    const occ::handle<BOPDS_PaveBlock>& aPB = aItPB.Value();
    aPB->Indices(nV1, nV2);
    //
    if (aMV.Contains(nV1))
    {
      if (aMAdd.Add(nV1) || aMVOr.Contains(nV1))
      {
        aLPExt.Append(aPB->Pave1());
      }
    }
    //
    if (aMV.Contains(nV2))
    {
      if (aMAdd.Add(nV2) || aMVOr.Contains(nV2))
      {
        aLPExt.Append(aPB->Pave2());
      }
    }
  }

  aE = (*(TopoDS_Edge*)(&theE));
  aE.Orientation(TopAbs_FORWARD);
  //
  aLIm.Clear();
  //
  // 5. split edge by new set of vertices.
  aLPB.Clear();
  aPBNew->SetOriginalEdge(nE);
  aPBNew->Update(aLPB, false);
  //
  for (aItPB.Initialize(aLPB); aItPB.More(); aItPB.Next())
  {
    occ::handle<BOPDS_PaveBlock>& aPB    = aItPB.ChangeValue();
    const BOPDS_Pave&             aPave1 = aPB->Pave1();
    aPave1.Contents(nV1, aT1);
    //
    const BOPDS_Pave& aPave2 = aPB->Pave2();
    aPave2.Contents(nV2, aT2);
    //
    aItMPB.Initialize(aMPB);
    // check if it is the old pave block.
    bOld = false;
    for (; aItMPB.More(); aItMPB.Next())
    {
      const occ::handle<BOPDS_PaveBlock>& aPB1 = aItMPB.Value();
      aPB1->Indices(nV11, nV21);
      aPB1->Range(aT11, aT21);
      if (nV1 == nV11 && nV2 == nV21 && aT1 == aT11 && aT2 == aT21)
      {
        const TopoDS_Shape& aEIm = myDS->Shape(aPB1->Edge());
        aLIm.Append(aEIm);
        bOld = true;
        break;
      }
    }
    if (bOld)
    {
      continue;
    }
    //
    aV1 = (*(TopoDS_Vertex*)(&myDS->Shape(nV1)));
    aV1.Orientation(TopAbs_FORWARD);
    //
    aV2 = (*(TopoDS_Vertex*)(&myDS->Shape(nV2)));
    aV2.Orientation(TopAbs_REVERSED);
    //
    BOPTools_AlgoTools::MakeSplitEdge(aE, aV1, aT1, aV2, aT2, aSp);
    BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aSp, theF, myContext);
    //
    aSI.SetShape(aSp);
    //
    Bnd_Box& aBox = aSI.ChangeBox();
    BRepBndLib::Add(aSp, aBox);
    //
    nSp = myDS->Append(aSI);
    //
    aPB->SetEdge(nSp);
    aLIm.Append(aSp);
  }
}

//=================================================================================================

void BRepFeat_Builder::CheckSolidImages()
{
  NCollection_Map<BOPTools_Set>                          aMST;
  NCollection_List<TopoDS_Shape>                         aLSImNew;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMS;
  NCollection_List<TopoDS_Shape>::Iterator               aIt;
  TopExp_Explorer                                        aExp, aExpF;
  bool                                                   bFlagSD;
  //
  const TopoDS_Shape& aArgs0 = myArguments.First();
  const TopoDS_Shape& aArgs1 = myTools.First();
  //
  const NCollection_List<TopoDS_Shape>& aLSIm = myImages.Find(aArgs1);
  aIt.Initialize(aLSIm);
  for (; aIt.More(); aIt.Next())
  {
    const TopoDS_Shape& aSolIm = aIt.Value();
    //
    BOPTools_Set aST;
    aST.Add(aSolIm, TopAbs_FACE);
    aMST.Add(aST);
  }
  //
  aExp.Init(aArgs0, TopAbs_SOLID);
  for (; aExp.More(); aExp.Next())
  {
    const TopoDS_Shape& aSolid = aExp.Current();
    if (myImages.IsBound(aSolid))
    {
      NCollection_List<TopoDS_Shape>& aLSImSol = myImages.ChangeFind(aSolid);
      aIt.Initialize(aLSImSol);
      for (; aIt.More(); aIt.Next())
      {
        const TopoDS_Shape& aSolIm = aIt.Value();
        //
        BOPTools_Set aST;
        aST.Add(aSolIm, TopAbs_FACE);
        bFlagSD = aMST.Contains(aST);
        //
        const BOPTools_Set& aSTx = aMST.Added(aST);
        const TopoDS_Shape& aSx  = aSTx.Shape();
        aLSImNew.Append(aSx);
        //
        if (bFlagSD)
        {
          myShapesSD.Bind(aSolIm, aSx);
        }
      }
      aLSImSol.Assign(aLSImNew);
    }
  }
}

//=================================================================================================

void BRepFeat_Builder::FillRemoved(const TopoDS_Shape&                                     S,
                                   NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& M)
{
  if (myShapes.Contains(S))
  {
    return;
  }
  //
  M.Add(S);
  TopoDS_Iterator It(S);
  while (It.More())
  {
    FillRemoved(It.Value(), M);
    It.Next();
  }
}

//=================================================================================================

void BRepFeat_Builder::FillIn3DParts(
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theDraftSolids,
  const Message_ProgressRange&                                              theRange)
{
  GetReport()->Clear();

  BOPAlgo_Builder::FillIn3DParts(theDraftSolids, theRange);

  // Clear the IN parts of the solids from the removed faces
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>::
    Iterator itM(myInParts);
  for (; itM.More(); itM.Next())
  {
    NCollection_List<TopoDS_Shape>&          aList = itM.ChangeValue();
    NCollection_List<TopoDS_Shape>::Iterator itL(aList);
    for (; itL.More();)
    {
      if (myRemoved.Contains(itL.Value()))
        aList.Remove(itL);
      else
        itL.Next();
    }
  }
}

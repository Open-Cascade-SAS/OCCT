// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BOPAlgo_PaveFiller.ixx>

#include <NCollection_IncAllocator.hxx>

#include <TopoDS_Vertex.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

#include <BRepBndLib.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>

#include <TopExp.hxx>

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom2d_Curve.hxx>

#include <BOPTools_AlgoTools.hxx>
#include <BOPTools_AlgoTools2D.hxx>

#include <BOPDS_VectorOfListOfPaveBlock.hxx>
#include <BOPDS_ListOfPaveBlock.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_CommonBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_VectorOfInterfFF.hxx>
#include <BOPDS_Interf.hxx>
#include <BOPDS_VectorOfCurve.hxx>
#include <BOPDS_VectorOfFaceInfo.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Curve.hxx>

static void UpdateVertices(const TopoDS_Edge& aE, 
                           const TopoDS_Face& aF);

//=======================================================================
// function: MakeSplitEdges
// purpose: 
//=======================================================================
  void BOPAlgo_PaveFiller::MakeSplitEdges()
{
  Standard_Integer aNbPBP;
  //
  myErrorStatus=0;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP=myDS->ChangePaveBlocksPool();
  aNbPBP=aPBP.Extent();
  if(!aNbPBP) {
    return;
  }
  //
  Standard_Boolean bCB, bV1, bV2;
  Standard_Integer i, nE, nV1, nV2, nSp, aNbPB;
  Standard_Real aT1, aT2;
  Handle(NCollection_IncAllocator) aAllocator;
  BOPDS_ListIteratorOfListOfPaveBlock aItPB, aItPBCB;
  Handle(BOPDS_PaveBlock) aPB, aPBx;
  //-----------------------------------------------------scope f
  //
  aAllocator=new NCollection_IncAllocator();
  //
  BOPDS_MapOfPaveBlock aMPB(100,aAllocator);
  //
  for (i=0; i<aNbPBP; ++i) {
    BOPDS_ListOfPaveBlock& aLPB=aPBP(i);
    //
    aNbPB=aLPB.Extent();
    //DEBf
    if (aNbPB) {
      aPBx=aLPB.First();
    }
    //DEBt
    if (aNbPB==1) {
      aPB=aLPB.First();
      aPB->Indices(nV1, nV2);
      bV1=myDS->IsNewShape(nV1);
      bV2=myDS->IsNewShape(nV2);
      //
      if (!(bV1 || bV2)) {
        nE=aPB->OriginalEdge();
        aPB->SetEdge(nE);
        continue;
      }
    }
    //
    aItPB.Initialize(aLPB);
    for (; aItPB.More(); aItPB.Next()) {
      aPB=aItPB.Value();
      const Handle(BOPDS_CommonBlock)& aCB=myDS->CommonBlock(aPB);
      bCB=!aCB.IsNull();
      if (bCB) {
        myDS->SortPaveBlocks(aCB);
        aPB=aCB->PaveBlock1();
      }
      //
      if (aMPB.Add(aPB)) {
        nE=aPB->OriginalEdge();
        aPB->Indices(nV1, nV2);
        aPB->Range(aT1, aT2);
        //
        nSp = SplitEdge(nE, nV1, aT1, nV2, aT2);
        //
        if (bCB) {
          aCB->SetEdge(nSp);
        }
        else {
          aPB->SetEdge(nSp);
        }
      }// if (aMPB.Add(aPB)) {
    }// for (; aItPB.More(); aItPB.Next()) {
  }// for (i=0; i<aNbPBP; ++i) {
  //
  //-----------------------------------------------------scope t
  aMPB.Clear();
  aAllocator.Nullify();
}

//=======================================================================
// function: SplitEdge
// purpose: 
//=======================================================================
Standard_Integer BOPAlgo_PaveFiller::SplitEdge(const Standard_Integer nE, 
                                               const Standard_Integer nV1,
                                               const Standard_Real aT1, 
                                               const Standard_Integer nV2, 
                                               const Standard_Real aT2)
{
  Standard_Integer nSp;
  TopoDS_Vertex aV1, aV2;
  TopoDS_Edge aE, aSp;
  BOPDS_ShapeInfo aSI;
  //
  aSI.SetShapeType(TopAbs_EDGE);
  //
  aE=(*(TopoDS_Edge *)(&myDS->Shape(nE))); 
  aE.Orientation(TopAbs_FORWARD);
  //
  aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
  aV1.Orientation(TopAbs_FORWARD); 
  //
  aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
  aV2.Orientation(TopAbs_REVERSED); 
  //
  BOPTools_AlgoTools::MakeSplitEdge(aE, aV1, aT1, aV2, aT2, aSp);  
  //
  aSI.SetShape(aSp);
  //
  Bnd_Box& aBox=aSI.ChangeBox();
  BRepBndLib::Add(aSp, aBox);
  //
  nSp=myDS->Append(aSI);
  return nSp;
}

//=======================================================================
// function: MakePCurves
// purpose: 
//=======================================================================
  void BOPAlgo_PaveFiller::MakePCurves()
{
  Standard_Integer i, nF1, nF2, aNbC, k, nE, aNbFF, aNbFI;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_MapIteratorOfMapOfPaveBlock aItMPB;
  TopoDS_Face aF1F, aF2F;
  //
  myErrorStatus=0;
  //
  // 1. Process Common Blocks 
  const BOPDS_VectorOfFaceInfo& aFIP=myDS->FaceInfoPool();
  //
  aNbFI=aFIP.Extent();
  for (i=0; i<aNbFI; ++i) {
    const BOPDS_FaceInfo& aFI=aFIP(i);
    nF1=aFI.Index();
    //
    aF1F=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    aF1F.Orientation(TopAbs_FORWARD);
    // In
    const BOPDS_IndexedMapOfPaveBlock& aMPBIn=aFI.PaveBlocksIn();
    aItMPB.Initialize(aMPBIn);
    for(; aItMPB.More(); aItMPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB=aItMPB.Value();
      nE=aPB->Edge();
      const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE)));
      //
      BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aE, aF1F);
    }
    // On
    const BOPDS_IndexedMapOfPaveBlock& aMPBOn=aFI.PaveBlocksOn();
    aItMPB.Initialize(aMPBOn);
    for(; aItMPB.More(); aItMPB.Next()) {
      const Handle(BOPDS_PaveBlock)& aPB=aItMPB.Value();
      if (myDS->IsCommonBlockOnEdge(aPB)) {
        nE=aPB->Edge();
        const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE)));
        //
        BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aE, aF1F);
      }
    }
  }
  //
  if (!mySectionAttribute.PCurveOnS1() && 
      !mySectionAttribute.PCurveOnS2()) {
    return;
  }
  //
  // 2. Process section edges
  BOPDS_VectorOfInterfFF& aFFs=myDS->InterfFF();
  aNbFF=aFFs.Extent();
  for (i=0; i<aNbFF; ++i) {
    const BOPDS_InterfFF& aFF=aFFs(i);
    aFF.Indices(nF1, nF2);
    //
    aF1F=(*(TopoDS_Face *)(&myDS->Shape(nF1)));
    aF1F.Orientation(TopAbs_FORWARD);
    aF2F=(*(TopoDS_Face *)(&myDS->Shape(nF2)));
    aF2F.Orientation(TopAbs_FORWARD);
    //
    const BOPDS_VectorOfCurve& aVNC=aFF.Curves();
    aNbC=aVNC.Extent();
    for (k=0; k<aNbC; ++k) {
      const BOPDS_Curve& aNC=aVNC(k);
      const BOPDS_ListOfPaveBlock& aLPB=aNC.PaveBlocks();
      aItLPB.Initialize(aLPB);
      for(; aItLPB.More(); aItLPB.Next()) {
        const Handle(BOPDS_PaveBlock)& aPB=aItLPB.Value();
        nE=aPB->Edge();
        const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE))); 
        //
        if (mySectionAttribute.PCurveOnS1()) {
          BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aE, aF1F);
          UpdateVertices(aE, aF1F);
        }
        //
        if (mySectionAttribute.PCurveOnS2()) {
          BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aE, aF2F);
          UpdateVertices(aE, aF2F);
        }
      }
    }
  }
}

//=======================================================================
// function: RefineFaceInfoOn
// purpose: 
//=======================================================================
  void BOPAlgo_PaveFiller::RefineFaceInfoOn() 
{
  Standard_Integer aNbPBP;
  //
  myErrorStatus=0;
  //
  BOPDS_VectorOfListOfPaveBlock& aPBP=myDS->ChangePaveBlocksPool();
  aNbPBP=aPBP.Extent();
  if(!aNbPBP) {
    return;
  }
  //
  Standard_Boolean bV1, bV2;
  Standard_Integer i, nV1, nV2, aNbPB;
  Handle(BOPDS_PaveBlock) aPB;
  //
  for (i=0; i<aNbPBP; ++i) {
    BOPDS_ListOfPaveBlock& aLPB=aPBP(i);
    //
    aNbPB=aLPB.Extent();
    if (aNbPB==1) {
      aPB=aLPB.First();
      aPB->Indices(nV1, nV2);
      bV1=myDS->IsNewShape(nV1);
      bV2=myDS->IsNewShape(nV2);
      //
      if (!(bV1 || bV2)) {
        if (!myDS->IsCommonBlock(aPB)) {
          // the PB seems to be untouced
          aLPB.Clear();
          continue;
        }
      }//if (!(bV1 || bV2)) {
    }//if (aNbPB==1) {
  }//for (i=0; i<aNbPBP; ++i) {
  myDS->RefineFaceInfoOn();
}

//=======================================================================
//function : UpdateVertices
//purpose  : update tolerances of vertices comparing extremities of
//           3d and 2d curves
//=======================================================================
void UpdateVertices(const TopoDS_Edge& aE, const TopoDS_Face& aF)
{
  Standard_Integer j;
  Standard_Real aT[2], aUx, aVx, aTolV2, aD2, aD;
  gp_Pnt aP3D, aP3Dx;
  gp_Pnt2d aP2Dx;
  Handle(Geom_Surface) aS;
  Handle(Geom_Curve) aC3D;
  Handle(Geom2d_Curve) aC2D;
  TopoDS_Edge aEf;
  TopoDS_Vertex aV[2];
  BRep_Builder aBB;
  //
  aEf=aE;
  aEf.Orientation(TopAbs_FORWARD);
  //
  TopExp::Vertices(aEf, aV[0], aV[1]);
  //
  aS=BRep_Tool::Surface(aF);
  aC3D=BRep_Tool::Curve(aEf, aT[0], aT[1]);
  aC2D=BRep_Tool::CurveOnSurface(aEf, aF, aT[0], aT[1]);
  //
  for (j=0; j<2; ++j) {
    aTolV2=BRep_Tool::Tolerance(aV[j]);
    aTolV2=aTolV2*aTolV2;
    //
    aC3D->D0(aT[j], aP3D);
    aC2D->D0(aT[j], aP2Dx);
    aP2Dx.Coord(aUx, aVx);
    aS->D0(aUx, aVx, aP3Dx);
    aD2=aP3D.SquareDistance(aP3Dx);
    if (aD2>aTolV2) {
      aD=sqrt(aD2);
      aBB.UpdateVertex(aV[j], aD);
    }
  }
}


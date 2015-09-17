// Created by: Peter KURNEV
// Copyright (c) 2010-2014 OPEN CASCADE SAS
// Copyright (c) 2007-2010 CEA/DEN, EDF R&D, OPEN CASCADE
// Copyright (c) 2003-2007 OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN, CEDRAT,
//                         EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPAlgo_SectionAttribute.hxx>
#include <BOPCol_ListOfInteger.hxx>
#include <BOPDS_Curve.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_FaceInfo.hxx>
#include <BOPDS_MapOfPaveBlock.hxx>
#include <BOPDS_Pave.hxx>
#include <BOPDS_PaveBlock.hxx>
#include <BOPDS_ShapeInfo.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <ElCLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntTools_Context.hxx>
#include <Precision.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Vertex.hxx>

static
  void MakeSplitEdge1 (const TopoDS_Edge&   aE,
                       const TopoDS_Face&   aF,
                       const TopoDS_Vertex& aV1,
                       const Standard_Real  aP1,
                       const TopoDS_Vertex& aV2,
                       const Standard_Real  aP2,
                       TopoDS_Edge& aNewEdge);

//=======================================================================
//function : ProcessDE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ProcessDE()
{
  Standard_Integer nF, aNb, nE, nV, nVSD, aNbPB;
  Handle(NCollection_BaseAllocator) aAllocator;
  Handle(BOPDS_PaveBlock) aPBD;
  BOPCol_ListIteratorOfListOfInteger aItLI;
  //
  myErrorStatus=0;
  //
  // 1. Find degnerated edges
  //-----------------------------------------------------scope f
  //
  aAllocator=
    NCollection_BaseAllocator::CommonBaseAllocator();
  BOPDS_ListOfPaveBlock aLPBOut(aAllocator);
  //
  aNb=myDS->NbSourceShapes();
  for (nE=0; nE<aNb; ++nE) {
    const BOPDS_ShapeInfo& aSIE=myDS->ShapeInfo(nE);
    if (aSIE.ShapeType()==TopAbs_EDGE) {
      if (aSIE.HasFlag(nF)) {
        const BOPDS_ShapeInfo& aSIF=myDS->ShapeInfo(nF);
        nV=aSIE.SubShapes().First();
        if (myDS->HasShapeSD(nV, nVSD)) {
          nV=nVSD;
        }
        //nV,nE,nF
        //
        if (aSIF.ShapeType() == TopAbs_FACE) {
          // 1. Find PaveBlocks that are go through nV for nF
          FindPaveBlocks(nV, nF, aLPBOut);
          aNbPB=aLPBOut.Extent();
          if (!aNbPB) {
            continue;
          }
          //
          // 2.
          BOPDS_ListOfPaveBlock& aLPBD=myDS->ChangePaveBlocks(nE);
          aPBD=aLPBD.First();
          //
          FillPaves(nV, nE, nF, aLPBOut, aPBD);
          //
          myDS->UpdatePaveBlock(aPBD);
          //
          MakeSplitEdge(nE, nF);
          //
          aLPBOut.Clear();
        }
        if (aSIF.ShapeType() == TopAbs_EDGE) {
          Standard_Real aTol=1.e-7;
          Standard_Integer nEn;
          BRep_Builder BB;
          const TopoDS_Edge& aDE=(*(TopoDS_Edge *)(&myDS->Shape(nE))); 
          const TopoDS_Vertex& aVn = (*(TopoDS_Vertex *)(&myDS->Shape(nV)));
          //
          TopoDS_Edge aE=aDE;
          aE.EmptyCopy();
          BB.Add(aE, aVn);
          BB.Degenerated(aE, Standard_True);
          BB.UpdateEdge(aE, aTol);
          BOPDS_ShapeInfo aSI;
          aSI.SetShapeType(TopAbs_EDGE);
          aSI.SetShape(aE);
          nEn=myDS->Append(aSI);
          BOPDS_ListOfPaveBlock& aLPBD=myDS->ChangePaveBlocks(nE);
          aPBD=aLPBD.First();
          aPBD->SetEdge(nEn);
        }
      }
    }
  }
}

//=======================================================================
//function : FindPaveBlocks
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::FindPaveBlocks(const Standard_Integer nV,
                                          const Standard_Integer nF,
                                          BOPDS_ListOfPaveBlock& aLPBOut)
{
  Standard_Integer i, aNbPBOn, aNbPBIn, aNbPBSc, nV1, nV2;
  //
  const BOPDS_FaceInfo& aFI=myDS->ChangeFaceInfo(nF);
  // In
  const BOPDS_IndexedMapOfPaveBlock& aMPBIn=aFI.PaveBlocksIn();
  aNbPBIn = aMPBIn.Extent();
  for (i = 1; i <= aNbPBIn; ++i) {
    const Handle(BOPDS_PaveBlock)& aPB = aMPBIn(i);
    aPB->Indices(nV1, nV2);
    if (nV==nV1 || nV==nV2) {
      aLPBOut.Append(aPB);
    }
  }
  // On
  const BOPDS_IndexedMapOfPaveBlock& aMPBOn=aFI.PaveBlocksOn();
  aNbPBOn = aMPBOn.Extent();
  for (i = 1; i <= aNbPBOn; ++i) {
    const Handle(BOPDS_PaveBlock)& aPB = aMPBOn(i);
    aPB->Indices(nV1, nV2);
    if (nV==nV1 || nV==nV2) {
      aLPBOut.Append(aPB);
    }
  }
  // Sections
  const BOPDS_IndexedMapOfPaveBlock& aMPBSc=aFI.PaveBlocksSc();
  aNbPBSc = aMPBSc.Extent();
  for (i = 1; i <= aNbPBSc; ++i) {
    const Handle(BOPDS_PaveBlock)& aPB = aMPBSc(i);
    aPB->Indices(nV1, nV2);
    if (nV==nV1 || nV==nV2) {
      aLPBOut.Append(aPB);
    }
  }
}

//=======================================================================
//function : MakeSplitEdge
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::MakeSplitEdge (const Standard_Integer nDE,
                                          const Standard_Integer nDF)
{ 
  Standard_Integer nSp, nV1, nV2, aNbPB;
  Standard_Real aT1, aT2;
  TopoDS_Edge aDE, aSp;
  TopoDS_Vertex aV1, aV2;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_ShapeInfo aSI;
  //
  aSI.SetShapeType(TopAbs_EDGE);
  //
  aDE=(*(TopoDS_Edge *)(&myDS->Shape(nDE))); 
  aDE.Orientation(TopAbs_FORWARD);
  //
  const TopoDS_Face& aDF=(*(TopoDS_Face *)(&myDS->Shape(nDF)));
  //
  BOPDS_ListOfPaveBlock& aLPB=myDS->ChangePaveBlocks(nDE);
  aNbPB=aLPB.Extent();
  //
  aItLPB.Initialize(aLPB);
  for (; aItLPB.More(); aItLPB.Next()) {
    Handle(BOPDS_PaveBlock)& aPB=aItLPB.ChangeValue();
    //
    const BOPDS_Pave& aPave1=aPB->Pave1();
    aPave1.Contents(nV1, aT1);
    //
    const BOPDS_Pave& aPave2=aPB->Pave2();
    aPave2.Contents(nV2, aT2);
    //
    if (myDS->IsNewShape(nV1) || aNbPB>1) {
      aV1=(*(TopoDS_Vertex *)(&myDS->Shape(nV1)));
      aV1.Orientation(TopAbs_FORWARD); 
      //
      aV2=(*(TopoDS_Vertex *)(&myDS->Shape(nV2)));
      aV2.Orientation(TopAbs_REVERSED); 
      //
      MakeSplitEdge1(aDE, aDF, aV1, aT1, aV2, aT2, aSp); 
      //
      aSI.SetShape(aSp);
      nSp=myDS->Append(aSI);
      aPB->SetEdge(nSp);
    }
    else {
      //aPB->SetEdge(nDE);
      aLPB.Clear();
      break;
    }
  }
}

//=======================================================================
//function : FillPaves
//purpose  : 
//=======================================================================
  void BOPAlgo_PaveFiller::FillPaves(const Standard_Integer nVD,
                                     const Standard_Integer nED,
                                     const Standard_Integer nFD,
                                     const BOPDS_ListOfPaveBlock& aLPBOut,
                                     const Handle(BOPDS_PaveBlock)& aPBD)
{
  Standard_Boolean bXDir, bIsDone;
  Standard_Integer nE, aNbPoints, j, anInd;
  Standard_Real aTD1, aTD2, aT1, aT2, aTolInter, aX, aDT;
  Standard_Real aTolCmp;
  gp_Pnt2d aP2d1, aP2d2, aP2D;
  gp_Lin2d aLDE;
  Handle(Geom2d_Line) aCLDE;
  Handle(Geom2d_Curve) aC2DDE1, aC2D;
  Handle(Geom2d_TrimmedCurve)aC2DDE;
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB;
  BOPDS_Pave aPave;
  //
  aDT=Precision::PConfusion();
  //
  aPave.SetIndex(nVD);
  //
  const TopoDS_Edge& aDE=(*(TopoDS_Edge *)(&myDS->Shape(nED)));
  const TopoDS_Face& aDF=(*(TopoDS_Face *)(&myDS->Shape(nFD)));
  //aC2DDE
  aC2DDE1=BRep_Tool::CurveOnSurface(aDE, aDF, aTD1, aTD2);
  aC2DDE=new Geom2d_TrimmedCurve(aC2DDE1, aTD1, aTD2);
  //aCLDE
  Handle(Geom2d_TrimmedCurve) aCLDET1=Handle(Geom2d_TrimmedCurve)::DownCast(aC2DDE1);
  if (aCLDET1.IsNull()) {
    aCLDE=Handle(Geom2d_Line)::DownCast(aC2DDE1);
  }
  else {
    Handle(Geom2d_Curve) aBasisCurve=aCLDET1->BasisCurve();
    aCLDE=Handle(Geom2d_Line)::DownCast(aBasisCurve);
  }
  //
  // Choose direction for degenerated edge
  aC2DDE->D0(aTD1, aP2d1);
  aC2DDE->D0(aTD2, aP2d2);
  //
  bXDir=Standard_False;
  if (fabs(aP2d1.Y()-aP2d2.Y()) < aDT){
    bXDir=!bXDir;
  }
  //
  aItLPB.Initialize(aLPBOut);
  for (; aItLPB.More(); aItLPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB=aItLPB.Value();
    nE=aPB->Edge();
    const TopoDS_Edge& aE=(*(TopoDS_Edge *)(&myDS->Shape(nE)));
    aC2D=BRep_Tool::CurveOnSurface(aE, aDF, aT1, aT2);
    if (aC2D.IsNull()) {
      continue;
    }
    //
    // Intersection
    Geom2dAdaptor_Curve aGAC1, aGAC2;
    aGAC1.Load(aC2DDE, aTD1, aTD2);
    //
    Handle(Geom2d_Line) aL2D= Handle(Geom2d_Line)::DownCast(aC2D);
    if (!aL2D.IsNull()) {
      aGAC2.Load(aC2D);
    }
    else {
      aGAC2.Load(aC2D, aT1, aT2);
    }
    //
    aTolInter=0.001;
    aTolCmp=1.414213562*aTolInter+aDT;
    Geom2dInt_GInter aGInter(aGAC1, aGAC2, aTolInter, aTolInter);
    bIsDone=aGInter.IsDone();
    if(!bIsDone) {
      continue;
    }
    //
    aNbPoints=aGInter.NbPoints();
    if (!aNbPoints){
      continue;
    }
    //
    for (j=1; j<=aNbPoints; ++j) {
      aP2D=aGInter.Point(j).Value();
      aX=aGInter.Point(j).ParamOnFirst();
      //
      if (fabs (aX-aTD1) < aTolCmp || fabs (aX-aTD2) < aTolCmp) {
        continue; 
      }
      if (aX < aTD1 || aX > aTD2) {
        continue; 
      }
      //
      if (aPBD->ContainsParameter(aX, aDT, anInd)) {
        continue;
      }
      aPave.SetParameter(aX);
      aPBD->AppendExtPave1(aPave);
    }
  }//for (; aItLPB.More(); aItLPB.Next()) {
  //
  myDS->UpdatePaveBlock(aPBD);
}
//=======================================================================
// function:  MakeSplitEdge1
// purpose: 
//=======================================================================
  void MakeSplitEdge1 (const TopoDS_Edge&   aE,
                       const TopoDS_Face&   aF,
                       const TopoDS_Vertex& aV1,
                       const Standard_Real  aP1,
                       const TopoDS_Vertex& aV2,
                       const Standard_Real  aP2,
                       TopoDS_Edge& aNewEdge)
{
  Standard_Real aTol=1.e-7;

  TopoDS_Edge E=aE;

  E.EmptyCopy();
  BRep_Builder BB;
  BB.Add  (E, aV1);
  BB.Add  (E, aV2);

  BB.Range(E, aF, aP1, aP2);

  BB.Degenerated(E, Standard_True);

  BB.UpdateEdge(E, aTol);
  aNewEdge=E;
}

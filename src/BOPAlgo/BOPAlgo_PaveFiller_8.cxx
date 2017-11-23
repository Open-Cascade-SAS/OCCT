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
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dInt_GInter.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntTools_Context.hxx>
#include <Precision.hxx>
#include <TColStd_ListOfInteger.hxx>
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

static
  Standard_Boolean AddSplitPoint(const Handle(BOPDS_PaveBlock)& thePBD,
                                 const BOPDS_Pave& thePave,
                                 const Standard_Real theTol);

//=======================================================================
//function : ProcessDE
//purpose  : 
//=======================================================================
void BOPAlgo_PaveFiller::ProcessDE()
{
  Standard_Integer nF, aNb, nE, nV, nVSD, aNbPB;
  Handle(NCollection_BaseAllocator) aAllocator;
  Handle(BOPDS_PaveBlock) aPBD;
  TColStd_ListIteratorOfListOfInteger aItLI;
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
          if (aNbPB) {
            //
            // 2.
            BOPDS_ListOfPaveBlock& aLPBD = myDS->ChangePaveBlocks(nE);
            Standard_ASSERT_VOID(!aLPBD.IsEmpty(), "ListOfPaveBlock is unexpectedly empty");
            if (aLPBD.IsEmpty())
              continue;
            aPBD = aLPBD.First();
            //
            FillPaves(nV, nE, nF, aLPBOut, aPBD);
            //
            myDS->UpdatePaveBlock(aPBD);
          }
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
      myDS->ChangeShapeInfo(nDE).SetReference(-1);
      aLPB.Clear();
      break;
    }
  }
}

//=======================================================================
//function : FillPaves
//purpose  : Find all pave blocks passing through the vertex <nVD> and
//           intersecting the 2D curve of the degenerated edge
//           somewhere in the middle. Save intersection points into
//           Extra paves of the pave block of degenerated edge for future
//           splitting.
//=======================================================================
  void BOPAlgo_PaveFiller::FillPaves(const Standard_Integer nVD,
                                     const Standard_Integer nED,
                                     const Standard_Integer nFD,
                                     const BOPDS_ListOfPaveBlock& aLPBOut,
                                     const Handle(BOPDS_PaveBlock)& aPBD)
{
  // Prepare pave to put to pave block as an Extra pave
  BOPDS_Pave aPave;
  aPave.SetIndex(nVD);
  //
  const TopoDS_Vertex& aDV = (*(TopoDS_Vertex *)(&myDS->Shape(nVD)));
  const TopoDS_Edge& aDE = (*(TopoDS_Edge *)(&myDS->Shape(nED)));
  const TopoDS_Face& aDF = (*(TopoDS_Face *)(&myDS->Shape(nFD)));
  //
  Standard_Real aTolV = BRep_Tool::Tolerance(aDV);
  const BRepAdaptor_Surface& aBAS = myContext->SurfaceAdaptor(aDF);
  //
  // 2D intersection tolerance should be computed as a resolution
  // from the tolerance of vertex to resolve the touching cases
  Standard_Real aTolInt = Precision::PConfusion();
  // UResolution from the tolerance of the vertex
  Standard_Real aURes = aBAS.UResolution(aTolV);
  // VResolution from the tolerance of the vertex
  Standard_Real aVRes = aBAS.VResolution(aTolV);
  //
  aTolInt = Max(aTolInt, Max(aURes, aVRes));
  //
  // Parametric tolerance to compare intersection point with boundaries
  // should be computed as a resolution from the tolerance of vertex
  // in the direction of the 2D curve of degenerated edge
  Standard_Real aTolCmp = Precision::PConfusion();
  // Get 2D curve
  Standard_Real aTD1, aTD2;
  Handle(Geom2d_Curve) aC2DDE = BRep_Tool::CurveOnSurface(aDE, aDF, aTD1, aTD2);
  // Get direction of the curve
  Standard_Boolean bUDir = Abs(aC2DDE->Value(aTD1).Y() - aC2DDE->Value(aTD2).Y()) < Precision::PConfusion();
  //
  aTolCmp = Max(aTolCmp, (bUDir ? aURes : aVRes));
  //
  // Prepare adaptor for the degenerated edge for intersection
  Geom2dAdaptor_Curve aGAC1;
  aGAC1.Load(aC2DDE, aTD1, aTD2);
  //
  BOPDS_ListIteratorOfListOfPaveBlock aItLPB(aLPBOut);
  for (; aItLPB.More(); aItLPB.Next()) {
    const Handle(BOPDS_PaveBlock)& aPB = aItLPB.Value();
    Standard_Integer nE = aPB->Edge();
    if (nE < 0) {
      continue;
    }
    const TopoDS_Edge& aE = (*(TopoDS_Edge *)(&myDS->Shape(nE)));
    Standard_Real aT1, aT2;
    Handle(Geom2d_Curve) aC2D = BRep_Tool::CurveOnSurface(aE, aDF, aT1, aT2);
    if (aC2D.IsNull()) {
      continue;
    }
    //
    // Prepare adaptor for the passing edge for intersection
    Geom2dAdaptor_Curve aGAC2;
    //
    Handle(Geom2d_Line) aL2D = Handle(Geom2d_Line)::DownCast(aC2D);
    if (!aL2D.IsNull()) {
      aGAC2.Load(aC2D);
    }
    else {
      aGAC2.Load(aC2D, aT1, aT2);
    }
    // Intersection
    Geom2dInt_GInter aGInter(aGAC1, aGAC2, aTolInt, aTolInt);
    if (aGInter.IsDone() && aGInter.NbPoints())
    {
      // Analyze intersection points
      Standard_Integer i, aNbPoints = aGInter.NbPoints();
      for (i = 1; i <= aNbPoints; ++i) {
        Standard_Real aX = aGInter.Point(i).ParamOnFirst();
        aPave.SetParameter(aX);
        AddSplitPoint(aPBD, aPave, aTolCmp);
      }
    }
    else
    {
      // If the intersection did not succeed, try the projection of the end point
      // of the curve corresponding to the vertex of degenerated edge
      Standard_Real aT = (nVD == aPB->Pave1().Index() ?
        aPB->Pave1().Parameter() : aPB->Pave2().Parameter());
      gp_Pnt2d aP2d = aC2D->Value(aT);
      Geom2dAPI_ProjectPointOnCurve aProj2d(aP2d, aC2DDE, aTD1, aTD2);
      if (aProj2d.NbPoints())
      {
        Standard_Real aX = aProj2d.LowerDistanceParameter();
        aPave.SetParameter(aX);
        AddSplitPoint(aPBD, aPave, aTolCmp);
      }
    }
  }
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

//=======================================================================
// function: AddSplitPoint
// purpose: Validates the point represented by the pave <thePave>
//          for the Pave Block <thePBD>.
//          In case the point passes the checks it is added as an
//          Extra Pave to the Pave Block for further splitting of the latter.
//          Returns TRUE if the point is added, otherwise returns FALSE.
//=======================================================================
Standard_Boolean AddSplitPoint(const Handle(BOPDS_PaveBlock)& thePBD,
                               const BOPDS_Pave& thePave,
                               const Standard_Real theTol)
{
  Standard_Real aTD1, aTD2;
  thePBD->Range(aTD1, aTD2);

  Standard_Real aT = thePave.Parameter();
  // Check that the parameter is inside the Pave Block
  if (aT - aTD1 < theTol || aTD2 - aT < theTol)
    return Standard_False;

  // Check that the pave block does not contain the same parameter
  Standard_Integer anInd;
  if (thePBD->ContainsParameter(aT, theTol, anInd))
    return Standard_False;

  // Add the point as an Extra pave to the Pave Block for further
  // splitting of the latter
  thePBD->AppendExtPave1(thePave);
  return Standard_True;
}

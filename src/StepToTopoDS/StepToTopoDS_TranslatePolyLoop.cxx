// Created on: 1995-01-03
// Created by: Frederic MAUPAS
// Copyright (c) 1995-1999 Matra Datavision
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

//gka,abv 14.09.99: S4136: eliminate BRepAPI::Precision()

#include <BRep_Builder.hxx>
#include <ElSLib.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_CartesianPoint.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepShape_PolyLoop.hxx>
#include <StepToGeom.hxx>
#include <StepToTopoDS_PointPair.hxx>
#include <StepToTopoDS_Tool.hxx>
#include <StepToTopoDS_TranslatePolyLoop.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_TransientProcess.hxx>

//#include <BRepAPI.hxx>
// ============================================================================
// Method  :
// Purpose :
// ============================================================================
StepToTopoDS_TranslatePolyLoop::StepToTopoDS_TranslatePolyLoop()
: myError(StepToTopoDS_TranslatePolyLoopOther)
{
  done = Standard_False;
}

// ============================================================================
// Method  :
// Purpose :
// ============================================================================

StepToTopoDS_TranslatePolyLoop::StepToTopoDS_TranslatePolyLoop(const Handle(StepShape_PolyLoop)& PL, StepToTopoDS_Tool& T, const Handle(Geom_Surface)& S, const TopoDS_Face& F)
{
  Init (PL, T, S, F);
}

// ============================================================================
// Method  :
// Purpose :
// ============================================================================

void StepToTopoDS_TranslatePolyLoop::Init(const Handle(StepShape_PolyLoop)& PL, StepToTopoDS_Tool& aTool, const Handle(Geom_Surface)& GeomSurf, const TopoDS_Face& TopoFace)
{
  if (!aTool.IsBound(PL)) {
    BRep_Builder B;
    Handle(Transfer_TransientProcess) TP = aTool.TransientProcess();

//:S4136    Standard_Real preci = BRepAPI::Precision();
    Standard_Integer i;
    Handle(StepGeom_CartesianPoint) P1,P2;
    Handle(Geom_CartesianPoint) GP1, GP2;
    TopoDS_Vertex V1, V2;
    TopoDS_Edge E;
    TopoDS_Wire W;
    Handle(Geom_Line) L;
    Handle(Geom2d_Line) L2d;
    gp_Vec V;
    gp_Vec2d V2d;
    Standard_Real Magn;
    Handle(Geom_Plane) SP = Handle(Geom_Plane)::DownCast(GeomSurf);
    if (SP.IsNull()) TP->AddFail(PL,"Surface not planar in a FacetedBRep !");
    Handle(ShapeAnalysis_Surface) STSU = new ShapeAnalysis_Surface ( GeomSurf );
    Standard_Integer Nb = PL->NbPolygon();
    Handle(StepGeom_HArray1OfCartesianPoint) Poly = 
      new StepGeom_HArray1OfCartesianPoint(1, Nb+1);

    for ( i=1; i<=Nb; i++ )
      Poly->SetValue(i, PL->PolygonValue(i));

    Nb++;
    Poly->SetValue(Nb, PL->PolygonValue(1));
    P1 = Poly->Value(1);
    GP1 = StepToGeom::MakeCartesianPoint (P1);
    if (aTool.IsVertexBound(P1)) {
      V1 = aTool.FindVertex(P1);
    }
    else {
      B.MakeVertex(V1, GP1->Pnt(), Precision::Confusion()); //:S4136: preci
      aTool.BindVertex(P1,V1);
    }
    B.MakeWire(W);
    for ( i=2; i<=Nb; i++){
      P2 = Poly->Value(i);
      if (P1 == P2) continue;  // peut arriver (KK)  CKY 9-DEC-1997
      StepToTopoDS_PointPair PP(P1, P2); 
      GP2 = StepToGeom::MakeCartesianPoint (P2);
      TopoDS_Shape aBoundEdge;
      Standard_Boolean isbound = aTool.IsEdgeBound(PP);
      if (!isbound) {
        if (aTool.IsVertexBound(P2)) {
          V2 = aTool.FindVertex(P2);
        }
        else {
          B.MakeVertex(V2, GP2->Pnt(), Precision::Confusion()); //:S4136: preci
          aTool.BindVertex(P2, V2);
        }
        V = gp_Vec(GP1->Pnt(), GP2->Pnt());
        L = new Geom_Line(GP1->Pnt(), gp_Dir(V));
        B.MakeEdge(E, L, Precision::Confusion()); //:S4136: preci
        V1.Orientation(TopAbs_FORWARD);
        V2.Orientation(TopAbs_REVERSED);
        B.Add(E, V1);
        B.Add(E, V2);
        Magn = V.Magnitude();
        B.UpdateVertex(V1, 0., E, 0.); //:S4136: preci
        B.UpdateVertex(V2, Magn, E, 0.); //:S4136: preci
      }
      else {
        aBoundEdge = aTool.FindEdge(PP);
        E = TopoDS::Edge(aBoundEdge);
        //  Il faut qu en finale l edge soit vue
        //  - via sa premiere face, orientation combinee = celle de cette premiere face
        //  - via sa deuxieme face, orientation combinee INVERSE de la precedente
        if (TopoFace.Orientation() == TopAbs_FORWARD) E.Reverse();
        V2 = aTool.FindVertex(P2);
      }
      gp_Pnt2d V2p1 = STSU->ValueOfUV (GP1->Pnt(), Precision());
      gp_Pnt2d V2p2 = STSU->ValueOfUV (GP2->Pnt(), Precision());
      if (E.Orientation() == TopAbs_FORWARD) {
        V2d = gp_Vec2d(V2p1, V2p2);
        L2d = new Geom2d_Line(V2p1, gp_Dir2d(V2d));
      }
      else {
        V2d = gp_Vec2d(V2p2, V2p1);
        L2d = new Geom2d_Line(V2p2, gp_Dir2d(V2d));
      }
      B.UpdateEdge(E, L2d, TopoFace, 0.);
      TopoDS_Edge EB = E;  // pour le binding : cumul des orientations !
      EB.Orientation (TopoFace.Orientation());
      if (!isbound) aTool.BindEdge(PP, EB);
      if (!E.IsNull()) {
        B.Add(W, E);
      }
      P1  = P2;
      GP1 = GP2;
      V1  = V2;
    }
    W.Closed (BRep_Tool::IsClosed (W));
    aTool.Bind(PL, W);
    myResult = W;
    myError  = StepToTopoDS_TranslatePolyLoopDone;
    done     = Standard_True;
  }
  else {
    myResult = TopoDS::Wire(aTool.Find(PL));
    myError  = StepToTopoDS_TranslatePolyLoopDone;
    done     = Standard_True;    
  }
}

// ============================================================================
// Method  :
// Purpose :
// ============================================================================

const TopoDS_Shape& StepToTopoDS_TranslatePolyLoop::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "StepToTopoDS_TranslatePolyLoop::Value() - no result");
  return myResult;
}

// ============================================================================
// Method  :
// Purpose :
// ============================================================================

StepToTopoDS_TranslatePolyLoopError StepToTopoDS_TranslatePolyLoop::Error() const
{
  return myError;
}

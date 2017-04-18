// Created on: 1994-11-30
// Created by: Frederic MAUPAS
// Copyright (c) 1994-1999 Matra Datavision
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


#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepLib.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <GeomToStep_MakeCurve.hxx>
#include <GeomToStep_MakeLine.hxx>
#include <gp_Vec.hxx>
#include <Interface_Static.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_HArray1OfPcurveOrSurface.hxx>
#include <StepGeom_Line.hxx>
#include <StepGeom_SeamCurve.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepShape_EdgeCurve.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepShape_Vertex.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDSToStep_MakeStepEdge.hxx>
#include <TopoDSToStep_MakeStepVertex.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep.hxx>
#include <TransferBRep_ShapeMapper.hxx>

// Processing of non-manifold topology (ssv; 11.11.2010)
// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------
TopoDSToStep_MakeStepEdge::TopoDSToStep_MakeStepEdge()
{
  done = Standard_False;
}

TopoDSToStep_MakeStepEdge::TopoDSToStep_MakeStepEdge
(const TopoDS_Edge& E,
 TopoDSToStep_Tool& T,
 const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;
  Init(E, T, FP);
}

// ----------------------------------------------------------------------------
// Method  : Init
// Purpose :
// ----------------------------------------------------------------------------

void TopoDSToStep_MakeStepEdge::Init(const TopoDS_Edge& aEdge, 
                                     TopoDSToStep_Tool& aTool,
                                     const Handle(Transfer_FinderProcess)& FP)
{
  // ------------------------------------------------------------------
  // The edge is given with its relative orientation (i.e. in the wire)
  // ------------------------------------------------------------------

  aTool.SetCurrentEdge(aEdge);

  // [BEGIN] Processing non-manifold topology (ssv; 11.11.2010)
  Standard_Boolean isNMMode = Interface_Static::IVal("write.step.nonmanifold") != 0;
  if (isNMMode) {
    Handle(StepShape_EdgeCurve) anEC;
    Handle(TransferBRep_ShapeMapper) aSTEPMapper = TransferBRep::ShapeMapper(FP, aEdge);
    if ( FP->FindTypedTransient(aSTEPMapper, STANDARD_TYPE(StepShape_EdgeCurve), anEC) ) {
      // Non-manifold topology detected
      myError  = TopoDSToStep_EdgeDone;
      myResult = anEC;
      done     = Standard_True;
      return;
    }
  }
  // [END] Processing non-manifold topology (ssv; 11.11.2010)

  if (aTool.IsBound(aEdge)) {
    myError  = TopoDSToStep_EdgeDone;
    done     = Standard_True;
    myResult = aTool.Find(aEdge);
    return;
  }

#define Nbpt 21
  TopoDS_Iterator  It;
  Standard_Integer i;
  Standard_Real    U, U1, U2;
  gp_Pnt           P;
  
  Standard_Boolean isSeam = BRep_Tool::IsClosed(aEdge, aTool.CurrentFace());
  
  //:i4 abv 02 Sep 98: ProSTEP TR8 Motor.rle f3 & f62: check that edge 
  // participates twice in the wires of the face before making it seam
  // (else it can have two pcurves on the same surface being shared by 
  // two faces on that surface)
  // This fix is necessary because sharing of surfaces is not preserved when 
  // writing faces to STEP (see TopoDSToSTEP_MakeStepFace)
  if ( isSeam ) {
    Standard_Integer count = 0;
    TopExp_Explorer exp ( aTool.CurrentFace(), TopAbs_EDGE );
    for ( ; exp.More(); exp.Next() )
      if ( aEdge.IsSame ( exp.Current() ) ) count++;
    if ( count < 2 ) isSeam = Standard_False;
  }

  BRepAdaptor_Curve   CA = BRepAdaptor_Curve(aEdge);
  BRepAdaptor_Surface SA = BRepAdaptor_Surface(aTool.CurrentFace());

  if (aEdge.Orientation() == TopAbs_INTERNAL  ||
      aEdge.Orientation() == TopAbs_EXTERNAL ) {
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aEdge);
    FP->AddWarning(errShape, " Edge(internal/external) from Non Manifold Topology");
    myError = TopoDSToStep_NonManifoldEdge;
    done    = Standard_False;
    return;
  }
  
  // Vertices

  Handle(StepShape_Vertex) V1,V2;
  Handle(StepShape_TopologicalRepresentationItem) Gpms2;
  TopoDS_Vertex Vfirst, Vlast;

  TopExp::Vertices(aEdge,Vfirst, Vlast);

  TopoDSToStep_MakeStepVertex MkVertex;
  
  MkVertex.Init(Vfirst, aTool, FP);  
  if (MkVertex.IsDone())
    V1 = Handle(StepShape_Vertex)::DownCast(MkVertex.Value());
  else {
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aEdge);
    FP->AddWarning(errShape, " First Vertex of Edge not mapped");
    myError = TopoDSToStep_EdgeOther;
    done    = Standard_False;
    return;
  }
  
  MkVertex.Init(Vlast, aTool, FP);
  if (MkVertex.IsDone())
    V2 = Handle(StepShape_Vertex)::DownCast(MkVertex.Value());
  else {
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aEdge);
    FP->AddWarning(errShape, " Last Vertex of Edge not mapped");
    myError = TopoDSToStep_EdgeOther;
    done    = Standard_False;
    return;
  }
  
  // ---------------------------------------
  // Translate 3D representation of the Edge
  // ---------------------------------------
  
  Handle(StepGeom_Curve) Gpms;
  Handle(Geom_Curve) C = CA.Curve().Curve();
  if (!C.IsNull()) {
    C = Handle(Geom_Curve)::DownCast(C->Copy());
    gp_Trsf Tr1 = CA.Trsf();
    C->Transform(Tr1);
    GeomToStep_MakeCurve MkCurve(C);
    Gpms = MkCurve.Value();
  }
  else {
    
    // -------------------------
    // a 3D Curve is constructed 
    // -------------------------

#ifdef OCCT_DEBUG
    cout << "Warning: TopoDSToStep_MakeStepEdge: edge without 3d curve; creating..." << endl;
#endif
    if ((SA.GetType() == GeomAbs_Plane) &&
	(CA.GetType() == GeomAbs_Line)) {
      U1 = CA.FirstParameter();
      U2 = CA.LastParameter();
      gp_Vec V = gp_Vec( CA.Value(U1), CA.Value(U2) );
      Handle(Geom_Line) L = 
	new Geom_Line(CA.Value(U1), gp_Dir(V));
      GeomToStep_MakeLine MkLine(L);
      Gpms = MkLine.Value();
    }
    else {
      // To Be Optimized : create an approximated BSpline
      //                   using GeomAPI_PointsToBSpline
      TColgp_Array1OfPnt Points(1,Nbpt);
      TColStd_Array1OfReal Knots(1,Nbpt);
      TColStd_Array1OfInteger Mult(1,Nbpt);
      U1 = CA.FirstParameter();
      U2 = CA.LastParameter();
      for ( i=1; i<=Nbpt; i++ ) {
	U = U1 + (i-1)*(U2 - U1)/(Nbpt - 1);
	P = CA.Value(U);
	Points.SetValue(i,P);
	Knots.SetValue(i,U);
	Mult.SetValue(i,1);
      }
      //Points.SetValue(1, BRep_Tool::Pnt(Vfirst));
      //Points.SetValue(Nbpt, BRep_Tool::Pnt(Vlast));
      Mult.SetValue(1,2);
      Mult.SetValue(Nbpt,2);
      Handle(Geom_Curve) Bs = 
	new Geom_BSplineCurve(Points, Knots, Mult, 1);
      GeomToStep_MakeCurve MkCurve(Bs);
      Gpms = MkCurve.Value();
    }
  }
  
  // ---------------------------------------------------------
  // Warning : if the edge is connected aGeom->Length = 2
  //           otherwise = 1 ;
  //           and enumeration is pscrPcurveS2 or pscrPcurveS1
  // This is corrected in the Write File phases !
  // ---------------------------------------------------------  

  //:abv 25.01.00 CAX-IF TRJ3
  // if PcurveMode is 1 (default), make surface_curve instead of simple 3d curve
  if ( aTool.PCurveMode() != 0 ) {
  
    Handle(StepGeom_HArray1OfPcurveOrSurface) aGeom =
      new StepGeom_HArray1OfPcurveOrSurface(1,2);
    Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
  
    if (!isSeam) {
      Handle(StepGeom_SurfaceCurve) SurfaceCurve = new StepGeom_SurfaceCurve;
      SurfaceCurve->Init(aName, Gpms, aGeom, StepGeom_pscrPcurveS1);
      Gpms = SurfaceCurve;
    }
    else {
      Handle(StepGeom_SeamCurve) SeamCurve = new StepGeom_SeamCurve;
      SeamCurve->Init(aName, Gpms, aGeom, StepGeom_pscrPcurveS1);
      Gpms = SeamCurve;
    }
  }
  
  // Edge curve
  Handle(StepShape_EdgeCurve) Epms = new StepShape_EdgeCurve;
  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
  Epms->Init(aName, V1, V2, Gpms, Standard_True);
  
  aTool.Bind(aEdge, Epms);
  myError  = TopoDSToStep_EdgeDone;
  myResult = Epms;
  done     = Standard_True;
  return;
}

// ----------------------------------------------------------------------------
// Method  : Value
// Purpose :
// ----------------------------------------------------------------------------

const Handle(StepShape_TopologicalRepresentationItem)& TopoDSToStep_MakeStepEdge::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeStepEdge::Value() - no result");
  return myResult;
}

// ----------------------------------------------------------------------------
// Method  : Error
// Purpose :
// ----------------------------------------------------------------------------

TopoDSToStep_MakeEdgeError TopoDSToStep_MakeStepEdge::Error() const 
{
  return myError;
}

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

//szv#4 S4163

#include <BRep_Tool.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <gp_Pnt.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Wire.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray1OfCartesianPoint.hxx>
#include <StepShape_Edge.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_HArray1OfOrientedEdge.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_PolyLoop.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepShape_Vertex.hxx>
#include <StepShape_VertexLoop.hxx>
#include <StepShape_VertexPoint.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDSToStep_MakeStepEdge.hxx>
#include <TopoDSToStep_MakeStepVertex.hxx>
#include <TopoDSToStep_MakeStepWire.hxx>
#include <TopoDSToStep_Tool.hxx>
#include <Transfer_FinderProcess.hxx>
#include <TransferBRep_ShapeMapper.hxx>

// ----------------------------------------------------------------------------
// Constructors
// ----------------------------------------------------------------------------
TopoDSToStep_MakeStepWire::TopoDSToStep_MakeStepWire()
{
  done = Standard_False;
}

TopoDSToStep_MakeStepWire::TopoDSToStep_MakeStepWire
(const TopoDS_Wire& W, 
 TopoDSToStep_Tool& T,
 const Handle(Transfer_FinderProcess)& FP)
{
  done = Standard_False;
  Init(W, T, FP);
}


// ----------------------------------------------------------------------------
// Method  : Init
// Purpose :
// ----------------------------------------------------------------------------

void TopoDSToStep_MakeStepWire::Init(const TopoDS_Wire& aWire, 
				    TopoDSToStep_Tool& aTool,
				    const Handle(Transfer_FinderProcess)& FP)
{

  // ----------------------------------------------------------------
  // The Wire is given in its relative orientation (i.e. in the face)
  // ----------------------------------------------------------------

  aTool.SetCurrentWire(aWire);

  if (aTool.IsBound(aWire)) {
    myError  = TopoDSToStep_WireDone;
    done     = Standard_True;
    myResult = aTool.Find(aWire);
    return;
  }

  TopoDS_Iterator  It;
  Standard_Integer i;
  
  if (aWire.Orientation() == TopAbs_INTERNAL ||
      aWire.Orientation() == TopAbs_EXTERNAL ) {
    Handle(TransferBRep_ShapeMapper) errShape =
      new TransferBRep_ShapeMapper(aWire);
    FP->AddWarning(errShape, " Wire(internal/external) from Non Manifold Topology");
    myError = TopoDSToStep_NonManifoldWire;
    done    = Standard_False;
    return;
  }

  BRepTools_WireExplorer      ItW;
  TopoDS_Edge                 CurrentEdge;
  TColStd_SequenceOfTransient mySeq;
  
  // --------
  // Polyloop
  // --------
  
  if (aTool.Faceted()) {
    Handle(StepShape_VertexPoint)                   VertexPoint;
    Handle(StepGeom_Point)                         Point;
    Handle(StepShape_TopologicalRepresentationItem) Gpms;
    TopoDS_Vertex                               TopoDSVertex1, TopoDSVertex2;
    
    TopoDSToStep_MakeStepVertex MkVertex;
//    TopoDS_Wire ForwardWire = TopoDS::Wire(aWire.Oriented(TopAbs_FORWARD));
    
    for (ItW.Init(aWire, aTool.CurrentFace()); 
	 ItW.More();ItW.Next()) {
      CurrentEdge = ItW.Current();
      if (CurrentEdge.Orientation() == TopAbs_FORWARD)
	TopExp::Vertices(CurrentEdge, TopoDSVertex1, TopoDSVertex2);
      else 
	TopExp::Vertices(CurrentEdge, TopoDSVertex2, TopoDSVertex1);
      
      MkVertex.Init(TopoDSVertex1, aTool, FP);
      if (MkVertex.IsDone()) {
	VertexPoint = Handle(StepShape_VertexPoint)::DownCast(MkVertex.Value());
	Point = VertexPoint->VertexGeometry();
	mySeq.Append(Point);
      }
      else {
	Handle(TransferBRep_ShapeMapper) errShape =
	  new TransferBRep_ShapeMapper(aWire);
	FP->AddWarning(errShape, " a Vertex Point not mapped");
	myError = TopoDSToStep_WireOther;
	done = Standard_False;
	return;
      }
    }
    Standard_Integer nbPoints = mySeq.Length();
    if (nbPoints>=3) {
      Handle(StepGeom_HArray1OfCartesianPoint) aPolygon =
	new StepGeom_HArray1OfCartesianPoint(1,nbPoints);
      for ( i=1; i<=nbPoints; i++) {
	aPolygon->SetValue(i, Handle(StepGeom_CartesianPoint)::
			   DownCast(mySeq.Value(i))); 
      }
      Handle(StepShape_PolyLoop) PL = new StepShape_PolyLoop();
      Handle(TCollection_HAsciiString) aName = 
	new TCollection_HAsciiString("");
      PL->Init(aName, aPolygon);
      
      aTool.Bind(aWire, PL);
      myError  = TopoDSToStep_WireDone;
      done     = Standard_True;
      myResult = PL;
      return;
    }
    else {
      Handle(TransferBRep_ShapeMapper) errShape =
	new TransferBRep_ShapeMapper(aWire);
      FP->AddWarning(errShape, " PolyLoop: Wire has less than 3 points");
      myError = TopoDSToStep_WireOther;
      done = Standard_False;
      return;
    }
  }
  
  // --------
  // EdgeLoop
  // --------
  
  else {

    Handle(StepShape_TopologicalRepresentationItem) Gpms;
    Handle(StepShape_Edge)                          Epms;
    Handle(StepShape_OrientedEdge)                  OrientedEdge;
    
    TopoDSToStep_MakeStepEdge MkEdge;

    //szv#4:S4163:12Mar99 SGI warns
    TopoDS_Shape sh = aWire.Oriented(TopAbs_FORWARD);
    const TopoDS_Wire ForwardWire = TopoDS::Wire(sh);
    // test 25-01-96 FMA  supprime CKY 2-JUN-1997, cf MakeStepFace->Face FWD]
    // remis CKY 9-DEC-1997 : chaque niveau se traite en FWD
//#11 rln 16/03/98
//TestRally8 file carter2.rle face#333 (wire is not sorted, not sorted edges are seam and iso-curve):
//aWire is REVERSED but ForwardWire is FORWARD, when exploding not connected seams their pcurves are
//returned in incorrect order (because of mismatched orientation)
//As a result not sorted edges are lost (not returned by BRepTools_WireExplorer)
//By the way, in the case of aTool.Faceted() aWire is used

//#11 ItW.Init(ForwardWire, aTool.CurrentFace());
//#11 for (;ItW.More();ItW.Next()) {
    Handle(ShapeFix_Wire) STW = new ShapeFix_Wire;
    STW->Load (ForwardWire);
    STW->FixReorder();
    Handle(ShapeExtend_WireData) sbwd = STW->WireData();
    Standard_Integer nb = sbwd->NbEdges();
    
    //:abv 04.05.00: CAX-IF TRJ4: writing complete sphere with single vertex_loop
    // check that whole wire is one seam (perhaps made of several seam edges)
    //pdn remove degenerated pcurves
    Handle(ShapeExtend_WireData) cwd = new ShapeExtend_WireData;
    Standard_Integer ie;
    for (ie = 1; ie <=nb; ie++) {
      TopoDS_Edge edge = sbwd->Edge(ie);
      if (!BRep_Tool::Degenerated(edge))
	cwd->Add(edge);
    }

    nb = cwd->NbEdges();
    if(nb%2 == 0 ) {
      for ( ie = 1; ie < nb; ie++) {
	if ( cwd->Edge(ie).IsSame(cwd->Edge(ie+1)) ) break;
      }
      if ( ie < nb ) {
	cwd->SetLast(ie);
	for ( ie=nb/2+1; ie <= nb; ie++ ) {
	  if ( ! cwd->Edge(ie).IsSame(cwd->Edge(nb-ie+1)) ) break;
	}
	if ( ie > nb ) { // make vertex_loop
	  ShapeAnalysis_Edge sae;
	  TopoDS_Vertex V = sae.FirstVertex(cwd->Edge(1));
	  TopoDSToStep_MakeStepVertex mkV ( V, aTool, FP );
	  Handle(StepShape_VertexLoop) vloop = new StepShape_VertexLoop;
	  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString ( "" );
	  vloop->Init ( name, Handle(StepShape_Vertex)::DownCast ( mkV.Value() ) );
	  aTool.Bind(aWire, vloop);
	  myError  = TopoDSToStep_WireDone;
	  done     = Standard_True;
	  myResult = vloop;
	  return;
	}
      }
    }
    nb = sbwd->NbEdges();
    
    for (Standard_Integer nEdge = 1; nEdge <= sbwd->NbEdges(); nEdge++) {
  
      CurrentEdge = sbwd->Edge(nEdge);
//#11 CurrentEdge = ItW.Current();
      
      //if (ItW.Current().Orientation() != ItW.Orientation())
      //cout << "DEBUG : Attention WireExplorer Orientation" << endl;

      // ---------------------------------
      // --- Is the edge Degenerated ? ---
      // ---------------------------------

      Standard_Real cf, cl;
      Handle(Geom2d_Curve) theC2d = 
	BRep_Tool::CurveOnSurface(CurrentEdge, aTool.CurrentFace(), cf, cl);
      //BRepAdaptor_Curve CA;
      //CA = BRepAdaptor_Curve(CurrentEdge, 
      //aTool.CurrentFace());
      //GeomAbs_CurveType typC = CA.CurveOnSurface().GetCurve().GetType();
      //if (typC == GeomAbs_Line && BRep_Tool::Degenerated(CurrentEdge)) {
	//Handle(TransferBRep_ShapeMapper) errShape =
	  //new TransferBRep_ShapeMapper(aWire);
	//FP->AddWarning(errShape, " EdgeLoop: Degenerated Pcurve not mapped");
      //}
      if ( //:abv 26Jan00, CAX-IF TRJ3: ! theC2d.IsNull() && theC2d->IsKind(STANDARD_TYPE(Geom2d_Line)) && 
	  BRep_Tool::Degenerated(CurrentEdge)) {
	Handle(TransferBRep_ShapeMapper) errShape =
	  new TransferBRep_ShapeMapper(aWire);
	FP->AddWarning(errShape, " EdgeLoop: Degenerated Pcurve not mapped");
	continue;
      }
      else {
	//szv#4:S4163:12Mar99 SGI warns
	//TopoDS_Shape ssh = CurrentEdge.Oriented(TopAbs_FORWARD);
	//const TopoDS_Edge ForwardEdge = TopoDS::Edge(ssh);

	MkEdge.Init(CurrentEdge, aTool, FP);
	if (MkEdge.IsDone()) {
	  OrientedEdge = new StepShape_OrientedEdge();
	  Epms = Handle(StepShape_Edge)::DownCast(MkEdge.Value());
	  Handle(TCollection_HAsciiString) aName = new TCollection_HAsciiString("");
	  OrientedEdge->Init(aName, Epms, (CurrentEdge.Orientation() == TopAbs_FORWARD));
	  mySeq.Append(OrientedEdge);
	}
	else {
	  Handle(TransferBRep_ShapeMapper) errShape =
	    new TransferBRep_ShapeMapper(aWire);
	  FP->AddWarning(errShape, " EdgeLoop: an Edge not mapped");
	  myError = TopoDSToStep_WireOther;
	  done    = Standard_False;
	  return;
	}
      }
    }
    Standard_Integer nbEdges = mySeq.Length();
    if ( nbEdges >0 ) {
      Handle(StepShape_HArray1OfOrientedEdge) aList =
	new StepShape_HArray1OfOrientedEdge(1,nbEdges);
      for ( i=1; i<=nbEdges; i++ ) {
	aList->SetValue(i, Handle(StepShape_OrientedEdge)::
			DownCast(mySeq.Value(i))); 
      }
      Handle(StepShape_EdgeLoop) Epmsl = new StepShape_EdgeLoop;
      Handle(TCollection_HAsciiString) aName = 
	new TCollection_HAsciiString("");
      Epmsl->Init(aName, aList);
      aTool.Bind(aWire, Epmsl);
      done = Standard_True;
      myResult = Epmsl;   
      return;
    }
    else {
      Handle(TransferBRep_ShapeMapper) errShape =
	new TransferBRep_ShapeMapper(aWire);
      FP->AddWarning(errShape, " No Edges of this Wire were mapped");
      myError = TopoDSToStep_WireOther;
      done = Standard_False;
      return;
    }
  }
}

// ----------------------------------------------------------------------------
// Method  : Value
// Purpose :
// ----------------------------------------------------------------------------

const Handle(StepShape_TopologicalRepresentationItem)& TopoDSToStep_MakeStepWire::Value() const 
{
  StdFail_NotDone_Raise_if (!done, "TopoDSToStep_MakeStepWire::Value() - no result");
  return myResult;
}

// ----------------------------------------------------------------------------
// Method  : Error
// Purpose :
// ----------------------------------------------------------------------------

TopoDSToStep_MakeWireError TopoDSToStep_MakeStepWire::Error() const 
{
  return myError;
}


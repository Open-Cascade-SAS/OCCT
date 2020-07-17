// Created on: 1999-02-12
// Created by: Andrey BETENEV
// Copyright (c) 1999-1999 Matra Datavision
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

//:o3 abv 17.02.99: r0301_db.stp #57082: apply FixReorder to composite curve
//:s5 abv 22.04.99  Adding debug printouts in catch {} blocks

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Interface_Static.hxx>
#include <Precision.hxx>
#include <ShapeExtend_WireData.hxx>
#include <ShapeFix_Wire.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <StepGeom_CompositeCurve.hxx>
#include <StepGeom_CompositeCurveSegment.hxx>
#include <StepGeom_Pcurve.hxx>
#include <StepGeom_PcurveOrSurface.hxx>
#include <StepGeom_Surface.hxx>
#include <StepGeom_SurfaceCurve.hxx>
#include <StepToGeom.hxx>
#include <StepToTopoDS_TranslateCompositeCurve.hxx>
#include <StepToTopoDS_TranslateEdge.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Wire.hxx>
#include <Transfer_TransientProcess.hxx>

//=======================================================================
//function : Create
//purpose  : 
//=======================================================================
StepToTopoDS_TranslateCompositeCurve::StepToTopoDS_TranslateCompositeCurve ()
: myInfiniteSegment(Standard_False)
{
}
	
//=======================================================================
//function : Create
//purpose  : 
//=======================================================================

StepToTopoDS_TranslateCompositeCurve::StepToTopoDS_TranslateCompositeCurve (
				      const Handle(StepGeom_CompositeCurve) &CC,
				      const Handle(Transfer_TransientProcess) &TP)
{
  Init ( CC, TP );
}

//=======================================================================
//function : Create
//purpose  : 
//=======================================================================

StepToTopoDS_TranslateCompositeCurve::StepToTopoDS_TranslateCompositeCurve (
				      const Handle(StepGeom_CompositeCurve) &CC,
				      const Handle(Transfer_TransientProcess) &TP,
				      const Handle(StepGeom_Surface) &S,
				      const Handle(Geom_Surface) &Surf)
{
  Init ( CC, TP, S, Surf );
}
	
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean StepToTopoDS_TranslateCompositeCurve::Init (const Handle(StepGeom_CompositeCurve) &CC,
							     const Handle(Transfer_TransientProcess) &TP)
{
  Handle(StepGeom_Surface) S;
  Handle(Geom_Surface) Surf;
  return Init ( CC, TP, S, Surf );
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

Standard_Boolean StepToTopoDS_TranslateCompositeCurve::Init (const Handle(StepGeom_CompositeCurve) &CC,
							     const Handle(Transfer_TransientProcess) &TP,
							     const Handle(StepGeom_Surface) &S,
							     const Handle(Geom_Surface) &Surf)
{
  myWire.Nullify();
  myInfiniteSegment = Standard_False;
  if ( CC.IsNull() ) return Standard_False;

  Standard_Boolean SurfMode = ( ! S.IsNull() && ! Surf.IsNull() );
  Standard_Boolean isClosed = Standard_False;

  if ( SurfMode ) {
    Standard_Integer modepcurve = Interface_Static::IVal("read.surfacecurve.mode");
    if ( modepcurve ==-3 ) SurfMode = Standard_False;
  }
  
  Handle(ShapeExtend_WireData) sbwd = new ShapeExtend_WireData;
  Standard_Integer nbs = CC->NbSegments();
  for ( Standard_Integer i=1; i <= nbs; i++ ) {
    Handle(StepGeom_CompositeCurveSegment) ccs = CC->SegmentsValue ( i );
    if ( ccs.IsNull() ) {
      TP->AddFail ( CC, "Null segment" );
      return Standard_False;
    }
    Handle(StepGeom_Curve) crv = ccs->ParentCurve();
    if ( crv.IsNull() ) {
      TP->AddFail ( CC, "Segment has null parent curve" );
      return Standard_False;
    }
    isClosed = ( ccs->Transition() != StepGeom_tcDiscontinuous );

    // if segment is itself a composite_curve, translate recursively
    if ( crv->IsKind(STANDARD_TYPE(StepGeom_CompositeCurve)) ) { 
      if (crv == CC) { // cyclic reference protection
        TP->AddFail(ccs, "Cyclic reference; segment dropped");
        continue;
      }
      Handle(StepGeom_CompositeCurve) cc = Handle(StepGeom_CompositeCurve)::DownCast ( crv );
      if ( ! Init ( cc, TP, S, Surf ) || myWire.IsNull() )
        continue;
      Standard_Integer nb = sbwd->NbEdges() + 1;
      for (TopoDS_Iterator it(myWire); it.More(); it.Next()) {
        TopoDS_Edge edge = TopoDS::Edge(it.Value());
        if (ccs->SameSense()) sbwd->Add(edge);
        else {
          edge.Reverse();
          sbwd->Add(edge, nb > sbwd->NbEdges() ? 0 : nb);
        }
      }
      myWire.Nullify();
      continue;
    }
    
    // ordinary segment
    
    // detect pcurve and 3d curve
    Handle(StepGeom_Pcurve) pcurve = Handle(StepGeom_Pcurve)::DownCast ( crv );
    if (pcurve.IsNull()) {
      Handle(StepGeom_SurfaceCurve) sc = Handle(StepGeom_SurfaceCurve)::DownCast(crv);
      if (!sc.IsNull()) {
        crv = sc->Curve3d();
        if (SurfMode) { // find proper pcurve
          for (Standard_Integer j = 1; j <= sc->NbAssociatedGeometry(); j++) {
            StepGeom_PcurveOrSurface PCorS = sc->AssociatedGeometryValue(j);
            Handle(StepGeom_Pcurve) pc = PCorS.Pcurve();
            if (pc.IsNull() || pc->BasisSurface() != S)
              continue;
            pcurve = pc;
            if (ccs->SameSense())
              break;
          }
        }
      }
    }
    else {
      if ( ! SurfMode || pcurve->BasisSurface() != S ) pcurve.Nullify();
      crv.Nullify();
    }
    
    // prepare edge
    TopoDS_Edge edge;
    
    // translate 3d curve, if present
    if ( ! crv.IsNull() ) {
      try {
        OCC_CATCH_SIGNALS
        Handle(Geom_Curve) c3d = StepToGeom::MakeCurve (crv);
        if (! c3d.IsNull()) {
          BRepBuilderAPI_MakeEdge MkEdge ( c3d, c3d->FirstParameter(), c3d->LastParameter() );
          if (MkEdge.IsDone())
          {
            if (Precision::IsNegativeInfinite (c3d->FirstParameter()) || Precision::IsPositiveInfinite (c3d->LastParameter()))
            {
              myInfiniteSegment = Standard_True;
              TP->AddWarning (CC, "Segment with infinite parameters");
            }
            edge = MkEdge.Edge();
          }
        }
      }
      catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	std::cout << "Warning: StepToTopoDS_TranslateCompositeCurve: Exception: ";
	anException.Print(std::cout); std::cout << std::endl;
#endif
	(void)anException;
      }
    }
    
    // translate pcurve, if available
    if ( ! pcurve.IsNull() ) {
      try {
        OCC_CATCH_SIGNALS
        StepToTopoDS_TranslateEdge TrE;
        Handle(Geom2d_Curve) c2d = TrE.MakePCurve(pcurve, Surf);
        if (!c2d.IsNull()) {
          if (edge.IsNull()) {
            BRepBuilderAPI_MakeEdge MkEdge(c2d, Surf, c2d->FirstParameter(), c2d->LastParameter());
            if (MkEdge.IsDone())
            {
              if (Precision::IsNegativeInfinite(c2d->FirstParameter()) || Precision::IsPositiveInfinite(c2d->LastParameter()))
              {
                myInfiniteSegment = Standard_True;
                TP->AddWarning(CC, "Segment with infinite parameters");
              }
              edge = MkEdge.Edge();
            }
          }
          else {
            BRep_Builder B;
            TopLoc_Location L;
            B.UpdateEdge(edge, c2d, Surf, L, 0.);
            B.Range(edge, Surf, L, c2d->FirstParameter(), c2d->LastParameter());
            B.SameRange(edge, Standard_False);
            B.SameParameter(edge, Standard_False);
          }
        }
      }
      catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	std::cout << "Warning: StepToTopoDS_TranslateCompositeCurve: Exception: ";
	anException.Print(std::cout); std::cout << std::endl;
#endif
	(void)anException;
      }
    }
    
    if ( edge.IsNull() ) {
      TP->AddFail ( crv, "Curve can not be translated");
      continue;
    }
    
    if ( ! ccs->SameSense() ) edge.Reverse();
    sbwd->Add ( edge );
  }
  if ( sbwd->NbEdges() <=0 ) {
    TP->AddFail ( CC, "Translation gives no result" );
    return Standard_False;
  }
  
  // connect wire; all other fixes are left for caller
  Standard_Real preci = Precision();
  Handle(ShapeFix_Wire) sfw = new ShapeFix_Wire;
  sfw->Load ( sbwd );
  sfw->SetPrecision ( preci );
  sfw->ClosedWireMode() = isClosed;
  sfw->FixReorder(); //:o3 abv 17 Feb 99: r0301_db.stp #57082
  if ( sfw->StatusReorder ( ShapeExtend_DONE ) ) {
    TP->AddWarning ( CC, "Segments were disordered; fixed" );
  }
  sfw->FixConnected ( preci );
  if ( sfw->StatusConnected ( ShapeExtend_FAIL ) ) {
    TP->AddWarning ( CC, "Segments are not connected" );
  }
  
  myWire = sbwd->Wire();
  done = ( sbwd->NbEdges() >0 );
  return Standard_True;    
}
	
//=======================================================================
//function : Value
//purpose  : return resulting wire
//=======================================================================

const TopoDS_Wire& StepToTopoDS_TranslateCompositeCurve::Value () const
{
  return myWire;
}

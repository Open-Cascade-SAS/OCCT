// Created on: 2000-06-07
// Created by: Galina KULIKOVA
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <BRep_Tool.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <ShapeExtend.hxx>
#include <ShapeUpgrade_FixSmallBezierCurves.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_Type.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <TopExp.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeUpgrade_FixSmallBezierCurves,ShapeUpgrade_FixSmallCurves)

ShapeUpgrade_FixSmallBezierCurves::ShapeUpgrade_FixSmallBezierCurves()
{
}
     
Standard_Boolean ShapeUpgrade_FixSmallBezierCurves::Approx(Handle(Geom_Curve)& Curve3d,Handle(Geom2d_Curve)& Curve2d,Handle(Geom2d_Curve)& Curve2dR,Standard_Real& First,Standard_Real& Last) 
{
 
  ShapeAnalysis_Edge sae;
  Handle(Geom_Curve) c3d;
  Standard_Real f,l;
  if(sae.Curve3d(myEdge,c3d,f,l,Standard_False)) {
    if(First < f)
      First = f;
    if(Last > l)
      Last =l;
    Handle(Geom_Curve) trc = new  Geom_TrimmedCurve(c3d,First,Last);
    GeomAbs_Shape aCont = (GeomAbs_Shape)trc->Continuity();
    if(aCont == GeomAbs_C3 || aCont == GeomAbs_CN)
      aCont = GeomAbs_C2;
    try {
      OCC_CATCH_SIGNALS
      GeomConvert_ApproxCurve AproxCurve(trc,Precision(),aCont,1,9);
      if(AproxCurve.IsDone()) {
	Handle(Geom_Curve) newCurve = AproxCurve.Curve();
	mySplitCurve3dTool->Init(AproxCurve.Curve(),First,Last);
	mySplitCurve3dTool->Perform(Standard_True);
	if (!mySplitCurve3dTool->Status ( ShapeExtend_FAIL )) {
	  Handle(TColGeom_HArray1OfCurve) theSegments3d;
	  theSegments3d = mySplitCurve3dTool->GetCurves();
	  if(theSegments3d->Length() >1) return Standard_False;
	  Curve3d = theSegments3d->Value(1);
	}
      }
    }
    catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "Warning: ShapeUpgrade_FixSmallBezierCurve::Approx(): Exception in Segment      :";
      anException.Print(cout); cout << endl;
#endif
      (void)anException;
      return Standard_False;
    }
  }
  if ( myFace.IsNull() ) return Standard_True;
  Handle(Geom2d_Curve) c2d;
  TopLoc_Location L;
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(myFace,L);
  GeomAdaptor_Surface ads(aSurf);// = new GeomAdaptor_Surface(aSurf);
  Standard_Real prec = Max(ads.UResolution(Precision()),ads.VResolution(Precision()));
  if(sae.PCurve(myEdge,myFace,c2d,f,l,Standard_False)) {
    if(First < f)
      First = f;
    if(Last > l)
      Last =l;
    Handle(Geom2d_Curve) trc2d = new  Geom2d_TrimmedCurve(c2d,First,Last);
    GeomAbs_Shape aCont = (GeomAbs_Shape)trc2d->Continuity();
    try {
      OCC_CATCH_SIGNALS
      Geom2dConvert_ApproxCurve AproxCurve2d(trc2d,prec,aCont,1,9);
      if(AproxCurve2d.IsDone()) {
	Handle(Geom2d_Curve) newCurve = AproxCurve2d.Curve();
	mySplitCurve2dTool->Init(AproxCurve2d.Curve(),First,Last);
	mySplitCurve2dTool->Perform(Standard_True);
	if ( mySplitCurve2dTool->Status ( ShapeExtend_FAIL ))
	  return Standard_False;
	Handle(TColGeom2d_HArray1OfCurve) theSegments2d;
	theSegments2d = mySplitCurve2dTool->GetCurves();
	if(theSegments2d->Length() >1) return Standard_False; //ShapeAnalysis_Surface
	Curve2d = theSegments2d->Value(1);
      }
    }
    catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
      cout << "Warning: ShapeUpgrade_FixSmallBezierCurve::Approx(): Exception in Segment      :";
      anException.Print(cout); cout << endl;
#endif
      (void)anException;
      return Standard_False;
    }
  }
  Standard_Boolean isSeam = BRep_Tool::IsClosed ( myEdge, myFace );
  if ( isSeam ) {
    Handle(Geom2d_Curve) c2;
    Standard_Real f2, l2;
    //smh#8
    TopoDS_Shape tmpE = myEdge.Reversed();
    TopoDS_Edge erev = TopoDS::Edge (tmpE );
    if ( sae.PCurve ( erev, myFace, c2, f2, l2, Standard_False) ) {
      if(First > f)
	First = f;
      if(Last > l)
	Last =l;
      Handle(Geom2d_Curve) trc2d = new  Geom2d_TrimmedCurve(c2,First,Last);
      GeomAbs_Shape aCont = trc2d->Continuity();
      Geom2dConvert_ApproxCurve AproxCurve2d(trc2d,prec,aCont,1,9);
      try {
        OCC_CATCH_SIGNALS
	if(AproxCurve2d.IsDone()) {
	  Handle(Geom2d_Curve) newCurve = AproxCurve2d.Curve();
	  mySplitCurve2dTool->Init(AproxCurve2d.Curve(),First,Last);
	  mySplitCurve2dTool->Perform(Standard_True);
	  if ( ! mySplitCurve2dTool->Status ( ShapeExtend_DONE ))
	    return Standard_False;
	  Handle(TColGeom2d_HArray1OfCurve) theSegments2d;
	  theSegments2d = mySplitCurve2dTool->GetCurves();
	  if(theSegments2d->Length() >1) return Standard_False; //ShapeAnalysis_Surface
	  Curve2dR = theSegments2d->Value(1);
	}
      }
      catch (Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
	  cout << "Warning: ShapeUpgrade_FixSmallBezierCurve::Approx(): Exception in Segment      :";
	  anException.Print(cout); cout << endl;
#endif
	  (void)anException;
	  return Standard_False;
	}
    }
  }
  return Standard_True;
}
 

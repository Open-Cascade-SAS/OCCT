// Created on: 2000-06-07
// Created by: Galina KULIKOVA
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <ShapeUpgrade_FixSmallBezierCurves.ixx>
#include <TopoDS_Edge.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <gp_Pnt.hxx>
#include <ShapeAnalysis_Edge.hxx>
#include <TopExp.hxx>
#include <BRep_Tool.hxx>
#include <gp_Pnt2d.hxx>
#include <Geom2dConvert_ApproxCurve.hxx>
#include <TColGeom_HArray1OfCurve.hxx>
#include <ShapeExtend.hxx>
#include <TColGeom2d_HArray1OfCurve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <TopLoc_Location.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <ShapeUpgrade_SplitCurve3d.hxx>
#include <ShapeUpgrade_SplitCurve2d.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
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
    Handle(Geom_TrimmedCurve) trc = new  Geom_TrimmedCurve(c3d,First,Last);
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
    catch (Standard_Failure) {
#ifdef DEB 
      cout << "Warning: ShapeUpgrade_FixSmallBezierCurve::Approx(): Exception in Segment      :";
      Standard_Failure::Caught()->Print(cout); cout << endl;
#endif
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
    Handle(Geom2d_TrimmedCurve) trc2d = new  Geom2d_TrimmedCurve(c2d,First,Last);
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
    catch (Standard_Failure) {
#ifdef DEB 
      cout << "Warning: ShapeUpgrade_FixSmallBezierCurve::Approx(): Exception in Segment      :";
      Standard_Failure::Caught()->Print(cout); cout << endl;
#endif
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
      Handle(Geom2d_TrimmedCurve) trc2d = new  Geom2d_TrimmedCurve(c2,First,Last);
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
      catch (Standard_Failure) {
#ifdef DEB 
	  cout << "Warning: ShapeUpgrade_FixSmallBezierCurve::Approx(): Exception in Segment      :";
	  Standard_Failure::Caught()->Print(cout); cout << endl;
#endif
	  return Standard_False;
	}
    }
  }
  return Standard_True;
}
 

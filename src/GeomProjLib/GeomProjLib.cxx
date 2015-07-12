// Created on: 1994-09-21
// Created by: Bruno DUMORTIER
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

//  Modified by skv - Wed Aug 11 17:26:03 2004 OCC6272

#include <Approx_CurveOnSurface.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomProjLib.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <ProjLib_CompProjectedCurve.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <ProjLib_ProjectOnPlane.hxx>
#include <ProjLib_ProjectOnSurface.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <stdio.h>
#ifdef DRAW
#include <DrawTrSurf.hxx>
static Standard_Boolean Affich = Standard_False;
static Standard_Integer NBPROJ = 1;
#endif


//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomProjLib::Curve2d(const Handle(Geom_Curve)& C,
					  const Standard_Real First,
					  const Standard_Real Last,
					  const Handle(Geom_Surface)& S,
					  const Standard_Real UDeb,
					  const Standard_Real UFin,
					  const Standard_Real VDeb,
					  const Standard_Real VFin,
					        Standard_Real& Tolerance)
{
#ifdef DRAW
  if ( Affich) {
    char name[256];
    Sprintf(name,"PROJCURV_%d",NBPROJ);
    DrawTrSurf::Set(name,C);
    Sprintf(name,"PROJSURF_%d",NBPROJ);
    DrawTrSurf::Set(name,S);
    NBPROJ++;
  }
#endif

  Tolerance = Max(Precision::PConfusion(),Tolerance);

  GeomAdaptor_Curve   AC(C,First,Last);
  GeomAdaptor_Surface AS(S,
			 UDeb,
			 UFin,
			 VDeb,
			 VFin);
  
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface(AS);
  Handle(GeomAdaptor_HCurve)   HC = new GeomAdaptor_HCurve(AC);

  ProjLib_ProjectedCurve Proj(HS,HC,Tolerance);

  Handle(Geom2d_Curve) G2dC;

  switch ( Proj.GetType()) {

  case GeomAbs_Line:
    G2dC = new Geom2d_Line(Proj.Line());
    break;

  case GeomAbs_Circle:
    G2dC = new Geom2d_Circle(Proj.Circle());
    break;

  case GeomAbs_Ellipse:
    G2dC = new Geom2d_Ellipse(Proj.Ellipse());
    break;

  case GeomAbs_Parabola:
    G2dC = new Geom2d_Parabola(Proj.Parabola());
    break;

  case GeomAbs_Hyperbola:
    G2dC = new Geom2d_Hyperbola(Proj.Hyperbola());
    break;

  case GeomAbs_BezierCurve:
    G2dC = Proj.Bezier();
    break;

  case GeomAbs_BSplineCurve:
    G2dC = Proj.BSpline();
    break;

  default:
    return G2dC;
    
  }

  if(G2dC.IsNull()) {
    Tolerance = Proj.GetTolerance();
    return G2dC;
  }

  if ( C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)) ) {
    Handle(Geom_TrimmedCurve) CTrim = Handle(Geom_TrimmedCurve)::DownCast(C);
    Standard_Real U1 = CTrim->FirstParameter();
    Standard_Real U2 = CTrim->LastParameter();
    if (!G2dC->IsPeriodic())
    {
      U1 = Max(U1, G2dC->FirstParameter());
      U2 = Min(U2, G2dC->LastParameter());
    }
    G2dC = new Geom2d_TrimmedCurve( G2dC, U1, U2);
  }

#ifdef DRAW
  if ( Affich) {
    static Standard_CString aprojcurv = "projcurv" ;
    DrawTrSurf::Set(aprojcurv,G2dC);
  }
#endif
  Tolerance = Proj.GetTolerance();
  return G2dC;
}

//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomProjLib::Curve2d(const Handle(Geom_Curve)& C,
					  const Standard_Real First,
					  const Standard_Real Last,
					  const Handle(Geom_Surface)& S,
					        Standard_Real& Tolerance)
{
 Standard_Real  UFirst,
                ULast,
                VFirst,
                VLast ;

 S->Bounds(UFirst,
	   ULast,
	   VFirst,
	   VLast) ;
 return Curve2d(C,
		First,
		Last,
		S,
		UFirst,
		ULast,
		VFirst,
		VLast,
		Tolerance) ;
}
//  Modified by skv - Wed Aug 11 17:26:03 2004 OCC6272 Begin
//  Add not implemented method.
//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomProjLib::Curve2d( const Handle(Geom_Curve)& C,
					   const Handle(Geom_Surface)& S,
					   const Standard_Real UDeb,
					   const Standard_Real UFin,
					   const Standard_Real VDeb,
					   const Standard_Real VFin)
{
  Standard_Real First = C->FirstParameter();
  Standard_Real Last  = C->LastParameter();
  Standard_Real Tol   = Precision::PConfusion();
  return GeomProjLib::Curve2d(C,First,Last,S,UDeb,UFin,VDeb,VFin,Tol);
}
//  Modified by skv - Wed Aug 11 17:26:03 2004 OCC6272 End

//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomProjLib::Curve2d( const Handle(Geom_Curve)& C,
					   const Handle(Geom_Surface)& S,
					   const Standard_Real UDeb,
					   const Standard_Real UFin,
					   const Standard_Real VDeb,
					   const Standard_Real VFin,
					   Standard_Real& Tolerance)
{
  Standard_Real First = C->FirstParameter();
  Standard_Real Last  = C->LastParameter();
  return GeomProjLib::Curve2d(C,First,Last,S,UDeb,UFin,VDeb,VFin,Tolerance);
}


//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomProjLib::Curve2d( const Handle(Geom_Curve)& C,
					   const Handle(Geom_Surface)& S) 
{
  Standard_Real First = C->FirstParameter();
  Standard_Real Last  = C->LastParameter();
  Standard_Real Tol   = Precision::PConfusion();
  return GeomProjLib::Curve2d(C,First,Last,S,Tol);
}


//=======================================================================
//function : Curve2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomProjLib::Curve2d( const Handle(Geom_Curve)&   C,
					   const Standard_Real         First,
					   const Standard_Real         Last,
					   const Handle(Geom_Surface)& S) 
{
  Standard_Real Tol = Precision::PConfusion();
  return GeomProjLib::Curve2d(C,First,Last,S,Tol);
}


//=======================================================================
//function : Project
//purpose  : 
//=======================================================================

Handle(Geom_Curve) GeomProjLib::Project( const Handle(Geom_Curve)& C,
					 const Handle(Geom_Surface)& S) 
{
  GeomAdaptor_Curve   AC(C);
  GeomAdaptor_Surface AS(S);

  Handle(Geom_Curve) GC;

  if ( AS.GetType() == GeomAbs_Plane) {
    ProjLib_ProjectOnPlane Proj( AS.Plane().Position());
    Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve(AC);
    Proj.Load(HC,Precision::PApproximation());

    switch ( Proj.GetType()) {
    case GeomAbs_Line:
      GC = new Geom_Line(Proj.Line());
      break;
      
    case GeomAbs_Circle:
      GC = new Geom_Circle(Proj.Circle());
      break;
      
    case GeomAbs_Ellipse:
      GC = new Geom_Ellipse(Proj.Ellipse());
      break;
      
    case GeomAbs_Parabola:
      GC = new Geom_Parabola(Proj.Parabola());
      break;
      
    case GeomAbs_Hyperbola:
      GC = new Geom_Hyperbola(Proj.Hyperbola());
      break;

    case GeomAbs_BezierCurve:
      GC = Proj.Bezier();
      break;

    case GeomAbs_BSplineCurve:
      GC = Proj.BSpline();
      break;

    default:
      return GC;
      
    }
    
    if ( C->IsKind(STANDARD_TYPE(Geom_TrimmedCurve))) {
      Handle(Geom_TrimmedCurve) CTrim = Handle(Geom_TrimmedCurve)::DownCast(C);
      Standard_Real U1 = CTrim->FirstParameter();
      Standard_Real U2 = CTrim->LastParameter();
      GC = new Geom_TrimmedCurve( GC, U1, U2);
    }
    
  }
  else {
    Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface(AS);
    Handle(GeomAdaptor_HCurve)   HC = new GeomAdaptor_HCurve(AC);
//    Standard_Real Tol  = Precision::Approximation();
//    Standard_Real TolU = Precision::PApproximation();
//    Standard_Real TolV = Precision::PApproximation();
    Standard_Real Tol  = 0.0001;
    Standard_Real TolU = Pow(Tol, 2./3);
    Standard_Real TolV = Pow(Tol, 2./3);
    ProjLib_CompProjectedCurve Proj(HS,HC,TolU,TolV,-1.);
    
    Standard_Real f,l;
    Proj.Bounds(1,f,l);
    Handle(Adaptor2d_HCurve2d) HC2d = Proj.Trim(f,l,TolU);
    Approx_CurveOnSurface Approx(HC2d, HS, f, l, Tol,
				 GeomAbs_C2,14,16,Standard_True);

    // ici, on a toujours un type BSpline.
    if (Approx.IsDone() && Approx.HasResult())
      GC = Approx.Curve3d();
  }

  return GC;
}

//=======================================================================
//function : ProjectOnPlane
//purpose  : 
//=======================================================================

Handle(Geom_Curve) GeomProjLib::ProjectOnPlane
(const Handle(Geom_Curve)& Curve,
 const Handle(Geom_Plane)& Plane,
 const gp_Dir& Dir,
 const Standard_Boolean KeepParametrization)
{
  GeomAdaptor_Curve   AC(Curve);
  Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve(AC);

  ProjLib_ProjectOnPlane Proj(Plane->Position(), Dir);
  Proj.Load(HC,Precision::Approximation(), KeepParametrization);

  Handle(Geom_Curve) GC;

  switch ( Proj.GetType()) {
  case GeomAbs_Line:
    GC = new Geom_Line(Proj.Line());
    break;
    
  case GeomAbs_Circle:
    GC = new Geom_Circle(Proj.Circle());
    break;
    
  case GeomAbs_Ellipse:
    GC = new Geom_Ellipse(Proj.Ellipse());
    break;
    
  case GeomAbs_Parabola:
    GC = new Geom_Parabola(Proj.Parabola());
    break;
    
  case GeomAbs_Hyperbola:
    GC = new Geom_Hyperbola(Proj.Hyperbola());
    break;
    
  case GeomAbs_BezierCurve: 
    GC = Proj.Bezier();
    break;

  case GeomAbs_BSplineCurve:
    GC = Proj.BSpline();
    break;
  default:
    return GC;
    
  }
  
  if ( Curve->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)) ) {
    Handle(Geom_TrimmedCurve) CTrim 
      = Handle(Geom_TrimmedCurve)::DownCast(Curve);
    GC = new Geom_TrimmedCurve( GC, Proj.FirstParameter(),
			            Proj.LastParameter());
  }
  
  return GC;
  
}



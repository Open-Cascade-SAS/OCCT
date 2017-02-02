// Created on: 1995-05-22
// Created by: Laurent BOURESCHE
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

#include <ChFiKPart_ComputeData_Fcts.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <ProjLib_ProjectedCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <TopOpeBRepDS_Curve.hxx>
#include <TopOpeBRepDS_Surface.hxx>


#include <Standard_NotImplemented.hxx>

#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>




//=======================================================================
//function : InPeriod
//purpose  : 
//=======================================================================

Standard_Real  ChFiKPart_InPeriod(const Standard_Real U, 
				  const Standard_Real UFirst, 
				  const Standard_Real ULast,
				  const Standard_Real Eps)
{
  Standard_Real u = U, period = ULast - UFirst;
  while (Eps < (UFirst-u)) u += period;
  while (Eps > (ULast -u)) u -= period;
  if ( u < UFirst) u = UFirst;
  return u;
}


//=======================================================================
//function : PCurve 
//purpose  : Calculate a straight line in form of BSpline to guarantee 
//           the parameters.
//=======================================================================

Handle(Geom2d_BSplineCurve) ChFiKPart_PCurve(const gp_Pnt2d& UV1,
		                            const gp_Pnt2d& UV2,
			                    const Standard_Real Pardeb,
			                    const Standard_Real Parfin)
{
  TColgp_Array1OfPnt2d p(1,2);
  TColStd_Array1OfReal k(1,2);
  TColStd_Array1OfInteger m(1,2);
  m.Init(2);
  k(1) = Pardeb;
  k(2) = Parfin;
  p(1) = UV1;
  p(2) = UV2;
  Handle(Geom2d_BSplineCurve) Pcurv = new Geom2d_BSplineCurve(p,k,m,1);
  return Pcurv;
}


//=======================================================================
//function : ProjPC
//purpose  : For spherical corners the contours which of are not 
//           isos the circle is projected.
//=======================================================================

void ChFiKPart_ProjPC(const GeomAdaptor_Curve& Cg, 
		     const GeomAdaptor_Surface& Sg, 
		     Handle(Geom2d_Curve)& Pcurv) 
{
  if (Sg.GetType() < GeomAbs_BezierSurface) {
    Handle(GeomAdaptor_HCurve)   HCg = new GeomAdaptor_HCurve(Cg);
    Handle(GeomAdaptor_HSurface) HSg = new GeomAdaptor_HSurface(Sg);
    ProjLib_ProjectedCurve Projc (HSg,HCg);
    switch (Projc.GetType()) {
    case GeomAbs_Line : 
      {
	Pcurv = new Geom2d_Line(Projc.Line());
      }
      break;
    case GeomAbs_BezierCurve :
      {
	Handle(Geom2d_BezierCurve) BezProjc = Projc.Bezier(); 
	TColgp_Array1OfPnt2d TP(1,BezProjc->NbPoles());
	if (BezProjc->IsRational()) {
	  TColStd_Array1OfReal TW(1,BezProjc->NbPoles());
	  BezProjc->Poles(TP);
	  BezProjc->Weights(TW);
	  Pcurv = new Geom2d_BezierCurve(TP,TW);
	}
	else {
	  BezProjc->Poles(TP);
	  Pcurv = new Geom2d_BezierCurve(TP);
	}
      }
      break;
#if 0 
	TColgp_Array1OfPnt2d TP(1,Projc.NbPoles());
	if (Projc.IsRational()) {
	  TColStd_Array1OfReal TW(1,Projc.NbPoles());
	  Projc.PolesAndWeights(TP,TW);
	  Pcurv = new Geom2d_BezierCurve(TP,TW);
	}
	else {
	  Projc.Poles(TP);
	  Pcurv = new Geom2d_BezierCurve(TP);
	}
      }
      break;
#endif
    case GeomAbs_BSplineCurve :
      {
	Handle(Geom2d_BSplineCurve) BspProjc = Projc.BSpline();
	TColgp_Array1OfPnt2d TP(1,BspProjc->NbPoles());
	TColStd_Array1OfReal TK(1,BspProjc->NbKnots());
	TColStd_Array1OfInteger TM(1,BspProjc->NbKnots());
	
	BspProjc->Knots(TK);
	BspProjc->Multiplicities(TM);
	if (BspProjc->IsRational()) {
	  TColStd_Array1OfReal TW(1,BspProjc->NbPoles());
	  BspProjc->Poles(TP);
	  BspProjc->Weights(TW);
	  Pcurv = new Geom2d_BSplineCurve(TP,TW,TK,TM,BspProjc->Degree());
	}
	else {
	  BspProjc->Poles(TP);
	  Pcurv = new Geom2d_BSplineCurve(TP,TK,TM,BspProjc->Degree());
	}
      }
    break;
#if 0 
	TColgp_Array1OfPnt2d TP(1,Projc.NbPoles());
	TColStd_Array1OfReal TK(1,Projc.NbKnots());
	TColStd_Array1OfInteger TM(1,Projc.NbKnots());
	Projc.KnotsAndMultiplicities(TK,TM);
	if (Projc.IsRational()) {
	  TColStd_Array1OfReal TW(1,Projc.NbPoles());
	  Projc.PolesAndWeights(TP,TW);
	  Pcurv = new Geom2d_BSplineCurve(TP,TW,TK,TM,Projc.Degree());
	}
	else {
	  Projc.Poles(TP);
	  Pcurv = new Geom2d_BSplineCurve(TP,TK,TM,Projc.Degree());
	}
      }
      break;
#endif
      default :
      throw Standard_NotImplemented("failed approximation of the pcurve ");
    }
  }
  else {
    throw Standard_NotImplemented("approximate pcurve on the left surface");
  }
}

//=======================================================================
//function : IndexCurveInDS
//purpose  : Place a Curve in the DS and return its index.
//=======================================================================

Standard_Integer ChFiKPart_IndexCurveInDS(const Handle(Geom_Curve)& C,
			                 TopOpeBRepDS_DataStructure& DStr) 
{
  return DStr.AddCurve(TopOpeBRepDS_Curve(C,0.));
}


//=======================================================================
//function : IndexSurfaceInDS
//purpose  : Place a Surface in the DS and return its index.
//=======================================================================

Standard_Integer ChFiKPart_IndexSurfaceInDS(const Handle(Geom_Surface)& S,
					   TopOpeBRepDS_DataStructure& DStr) 
{
  return DStr.AddSurface(TopOpeBRepDS_Surface(S,0.));
}


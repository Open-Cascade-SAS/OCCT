// Created on: 1995-05-22
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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
      Standard_NotImplemented::Raise("failed approximation of the pcurve ");
    }
  }
  else {
    Standard_NotImplemented::Raise("approximate pcurve on the left surface");
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


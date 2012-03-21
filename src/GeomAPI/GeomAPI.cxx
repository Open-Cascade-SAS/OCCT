// Created on: 1994-08-05
// Created by: Remi LEQUETTE
// Copyright (c) 1994-1999 Matra Datavision
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



#include <GeomAPI.ixx>

#include <ProjLib_ProjectedCurve.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor.hxx>

#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColgp_Array1OfPnt.hxx>

//=======================================================================
//function : To2d
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) GeomAPI::To2d(const Handle(Geom_Curve)& C,
				   const gp_Pln& P)
{
  Handle(Geom2d_Curve) result;
  Handle(GeomAdaptor_HCurve) HC = new GeomAdaptor_HCurve(C);
  Handle(Geom_Plane) Plane = new Geom_Plane(P);
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface(Plane);

  ProjLib_ProjectedCurve Proj(HS,HC);

  if (Proj.GetType() != GeomAbs_OtherCurve) {
    result = Geom2dAdaptor::MakeCurve(Proj);
  }
  
  return result;
}



//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

Handle(Geom_Curve) GeomAPI::To3d(const Handle(Geom2d_Curve)& C,
				 const gp_Pln& P)
{
  Handle(Geom2dAdaptor_HCurve) AHC  = new Geom2dAdaptor_HCurve(C);

  Handle(Geom_Plane) ThePlane = new Geom_Plane(P);
  Handle(GeomAdaptor_HSurface) AHS = new GeomAdaptor_HSurface(ThePlane);

  Adaptor3d_CurveOnSurface COS(AHC,AHS);
  return GeomAdaptor::MakeCurve(COS);
}

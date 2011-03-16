// File:	GeomAPI.cxx
// Created:	Fri Aug  5 11:38:25 1994
// Author:	Remi LEQUETTE
//		<rle@bravox>


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

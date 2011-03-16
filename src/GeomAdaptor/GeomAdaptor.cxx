// File:      GeomAdaptor.cxx
// Created:   Wed May  3 17:48:12 1995
// Author:    Bruno DUMORTIER
// Copyright: OPEN CASCADE 1995

#include <GeomAdaptor.ixx>

#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>

#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>

#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>

//=======================================================================
//function : MakeCurve
//purpose  : 
//=======================================================================

Handle(Geom_Curve) GeomAdaptor::MakeCurve (const Adaptor3d_Curve& HC)
{
  Handle(Geom_Curve) C;
  
  switch (HC.GetType())
  {
  case GeomAbs_Line:
    C = new Geom_Line(HC.Line());
    break;

  case GeomAbs_Circle:
    C = new Geom_Circle(HC.Circle());
    break;

  case GeomAbs_Ellipse:
    C = new Geom_Ellipse(HC.Ellipse());
    break;

  case GeomAbs_Parabola:
    C = new Geom_Parabola(HC.Parabola());
    break;

  case GeomAbs_Hyperbola:
    C = new Geom_Hyperbola(HC.Hyperbola());
    break;

  case GeomAbs_BezierCurve:
    C = Handle(Geom_BezierCurve)::DownCast(HC.Bezier()->Copy());
    break;

  case GeomAbs_BSplineCurve:
    C = Handle(Geom_BSplineCurve)::DownCast(HC.BSpline()->Copy());
    break;

  case GeomAbs_OtherCurve:
    Standard_DomainError::Raise("GeomAdaptor::MakeCurve : OtherCurve");

  }

  // trim the curve if necassary.
  if (! C.IsNull() &&
      (HC.FirstParameter() != C->FirstParameter()) ||
      (HC.LastParameter()  != C->LastParameter())) {

    C = new Geom_TrimmedCurve(C,HC.FirstParameter(),HC.LastParameter());
  }

  return C;
}


//=======================================================================
//function : MakeSurface
//purpose  : 
//=======================================================================

Handle(Geom_Surface) GeomAdaptor::MakeSurface(const Adaptor3d_Surface& HS)
{
  Handle(Geom_Surface) S;

  switch ( HS.GetType())
  {
  case GeomAbs_Plane:
    S = new Geom_Plane(HS.Plane());
    break;

  case GeomAbs_Cylinder:
    S = new Geom_CylindricalSurface(HS.Cylinder());
    break;

  case GeomAbs_Cone:
    S = new Geom_ConicalSurface(HS.Cone());
    break;

  case GeomAbs_Sphere:
    S = new Geom_SphericalSurface(HS.Sphere());
    break;

  case GeomAbs_Torus:
    S = new Geom_ToroidalSurface(HS.Torus());
    break;

  case GeomAbs_BezierSurface:
    S = Handle(Geom_BezierSurface)::DownCast(HS.Bezier()->Copy());
    break;

  case GeomAbs_BSplineSurface:
    S = Handle(Geom_BSplineSurface)::DownCast(HS.BSpline()->Copy());
    break;

  case GeomAbs_SurfaceOfRevolution:
    S = new Geom_SurfaceOfRevolution
      (GeomAdaptor::MakeCurve(HS.BasisCurve()->Curve()),HS.AxeOfRevolution());
    break;

  case GeomAbs_SurfaceOfExtrusion:
    S = new Geom_SurfaceOfLinearExtrusion
      (GeomAdaptor::MakeCurve(HS.BasisCurve()->Curve()),HS.Direction());
    break;

  case GeomAbs_OffsetSurface:
    S = new Geom_OffsetSurface(GeomAdaptor::MakeSurface(HS.BasisSurface()->Surface()),
			       HS.OffsetValue());
    break;

  case GeomAbs_OtherSurface:
    Standard_DomainError::Raise("GeomAdaptor::MakeSurface : OtherSurface");
    break;
  }

  if ( S.IsNull() )
    return S;

  // trim the surface if necassary.
  Standard_Real U1, U2, V1, V2;
  S->Bounds(U1, U2, V1, V2);
  if ((HS.FirstUParameter() != U1 ) ||
      (HS.LastUParameter () != U2 ) ||
      (HS.FirstVParameter() != V1 ) ||
      (HS.LastVParameter () != V2 )   ) {

    S = new Geom_RectangularTrimmedSurface
      (S,HS.FirstUParameter(),HS.LastUParameter(),
         HS.FirstVParameter(),HS.LastVParameter());
  }

  return S;
}

// Created on: 1993-02-19
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#include <BRepAdaptor_Surface.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <TopoDS_Face.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRepAdaptor_Surface, Adaptor3d_Surface)

//=================================================================================================

BRepAdaptor_Surface::BRepAdaptor_Surface() {}

//=================================================================================================

BRepAdaptor_Surface::BRepAdaptor_Surface(const TopoDS_Face& F, const Standard_Boolean R)
{
  Initialize(F, R);
}

//=================================================================================================

Handle(Adaptor3d_Surface) BRepAdaptor_Surface::ShallowCopy() const
{
  Handle(BRepAdaptor_Surface) aCopy = new BRepAdaptor_Surface();

  const Handle(Adaptor3d_Surface) aSurface     = mySurf.ShallowCopy();
  const GeomAdaptor_Surface&      aGeomSurface = *(Handle(GeomAdaptor_Surface)::DownCast(aSurface));
  aCopy->mySurf                                = aGeomSurface;

  aCopy->myTrsf = myTrsf;
  aCopy->myFace = myFace;

  return aCopy;
}

//=================================================================================================

void BRepAdaptor_Surface::Initialize(const TopoDS_Face& F, const Standard_Boolean Restriction)
{
  myFace = F;
  TopLoc_Location             L;
  const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(F, L);
  if (aSurface.IsNull())
    return;

  if (Restriction)
  {
    Standard_Real umin, umax, vmin, vmax;
    BRepTools::UVBounds(F, umin, umax, vmin, vmax);
    mySurf.Load(aSurface, umin, umax, vmin, vmax);
  }
  else
    mySurf.Load(aSurface);
  myTrsf = L.Transformation();
}

//=================================================================================================

const GeomAdaptor_Surface& BRepAdaptor_Surface::Surface() const
{
  return mySurf;
}

//=================================================================================================

GeomAdaptor_Surface& BRepAdaptor_Surface::ChangeSurface()
{
  return mySurf;
}

//=================================================================================================

const gp_Trsf& BRepAdaptor_Surface::Trsf() const
{
  return myTrsf;
}

//=================================================================================================

const TopoDS_Face& BRepAdaptor_Surface::Face() const
{
  return myFace;
}

//=================================================================================================

Standard_Real BRepAdaptor_Surface::Tolerance() const
{
  return BRep_Tool::Tolerance(myFace);
}

//=================================================================================================

void BRepAdaptor_Surface::UIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  mySurf.UIntervals(T, S);
}

//=================================================================================================

void BRepAdaptor_Surface::VIntervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const
{
  mySurf.VIntervals(T, S);
}

//=================================================================================================

Handle(Adaptor3d_Surface) BRepAdaptor_Surface::UTrim(const Standard_Real First,
                                                     const Standard_Real Last,
                                                     const Standard_Real Tol) const
{
  Handle(GeomAdaptor_Surface) HS = new GeomAdaptor_Surface();
  HS->Load(Handle(Geom_Surface)::DownCast(mySurf.Surface()->Transformed(myTrsf)));
  return HS->UTrim(First, Last, Tol);
}

//=================================================================================================

Handle(Adaptor3d_Surface) BRepAdaptor_Surface::VTrim(const Standard_Real First,
                                                     const Standard_Real Last,
                                                     const Standard_Real Tol) const
{
  Handle(GeomAdaptor_Surface) HS = new GeomAdaptor_Surface();
  HS->Load(Handle(Geom_Surface)::DownCast(mySurf.Surface()->Transformed(myTrsf)));
  return HS->VTrim(First, Last, Tol);
}

//=================================================================================================

gp_Pnt BRepAdaptor_Surface::Value(const Standard_Real U, const Standard_Real V) const
{
  return mySurf.Value(U, V).Transformed(myTrsf);
}

//=================================================================================================

void BRepAdaptor_Surface::D0(const Standard_Real U, const Standard_Real V, gp_Pnt& P) const
{
  mySurf.D0(U, V, P);
  P.Transform(myTrsf);
}

//=================================================================================================

void BRepAdaptor_Surface::D1(const Standard_Real U,
                             const Standard_Real V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V) const
{
  mySurf.D1(U, V, P, D1U, D1V);
  P.Transform(myTrsf);
  D1U.Transform(myTrsf);
  D1V.Transform(myTrsf);
}

//=================================================================================================

void BRepAdaptor_Surface::D2(const Standard_Real U,
                             const Standard_Real V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V,
                             gp_Vec&             D2U,
                             gp_Vec&             D2V,
                             gp_Vec&             D2UV) const
{
  mySurf.D2(U, V, P, D1U, D1V, D2U, D2V, D2UV);
  P.Transform(myTrsf);
  D1U.Transform(myTrsf);
  D1V.Transform(myTrsf);
  D2U.Transform(myTrsf);
  D2V.Transform(myTrsf);
  D2UV.Transform(myTrsf);
}

//=================================================================================================

void BRepAdaptor_Surface::D3(const Standard_Real U,
                             const Standard_Real V,
                             gp_Pnt&             P,
                             gp_Vec&             D1U,
                             gp_Vec&             D1V,
                             gp_Vec&             D2U,
                             gp_Vec&             D2V,
                             gp_Vec&             D2UV,
                             gp_Vec&             D3U,
                             gp_Vec&             D3V,
                             gp_Vec&             D3UUV,
                             gp_Vec&             D3UVV) const
{
  mySurf.D3(U, V, P, D1U, D1V, D2U, D2V, D2UV, D3U, D3V, D3UUV, D3UVV);
  P.Transform(myTrsf);
  D1U.Transform(myTrsf);
  D1V.Transform(myTrsf);
  D2U.Transform(myTrsf);
  D2V.Transform(myTrsf);
  D2UV.Transform(myTrsf);
  D3U.Transform(myTrsf);
  D3V.Transform(myTrsf);
  D3UUV.Transform(myTrsf);
  D3UVV.Transform(myTrsf);
}

//=================================================================================================

gp_Vec BRepAdaptor_Surface::DN(const Standard_Real    U,
                               const Standard_Real    V,
                               const Standard_Integer Nu,
                               const Standard_Integer Nv) const
{
  return mySurf.DN(U, V, Nu, Nv).Transformed(myTrsf);
}

//=================================================================================================

gp_Pln BRepAdaptor_Surface::Plane() const
{
  return mySurf.Plane().Transformed(myTrsf);
}

//=================================================================================================

gp_Cylinder BRepAdaptor_Surface::Cylinder() const
{
  return mySurf.Cylinder().Transformed(myTrsf);
}

//=================================================================================================

gp_Sphere BRepAdaptor_Surface::Sphere() const
{
  return mySurf.Sphere().Transformed(myTrsf);
}

//=================================================================================================

gp_Cone BRepAdaptor_Surface::Cone() const
{
  return mySurf.Cone().Transformed(myTrsf);
}

//=================================================================================================

gp_Torus BRepAdaptor_Surface::Torus() const
{
  return mySurf.Torus().Transformed(myTrsf);
}

//=================================================================================================

Handle(Geom_BezierSurface) BRepAdaptor_Surface::Bezier() const
{
  return Handle(Geom_BezierSurface)::DownCast(mySurf.Bezier()->Transformed(myTrsf));
}

//=================================================================================================

Handle(Geom_BSplineSurface) BRepAdaptor_Surface::BSpline() const
{
  return Handle(Geom_BSplineSurface)::DownCast(mySurf.BSpline()->Transformed(myTrsf));
}

//=================================================================================================

gp_Ax1 BRepAdaptor_Surface::AxeOfRevolution() const
{
  return mySurf.AxeOfRevolution().Transformed(myTrsf);
}

//=================================================================================================

gp_Dir BRepAdaptor_Surface::Direction() const
{
  return mySurf.Direction().Transformed(myTrsf);
}

//=================================================================================================

Handle(Adaptor3d_Curve) BRepAdaptor_Surface::BasisCurve() const
{
  Handle(GeomAdaptor_Surface) HS = new GeomAdaptor_Surface();
  HS->Load(Handle(Geom_Surface)::DownCast(mySurf.Surface()->Transformed(myTrsf)));

  return HS->BasisCurve();
}

//=================================================================================================

Handle(Adaptor3d_Surface) BRepAdaptor_Surface::BasisSurface() const
{
  Handle(GeomAdaptor_Surface) HS = new GeomAdaptor_Surface();
  HS->Load(Handle(Geom_Surface)::DownCast(mySurf.Surface()->Transformed(myTrsf)));
  return HS->BasisSurface();
}

//=================================================================================================

Standard_Real BRepAdaptor_Surface::OffsetValue() const
{
  return mySurf.OffsetValue();
}

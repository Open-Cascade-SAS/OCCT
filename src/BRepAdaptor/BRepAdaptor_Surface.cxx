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


#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepTools.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomAdaptor_HCurve.hxx>
#include <GeomAdaptor_HSurface.hxx>
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
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <TopoDS_Face.hxx>

//=======================================================================
//function : BRepAdaptor_Surface
//purpose  : 
//=======================================================================
BRepAdaptor_Surface::BRepAdaptor_Surface() 
{
}


//=======================================================================
//function : BRepAdaptor_Surface
//purpose  : 
//=======================================================================

BRepAdaptor_Surface::BRepAdaptor_Surface(const TopoDS_Face& F,
					 const Standard_Boolean R) 
{
  Initialize(F,R);
}


//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void BRepAdaptor_Surface::Initialize(const TopoDS_Face& F,
				     const Standard_Boolean Restriction) 
{
  myFace = F;
  TopLoc_Location L;
  const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(F, L);
  if (aSurface.IsNull())
    return;

  if (Restriction) {
    Standard_Real umin,umax,vmin,vmax;
    BRepTools::UVBounds(F,umin,umax,vmin,vmax);
    mySurf.Load(aSurface,umin,umax,vmin,vmax);
  }
  else 
    mySurf.Load(aSurface);
  myTrsf = L.Transformation();
}


//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const GeomAdaptor_Surface& BRepAdaptor_Surface::Surface() const 
{
  return mySurf;
}


//=======================================================================
//function : ChangeSurface
//purpose  : 
//=======================================================================

GeomAdaptor_Surface& BRepAdaptor_Surface::ChangeSurface()
{
  return mySurf;
}


//=======================================================================
//function : Trsf
//purpose  : 
//=======================================================================

const gp_Trsf& BRepAdaptor_Surface::Trsf() const 
{
  return myTrsf;
}


//=======================================================================
//function : Face
//purpose  : 
//=======================================================================

const TopoDS_Face& BRepAdaptor_Surface::Face() const
{
  return myFace;
}

//=======================================================================
//function : Tolerance
//purpose  : 
//=======================================================================

Standard_Real BRepAdaptor_Surface::Tolerance() const
{
  return BRep_Tool::Tolerance(myFace);
}


//=======================================================================
//function : UIntervals
//purpose  : 
//=======================================================================

void BRepAdaptor_Surface::UIntervals(TColStd_Array1OfReal& T,
				     const GeomAbs_Shape S) const 
{
  mySurf.UIntervals(T,S);
}


//=======================================================================
//function : VIntervals
//purpose  : 
//=======================================================================

void BRepAdaptor_Surface::VIntervals(TColStd_Array1OfReal& T,
				     const GeomAbs_Shape S) const 
{
  mySurf.VIntervals(T,S);
}


//=======================================================================
//function : UTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) BRepAdaptor_Surface::UTrim
(const Standard_Real First,
 const Standard_Real Last ,
 const Standard_Real Tol   ) const 
{
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
  HS->ChangeSurface().Load
    (Handle(Geom_Surface)::DownCast(mySurf.Surface()->Transformed(myTrsf)));
  return HS->UTrim(First,Last,Tol);
}


//=======================================================================
//function : VTrim
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) BRepAdaptor_Surface::VTrim
(const Standard_Real First,
 const Standard_Real Last, 
 const Standard_Real Tol) const 
{
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
  HS->ChangeSurface().Load
    (Handle(Geom_Surface)::DownCast(mySurf.Surface()->Transformed(myTrsf)));
  return HS->VTrim(First,Last,Tol);
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt BRepAdaptor_Surface::Value(const Standard_Real U,
				  const Standard_Real V) const
{
  return mySurf.Value(U,V).Transformed(myTrsf);
}

//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void  BRepAdaptor_Surface::D0(const Standard_Real U, 
			      const Standard_Real V, 
			      gp_Pnt& P) const
{
  mySurf.D0(U,V,P);
  P.Transform(myTrsf);
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void  BRepAdaptor_Surface::D1(const Standard_Real U, 
			      const Standard_Real V, 
			      gp_Pnt& P, 
			      gp_Vec& D1U,
			      gp_Vec& D1V)const 
{
  mySurf.D1(U,V,P,D1U,D1V);
  P.Transform(myTrsf);
  D1U.Transform(myTrsf);
  D1V.Transform(myTrsf);
}


//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void  BRepAdaptor_Surface::D2(const Standard_Real U, 
			      const Standard_Real V,
			      gp_Pnt& P, gp_Vec& D1U, 
			      gp_Vec& D1V,
			      gp_Vec& D2U, 
			      gp_Vec& D2V, 
			      gp_Vec& D2UV)const 
{
  mySurf.D2(U,V,P,D1U,D1V,D2U,D2V,D2UV);
  P.Transform(myTrsf);
  D1U.Transform(myTrsf);
  D1V.Transform(myTrsf);
  D2U.Transform(myTrsf);
  D2V.Transform(myTrsf);
  D2UV.Transform(myTrsf);
}

//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void  BRepAdaptor_Surface::D3(const Standard_Real U, 
			      const Standard_Real V,
			      gp_Pnt& P,
			      gp_Vec& D1U, gp_Vec& D1V,
			      gp_Vec& D2U, gp_Vec& D2V, 
			      gp_Vec& D2UV,
			      gp_Vec& D3U, gp_Vec& D3V,
			      gp_Vec& D3UUV, gp_Vec& D3UVV)const 
{
  mySurf.D3(U,V,P,D1U,D1V,D2U,D2V,D2UV,D3U,D3V,D3UUV,D3UVV);
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


//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

gp_Vec BRepAdaptor_Surface::DN(const Standard_Real U,
			       const Standard_Real V,
			       const Standard_Integer Nu,
			       const Standard_Integer Nv) const
{
  return mySurf.DN(U,V,Nu,Nv).Transformed(myTrsf);
}

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

gp_Pln  BRepAdaptor_Surface::Plane()const 
{
  return mySurf.Plane().Transformed(myTrsf);
}


//=======================================================================
//function : Cylinder
//purpose  : 
//=======================================================================

gp_Cylinder  BRepAdaptor_Surface::Cylinder()const 
{
  return mySurf.Cylinder().Transformed(myTrsf);
}


//=======================================================================
//function : Sphere
//purpose  : 
//=======================================================================

gp_Sphere  BRepAdaptor_Surface::Sphere()const 
{
  return mySurf.Sphere().Transformed(myTrsf);
}


//=======================================================================
//function : Cone
//purpose  : 
//=======================================================================

gp_Cone  BRepAdaptor_Surface::Cone()const 
{
  return mySurf.Cone().Transformed(myTrsf);
}

//=======================================================================
//function : Torus
//purpose  : 
//=======================================================================

gp_Torus  BRepAdaptor_Surface::Torus()const 
{
  return mySurf.Torus().Transformed(myTrsf);
}

//=======================================================================
//function : Bezier
//purpose  : 
//=======================================================================

Handle(Geom_BezierSurface) BRepAdaptor_Surface::Bezier() const 
{
  return Handle(Geom_BezierSurface)::DownCast
    (mySurf.Bezier()->Transformed(myTrsf));
}


//=======================================================================
//function : BSpline
//purpose  : 
//=======================================================================

Handle(Geom_BSplineSurface) BRepAdaptor_Surface::BSpline() const 
{
  return Handle(Geom_BSplineSurface)::DownCast
    (mySurf.BSpline()->Transformed(myTrsf));
}


//=======================================================================
//function : AxeOfRevolution
//purpose  : 
//=======================================================================

gp_Ax1 BRepAdaptor_Surface::AxeOfRevolution() const 
{
  return mySurf.AxeOfRevolution().Transformed(myTrsf);
}


//=======================================================================
//function : Direction
//purpose  : 
//=======================================================================

gp_Dir BRepAdaptor_Surface::Direction() const 
{
  return mySurf.Direction().Transformed(myTrsf);
}


//=======================================================================
//function : BasisCurve
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HCurve) BRepAdaptor_Surface::BasisCurve() const 
{
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
  HS->ChangeSurface().Load
    (Handle(Geom_Surface)::DownCast(mySurf.Surface()->Transformed(myTrsf)));

  return HS->BasisCurve();
}


//=======================================================================
//function : BasisSurface
//purpose  : 
//=======================================================================

Handle(Adaptor3d_HSurface) BRepAdaptor_Surface::BasisSurface() const 
{  
  Handle(GeomAdaptor_HSurface) HS = new GeomAdaptor_HSurface();
  HS->ChangeSurface().Load
    (Handle(Geom_Surface)::DownCast(mySurf.Surface()->Transformed(myTrsf)));
  return HS->BasisSurface();
}


//=======================================================================
//function : OffsetValue
//purpose  : 
//=======================================================================

Standard_Real BRepAdaptor_Surface::OffsetValue() const 
{
  return mySurf.OffsetValue();
}


// Created on: 1993-07-06
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


#include <BRep_CurveRepresentation.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_CurveRepresentation,Standard_Transient)

//=======================================================================
//function : BRep_CurveRepresentation
//purpose  : 
//=======================================================================
BRep_CurveRepresentation::BRep_CurveRepresentation(const TopLoc_Location& L):
       myLocation(L)
{
}


//=======================================================================
//function : IsCurve3D
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsCurve3D()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsCurveOnSurface()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsCurveOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsCurveOnClosedSurface()const 
{
  return Standard_False;
}


//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsRegularity()const 
{
  return Standard_False;
}

//=======================================================================
//function : IsCurveOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsCurveOnSurface
  (const Handle(Geom_Surface)& , const TopLoc_Location& )const 
{
  return Standard_False;
}


//=======================================================================
//function : IsRegularity
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsRegularity
  (const Handle(Geom_Surface)& ,
   const Handle(Geom_Surface)& ,
   const TopLoc_Location& , 
   const TopLoc_Location& )const 
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygon3D
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsPolygon3D() const
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsPolygonOnTriangulation() const
{
  return Standard_False;
}


//=======================================================================
//function : IsPolygonOnTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsPolygonOnTriangulation
  (const Handle(Poly_Triangulation)&, const TopLoc_Location&) const
{
  return Standard_False;
}


//=======================================================================
//function : IsPolygonOnClosedTriangulation
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsPolygonOnClosedTriangulation() const
{
  return Standard_False;
}


//=======================================================================
//function : IsPolygonOnClosedSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsPolygonOnClosedSurface() const
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsPolygonOnSurface() const
{
  return Standard_False;
}

//=======================================================================
//function : IsPolygonOnSurface
//purpose  : 
//=======================================================================

Standard_Boolean  BRep_CurveRepresentation::IsPolygonOnSurface
  (const Handle(Geom_Surface)&, 
   const TopLoc_Location&) const
{
  return Standard_False;
}


//=======================================================================
//function : Curve3D
//purpose  : 
//=======================================================================

const Handle(Geom_Curve)&  BRep_CurveRepresentation::Curve3D()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=======================================================================
//function : Curve3D
//purpose  : 
//=======================================================================

void BRep_CurveRepresentation::Curve3D(const Handle(Geom_Curve)&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}





//=======================================================================
//function : Surface
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_CurveRepresentation::Surface()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  BRep_CurveRepresentation::PCurve()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=======================================================================
//function : PCurve2
//purpose  : 
//=======================================================================

const Handle(Geom2d_Curve)&  BRep_CurveRepresentation::PCurve2()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=======================================================================
//function : PCurve
//purpose  : 
//=======================================================================

void  BRep_CurveRepresentation::PCurve(const Handle(Geom2d_Curve)&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=======================================================================
//function : PCurve2
//purpose  : 
//=======================================================================

void  BRep_CurveRepresentation::PCurve2(const Handle(Geom2d_Curve)&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

const GeomAbs_Shape&  BRep_CurveRepresentation::Continuity()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

void BRep_CurveRepresentation::Continuity(const GeomAbs_Shape )
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : Surface2
//purpose  : 
//=======================================================================

const Handle(Geom_Surface)&  BRep_CurveRepresentation::Surface2()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : Location2
//purpose  : 
//=======================================================================

const TopLoc_Location&  BRep_CurveRepresentation::Location2()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}



//=======================================================================
//function : Polygon3D
//purpose  : 
//=======================================================================

const Handle(Poly_Polygon3D)&  BRep_CurveRepresentation::Polygon3D()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=======================================================================
//function : Polygon3D
//purpose  : 
//=======================================================================

void BRep_CurveRepresentation::Polygon3D(const Handle(Poly_Polygon3D)&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : Polygon
//purpose  : 
//=======================================================================

const Handle(Poly_Polygon2D)&  BRep_CurveRepresentation::Polygon()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : Polygon
//purpose  : 
//=======================================================================

void BRep_CurveRepresentation::Polygon(const Handle(Poly_Polygon2D)&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=======================================================================
//function : PolygonOnTriangulation2
//purpose  : 
//=======================================================================

const Handle(Poly_PolygonOnTriangulation)& BRep_CurveRepresentation::PolygonOnTriangulation2() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=======================================================================
//function : PolygonOnTriangulation2
//purpose  : 
//=======================================================================

void BRep_CurveRepresentation::PolygonOnTriangulation2
  (const Handle(Poly_PolygonOnTriangulation)&) 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

void BRep_CurveRepresentation::PolygonOnTriangulation
  (const Handle(Poly_PolygonOnTriangulation)&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : PolygonOnTriangulation
//purpose  : 
//=======================================================================

const Handle(Poly_PolygonOnTriangulation)& BRep_CurveRepresentation::PolygonOnTriangulation()
const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}
//=======================================================================
//function : Triangulation
//purpose  : 
//=======================================================================

const Handle(Poly_Triangulation)& BRep_CurveRepresentation::Triangulation()
const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}



//=======================================================================
//function : Polygon2
//purpose  : 
//=======================================================================

const Handle(Poly_Polygon2D)&  BRep_CurveRepresentation::Polygon2()const 
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


//=======================================================================
//function : Polygon2
//purpose  : 
//=======================================================================

void BRep_CurveRepresentation::Polygon2(const Handle(Poly_Polygon2D)&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}


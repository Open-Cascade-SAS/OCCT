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
#include <Geom_Surface.hxx>
#include <Poly_Polygon2D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_CurveRepresentation, Standard_Transient)

//=================================================================================================

bool BRep_CurveRepresentation::IsCurveOnSurface(const occ::handle<Geom_Surface>&,
                                                const TopLoc_Location&) const
{
  return false;
}

//=================================================================================================

bool BRep_CurveRepresentation::IsRegularity(const occ::handle<Geom_Surface>&,
                                            const occ::handle<Geom_Surface>&,
                                            const TopLoc_Location&,
                                            const TopLoc_Location&) const
{
  return false;
}

//=================================================================================================

bool BRep_CurveRepresentation::IsPolygonOnTriangulation(const occ::handle<Poly_Triangulation>&,
                                                        const TopLoc_Location&) const
{
  return false;
}

//=================================================================================================

bool BRep_CurveRepresentation::IsPolygonOnSurface(const occ::handle<Geom_Surface>&,
                                                  const TopLoc_Location&) const
{
  return false;
}

//=================================================================================================

const occ::handle<Geom_Curve>& BRep_CurveRepresentation::Curve3D() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::Curve3D(const occ::handle<Geom_Curve>&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Geom_Surface>& BRep_CurveRepresentation::Surface() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& BRep_CurveRepresentation::PCurve() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Geom2d_Curve>& BRep_CurveRepresentation::PCurve2() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::PCurve(const occ::handle<Geom2d_Curve>&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::PCurve2(const occ::handle<Geom2d_Curve>&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const GeomAbs_Shape& BRep_CurveRepresentation::Continuity() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::Continuity(const GeomAbs_Shape)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Geom_Surface>& BRep_CurveRepresentation::Surface2() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const TopLoc_Location& BRep_CurveRepresentation::Location2() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Poly_Polygon3D>& BRep_CurveRepresentation::Polygon3D() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::Polygon3D(const occ::handle<Poly_Polygon3D>&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Poly_Polygon2D>& BRep_CurveRepresentation::Polygon() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::Polygon(const occ::handle<Poly_Polygon2D>&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Poly_PolygonOnTriangulation>& BRep_CurveRepresentation::PolygonOnTriangulation2()
  const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::PolygonOnTriangulation2(
  const occ::handle<Poly_PolygonOnTriangulation>&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::PolygonOnTriangulation(
  const occ::handle<Poly_PolygonOnTriangulation>&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Poly_PolygonOnTriangulation>& BRep_CurveRepresentation::PolygonOnTriangulation()
  const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Poly_Triangulation>& BRep_CurveRepresentation::Triangulation() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

const occ::handle<Poly_Polygon2D>& BRep_CurveRepresentation::Polygon2() const
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::Polygon2(const occ::handle<Poly_Polygon2D>&)
{
  throw Standard_DomainError("BRep_CurveRepresentation");
}

//=================================================================================================

void BRep_CurveRepresentation::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myLocation)
}

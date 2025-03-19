// Created on: 1995-03-15
// Created by: Laurent PAINNOT
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

#include <BRep_CurveRepresentation.hxx>
#include <BRep_Polygon3D.hxx>
#include <Poly_Polygon3D.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_Polygon3D, BRep_CurveRepresentation)

//=================================================================================================

BRep_Polygon3D::BRep_Polygon3D(const Handle(Poly_Polygon3D)& P, const TopLoc_Location& L)
    : BRep_CurveRepresentation(L),
      myPolygon3D(P)
{
}

//=================================================================================================

Standard_Boolean BRep_Polygon3D::IsPolygon3D() const
{
  return Standard_True;
}

//=================================================================================================

const Handle(Poly_Polygon3D)& BRep_Polygon3D::Polygon3D() const
{
  return myPolygon3D;
}

//=================================================================================================

void BRep_Polygon3D::Polygon3D(const Handle(Poly_Polygon3D)& P)
{
  myPolygon3D = P;
}

//=================================================================================================

Handle(BRep_CurveRepresentation) BRep_Polygon3D::Copy() const
{
  Handle(BRep_Polygon3D) P = new BRep_Polygon3D(myPolygon3D, Location());
  return P;
}

//=================================================================================================

void BRep_Polygon3D::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, BRep_CurveRepresentation)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myPolygon3D.get())
}

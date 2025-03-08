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
#include <BRep_PolygonOnClosedTriangulation.hxx>
#include <Poly_PolygonOnTriangulation.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_PolygonOnClosedTriangulation, BRep_PolygonOnTriangulation)

//=================================================================================================

BRep_PolygonOnClosedTriangulation::BRep_PolygonOnClosedTriangulation(
  const Handle(Poly_PolygonOnTriangulation)& P1,
  const Handle(Poly_PolygonOnTriangulation)& P2,
  const Handle(Poly_Triangulation)&          T,
  const TopLoc_Location&                     L)
    : BRep_PolygonOnTriangulation(P1, T, L),
      myPolygon2(P2)
{
}

//=================================================================================================

Standard_Boolean BRep_PolygonOnClosedTriangulation::IsPolygonOnClosedTriangulation() const
{
  return Standard_True;
}

//=================================================================================================

void BRep_PolygonOnClosedTriangulation::PolygonOnTriangulation2(
  const Handle(Poly_PolygonOnTriangulation)& P2)
{
  myPolygon2 = P2;
}

//=================================================================================================

const Handle(Poly_PolygonOnTriangulation)& BRep_PolygonOnClosedTriangulation::
  PolygonOnTriangulation2() const
{
  return myPolygon2;
}

//=================================================================================================

Handle(BRep_CurveRepresentation) BRep_PolygonOnClosedTriangulation::Copy() const
{
  Handle(BRep_PolygonOnClosedTriangulation) P;
  P = new BRep_PolygonOnClosedTriangulation(PolygonOnTriangulation(),
                                            myPolygon2,
                                            Triangulation(),
                                            Location());

  return P;
}

//=================================================================================================

void BRep_PolygonOnClosedTriangulation::DumpJson(Standard_OStream& theOStream,
                                                 Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, BRep_PolygonOnTriangulation)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myPolygon2.get())
}

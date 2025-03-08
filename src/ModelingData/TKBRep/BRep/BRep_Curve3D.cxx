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

#include <BRep_Curve3D.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_Curve3D, BRep_GCurve)

//=================================================================================================

BRep_Curve3D::BRep_Curve3D(const Handle(Geom_Curve)& C, const TopLoc_Location& L)
    : BRep_GCurve(L,
                  C.IsNull() ? RealFirst() : C->FirstParameter(),
                  C.IsNull() ? RealLast() : C->LastParameter()),
      myCurve(C)
{
}

//=================================================================================================

void BRep_Curve3D::D0(const Standard_Real U, gp_Pnt& P) const
{
  // should be D0 NYI
  P = myCurve->Value(U);
}

//=================================================================================================

Standard_Boolean BRep_Curve3D::IsCurve3D() const
{
  return Standard_True;
}

//=================================================================================================

const Handle(Geom_Curve)& BRep_Curve3D::Curve3D() const
{
  return myCurve;
}

//=================================================================================================

void BRep_Curve3D::Curve3D(const Handle(Geom_Curve)& C)
{
  myCurve = C;
}

//=================================================================================================

Handle(BRep_CurveRepresentation) BRep_Curve3D::Copy() const
{
  Handle(BRep_Curve3D) C = new BRep_Curve3D(myCurve, Location());

  C->SetRange(First(), Last());
  return C;
}

//=================================================================================================

void BRep_Curve3D::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, BRep_GCurve)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myCurve.get())
}

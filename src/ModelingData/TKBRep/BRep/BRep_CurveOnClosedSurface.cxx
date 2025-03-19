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

#include <BRep_CurveOnClosedSurface.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopLoc_Location.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_CurveOnClosedSurface, BRep_CurveOnSurface)

//=================================================================================================

BRep_CurveOnClosedSurface::BRep_CurveOnClosedSurface(const Handle(Geom2d_Curve)& PC1,
                                                     const Handle(Geom2d_Curve)& PC2,
                                                     const Handle(Geom_Surface)& S,
                                                     const TopLoc_Location&      L,
                                                     const GeomAbs_Shape         C)
    : BRep_CurveOnSurface(PC1, S, L),
      myPCurve2(PC2),
      myContinuity(C)
{
}

//=================================================================================================

Standard_Boolean BRep_CurveOnClosedSurface::IsCurveOnClosedSurface() const
{
  return Standard_True;
}

//=================================================================================================

Standard_Boolean BRep_CurveOnClosedSurface::IsRegularity() const
{
  return Standard_True;
}

//=================================================================================================

Standard_Boolean BRep_CurveOnClosedSurface::IsRegularity(const Handle(Geom_Surface)& S1,
                                                         const Handle(Geom_Surface)& S2,
                                                         const TopLoc_Location&      L1,
                                                         const TopLoc_Location&      L2) const
{
  return ((Surface() == S1) && (Surface() == S2) && (Location() == L1) && (Location() == L2));
}

//=================================================================================================

const Handle(Geom2d_Curve)& BRep_CurveOnClosedSurface::PCurve2() const
{
  return myPCurve2;
}

//=================================================================================================

const GeomAbs_Shape& BRep_CurveOnClosedSurface::Continuity() const
{
  return myContinuity;
}

//=================================================================================================

const Handle(Geom_Surface)& BRep_CurveOnClosedSurface::Surface2() const
{
  return Surface();
}

//=================================================================================================

const TopLoc_Location& BRep_CurveOnClosedSurface::Location2() const
{
  return Location();
}

//=================================================================================================

void BRep_CurveOnClosedSurface::PCurve2(const Handle(Geom2d_Curve)& C)
{
  myPCurve2 = C;
}

//=================================================================================================

void BRep_CurveOnClosedSurface::Continuity(const GeomAbs_Shape C)
{
  myContinuity = C;
}

//=================================================================================================

Handle(BRep_CurveRepresentation) BRep_CurveOnClosedSurface::Copy() const
{
  Handle(BRep_CurveOnClosedSurface) C =
    new BRep_CurveOnClosedSurface(PCurve(), PCurve2(), Surface(), Location(), myContinuity);

  C->SetRange(First(), Last());
  C->SetUVPoints(myUV1, myUV2);
  C->SetUVPoints2(myUV21, myUV22);

  return C;
}

//=================================================================================================

void BRep_CurveOnClosedSurface::Update()
{
  if (!Precision::IsNegativeInfinite(First()))
    myPCurve2->D0(First(), myUV21);
  if (!Precision::IsPositiveInfinite(Last()))
    myPCurve2->D0(Last(), myUV22);
  BRep_CurveOnSurface::Update();
}

//=================================================================================================

void BRep_CurveOnClosedSurface::DumpJson(Standard_OStream& theOStream,
                                         Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, BRep_CurveOnSurface)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, myPCurve2.get())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myContinuity)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myUV21)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myUV22)
}

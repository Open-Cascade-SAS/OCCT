// Created on: 1992-08-25
// Created by: Modelistation
// Copyright (c) 1992-1999 Matra Datavision
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
#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_Polygon3D.hxx>
#include <BRep_TEdge.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_TEdge, TopoDS_TEdge)

static const int ParameterMask   = 1;
static const int RangeMask       = 2;
static const int DegeneratedMask = 4;

//=================================================================================================

BRep_TEdge::BRep_TEdge()
    : myTolerance(Precision::Confusion()),
      myFlags(0)
{
  SameParameter(true);
  SameRange(true);
}

//=================================================================================================

bool BRep_TEdge::SameParameter() const
{
  return (myFlags & ParameterMask) != 0;
}

//=================================================================================================

void BRep_TEdge::SameParameter(const bool S)
{
  if (S)
    myFlags |= ParameterMask;
  else
    myFlags &= ~ParameterMask;
}

//=================================================================================================

bool BRep_TEdge::SameRange() const
{
  return (myFlags & RangeMask) != 0;
}

//=================================================================================================

void BRep_TEdge::SameRange(const bool S)
{
  if (S)
    myFlags |= RangeMask;
  else
    myFlags &= ~RangeMask;
}

//=================================================================================================

bool BRep_TEdge::Degenerated() const
{
  return (myFlags & DegeneratedMask) != 0;
}

//=================================================================================================

void BRep_TEdge::Degenerated(const bool S)
{
  if (S)
    myFlags |= DegeneratedMask;
  else
    myFlags &= ~DegeneratedMask;
}

//=================================================================================================

occ::handle<TopoDS_TShape> BRep_TEdge::EmptyCopy() const
{
  occ::handle<BRep_TEdge> TE = new BRep_TEdge();
  TE->Tolerance(myTolerance);
  // Copy the curves representations (polygons are NOT copied).
  NCollection_List<occ::handle<BRep_CurveRepresentation>>&          l = TE->ChangeCurves();
  NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itr(myCurves);

  while (itr.More())
  {
    const BRep_CurveRepKind kind = itr.Value()->RepresentationKind();
    if (kind == BRep_CurveRepKind::Curve3D || kind == BRep_CurveRepKind::CurveOnSurface
        || kind == BRep_CurveRepKind::CurveOnClosedSurface
        || kind == BRep_CurveRepKind::CurveOn2Surfaces)
    {
      occ::handle<BRep_CurveRepresentation> aCopy = itr.Value()->Copy();
      // Maintain 3D curve cache for the new edge.
      if (kind == BRep_CurveRepKind::Curve3D)
      {
        TE->Curve3D(occ::down_cast<BRep_Curve3D>(aCopy));
      }
      l.Append(aCopy);
    }
    itr.Next();
  }

  TE->Degenerated(Degenerated());
  TE->SameParameter(SameParameter());
  TE->SameRange(SameRange());

  return TE;
}

//=================================================================================================

void BRep_TEdge::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TopoDS_TEdge)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTolerance)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFlags)

  for (NCollection_List<occ::handle<BRep_CurveRepresentation>>::Iterator itr(myCurves); itr.More();
       itr.Next())
  {
    const occ::handle<BRep_CurveRepresentation>& aCurveRepresentation = itr.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aCurveRepresentation.get())
  }
}

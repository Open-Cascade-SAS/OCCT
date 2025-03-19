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

#include <BRep_CurveOn2Surfaces.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <BRep_GCurve.hxx>
#include <BRep_TEdge.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BRep_TEdge, TopoDS_TEdge)

static const Standard_Integer ParameterMask   = 1;
static const Standard_Integer RangeMask       = 2;
static const Standard_Integer DegeneratedMask = 4;

//=================================================================================================

BRep_TEdge::BRep_TEdge()
    : TopoDS_TEdge(),
      myTolerance(RealEpsilon()),
      myFlags(0)
{
  SameParameter(Standard_True);
  SameRange(Standard_True);
}

//=================================================================================================

Standard_Boolean BRep_TEdge::SameParameter() const
{
  return (myFlags & ParameterMask) != 0;
}

//=================================================================================================

void BRep_TEdge::SameParameter(const Standard_Boolean S)
{
  if (S)
    myFlags |= ParameterMask;
  else
    myFlags &= ~ParameterMask;
}

//=================================================================================================

Standard_Boolean BRep_TEdge::SameRange() const
{
  return (myFlags & RangeMask) != 0;
}

//=================================================================================================

void BRep_TEdge::SameRange(const Standard_Boolean S)
{
  if (S)
    myFlags |= RangeMask;
  else
    myFlags &= ~RangeMask;
}

//=================================================================================================

Standard_Boolean BRep_TEdge::Degenerated() const
{
  return (myFlags & DegeneratedMask) != 0;
}

//=================================================================================================

void BRep_TEdge::Degenerated(const Standard_Boolean S)
{
  if (S)
    myFlags |= DegeneratedMask;
  else
    myFlags &= ~DegeneratedMask;
}

//=================================================================================================

Handle(TopoDS_TShape) BRep_TEdge::EmptyCopy() const
{
  Handle(BRep_TEdge) TE = new BRep_TEdge();
  TE->Tolerance(myTolerance);
  // copy the curves representations
  BRep_ListOfCurveRepresentation&              l = TE->ChangeCurves();
  BRep_ListIteratorOfListOfCurveRepresentation itr(myCurves);

  while (itr.More())
  {
    // on ne recopie PAS les polygones
    if (itr.Value()->IsKind(STANDARD_TYPE(BRep_GCurve))
        || itr.Value()->IsKind(STANDARD_TYPE(BRep_CurveOn2Surfaces)))
    {
      l.Append(itr.Value()->Copy());
    }
    itr.Next();
  }

  TE->Degenerated(Degenerated());
  TE->SameParameter(SameParameter());
  TE->SameRange(SameRange());

  return TE;
}

//=================================================================================================

void BRep_TEdge::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TopoDS_TEdge)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTolerance)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFlags)

  for (BRep_ListIteratorOfListOfCurveRepresentation itr(myCurves); itr.More(); itr.Next())
  {
    const Handle(BRep_CurveRepresentation)& aCurveRepresentation = itr.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aCurveRepresentation.get())
  }
}

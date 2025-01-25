// Created on: 1996-02-13
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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

#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

BRepOffsetAPI_MakeOffsetShape::BRepOffsetAPI_MakeOffsetShape()
    : myLastUsedAlgo(OffsetAlgo_NONE)
{
}

//=================================================================================================

void BRepOffsetAPI_MakeOffsetShape::PerformByJoin(const TopoDS_Shape&          S,
                                                  const Standard_Real          Offset,
                                                  const Standard_Real          Tol,
                                                  const BRepOffset_Mode        Mode,
                                                  const Standard_Boolean       Intersection,
                                                  const Standard_Boolean       SelfInter,
                                                  const GeomAbs_JoinType       Join,
                                                  const Standard_Boolean       RemoveIntEdges,
                                                  const Message_ProgressRange& theRange)
{
  NotDone();
  myLastUsedAlgo = OffsetAlgo_JOIN;

  myOffsetShape.Initialize(S,
                           Offset,
                           Tol,
                           Mode,
                           Intersection,
                           SelfInter,
                           Join,
                           Standard_False,
                           RemoveIntEdges);
  myOffsetShape.MakeOffsetShape(theRange);

  if (!myOffsetShape.IsDone())
    return;

  myShape = myOffsetShape.Shape();
  Done();
}

//=================================================================================================

void BRepOffsetAPI_MakeOffsetShape::PerformBySimple(const TopoDS_Shape& theS,
                                                    const Standard_Real theOffsetValue)
{
  NotDone();
  myLastUsedAlgo = OffsetAlgo_SIMPLE;

  mySimpleOffsetShape.Initialize(theS, theOffsetValue);
  mySimpleOffsetShape.Perform();

  if (!mySimpleOffsetShape.IsDone())
    return;

  myShape = mySimpleOffsetShape.GetResultShape();
  Done();
}

//=================================================================================================

const BRepOffset_MakeOffset& BRepOffsetAPI_MakeOffsetShape::MakeOffset() const
{
  return myOffsetShape;
}

//=================================================================================================

void BRepOffsetAPI_MakeOffsetShape::Build(const Message_ProgressRange& /*theRange*/) {}

//=================================================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeOffsetShape::Generated(const TopoDS_Shape& S)
{
  myGenerated.Clear();
  if (myLastUsedAlgo == OffsetAlgo_JOIN)
  {
    myGenerated = myOffsetShape.Generated(S);
  }
  else if (myLastUsedAlgo == OffsetAlgo_SIMPLE)
  {
    TopoDS_Shape aGenShape = mySimpleOffsetShape.Generated(S);
    if (!aGenShape.IsNull() && !aGenShape.IsSame(S))
      myGenerated.Append(aGenShape);
  }

  return myGenerated;
}

//=================================================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeOffsetShape::Modified(const TopoDS_Shape& S)
{
  myGenerated.Clear();
  if (myLastUsedAlgo == OffsetAlgo_JOIN)
  {
    myGenerated = myOffsetShape.Modified(S);
  }
  else if (myLastUsedAlgo == OffsetAlgo_SIMPLE)
  {
    TopoDS_Shape aGenShape = mySimpleOffsetShape.Modified(S);
    if (!aGenShape.IsNull() && !aGenShape.IsSame(S))
      myGenerated.Append(aGenShape);
  }

  return myGenerated;
}

//=================================================================================================

Standard_Boolean BRepOffsetAPI_MakeOffsetShape::IsDeleted(const TopoDS_Shape& S)
{
  if (myLastUsedAlgo == OffsetAlgo_JOIN)
  {
    return myOffsetShape.IsDeleted(S);
  }
  return Standard_False;
}

//=======================================================================
// function : GetJoinType
// purpose  : Query offset join type.
//=======================================================================
GeomAbs_JoinType BRepOffsetAPI_MakeOffsetShape::GetJoinType() const
{
  return myOffsetShape.GetJoinType();
}

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

#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================

BRepOffsetAPI_MakeThickSolid::BRepOffsetAPI_MakeThickSolid()
{
  // Build only solids.
  mySimpleOffsetShape.SetBuildSolidFlag(Standard_True);
}

//=================================================================================================

void BRepOffsetAPI_MakeThickSolid::MakeThickSolidByJoin(const TopoDS_Shape&          S,
                                                        const TopTools_ListOfShape&  ClosingFaces,
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
  TopTools_ListIteratorOfListOfShape it(ClosingFaces);
  for (; it.More(); it.Next())
    myOffsetShape.AddFace(TopoDS::Face(it.Value()));

  myOffsetShape.MakeThickSolid(theRange);
  if (!myOffsetShape.IsDone())
    return;

  myShape = myOffsetShape.Shape();
  Done();
}

//=================================================================================================

void BRepOffsetAPI_MakeThickSolid::MakeThickSolidBySimple(const TopoDS_Shape& theS,
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

void BRepOffsetAPI_MakeThickSolid::Build(const Message_ProgressRange& /*theRange*/) {}

//=================================================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeThickSolid::Modified(const TopoDS_Shape& F)
{
  myGenerated.Clear();

  if (myLastUsedAlgo == OffsetAlgo_JOIN && myOffsetShape.OffsetFacesFromShapes().HasImage(F))
  {
    if (myOffsetShape.ClosingFaces().Contains(F))
    {
      myOffsetShape.OffsetFacesFromShapes().LastImage(F, myGenerated);

      // Reverse generated shapes in case of small solids.
      // Useful only for faces without influence on others.
      TopTools_ListIteratorOfListOfShape it(myGenerated);
      for (; it.More(); it.Next())
        it.ChangeValue().Reverse();
    }
  }
  else if (myLastUsedAlgo == OffsetAlgo_SIMPLE)
  {
    TopoDS_Shape aModShape = mySimpleOffsetShape.Modified(F);
    if (!aModShape.IsNull())
      myGenerated.Append(aModShape);
  }

  return myGenerated;
}

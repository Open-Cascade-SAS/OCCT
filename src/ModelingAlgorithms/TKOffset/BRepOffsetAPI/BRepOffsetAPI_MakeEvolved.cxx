// Created on: 1995-09-18
// Created by: Bruno DUMORTIER
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

#include <BRepFill.hxx>
#include <BRepOffsetAPI_MakeEvolved.hxx>
#include <gp_Ax3.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS.hxx>

static const TopTools_ListOfShape anEmptyList;

//=================================================================================================

BRepOffsetAPI_MakeEvolved::BRepOffsetAPI_MakeEvolved() {}

//=================================================================================================

BRepOffsetAPI_MakeEvolved::BRepOffsetAPI_MakeEvolved(const TopoDS_Shape&    Spine,
                                                     const TopoDS_Wire&     Profil,
                                                     const GeomAbs_JoinType Join,
                                                     const Standard_Boolean AxeProf,
                                                     const Standard_Boolean Solid,
                                                     const Standard_Boolean ProfOnSpine,
                                                     const Standard_Real    Tol,
                                                     const Standard_Boolean theIsVolume,
                                                     const Standard_Boolean theRunInParallel)
    : myIsVolume(theIsVolume)
{
  if (Spine.ShapeType() != TopAbs_WIRE && Spine.ShapeType() != TopAbs_FACE)
  {
    Standard_TypeMismatch::Raise("BRepOffsetAPI_MakeEvolved: face or wire is expected as a spine");
  }
  if (theIsVolume)
  {
    myVolume.SetParallelMode(theRunInParallel);
    TopoDS_Wire aSpine;
    if (Spine.ShapeType() == TopAbs_WIRE)
    {
      aSpine = TopoDS::Wire(Spine);
    }
    else
    {
      aSpine = TopoDS::Wire(TopoDS_Iterator(Spine).Value());
    }
    myVolume.Perform(aSpine, Profil, Tol, Solid);
    if (!myVolume.IsDone())
    {
      return;
    }
  }
  else
  {
    gp_Ax3 Axis(gp::Origin(), gp::DZ(), gp::DX());

    if (!AxeProf)
    {
      Standard_Boolean POS;
      BRepFill::Axe(Spine, Profil, Axis, POS, Max(Tol, Precision::Confusion()));
      if (ProfOnSpine && !POS)
        return;
    }
    if (Spine.ShapeType() == TopAbs_WIRE)
    {
      myEvolved.Perform(TopoDS::Wire(Spine), Profil, Axis, Join, Solid);
    }
    else
    {
      myEvolved.Perform(TopoDS::Face(Spine), Profil, Axis, Join, Solid);
    }
  }

  Build();
}

//=======================================================================
// function : BRepFill_Evolved&
// purpose  :
//=======================================================================

const BRepFill_Evolved& BRepOffsetAPI_MakeEvolved::Evolved() const
{
  if (myIsVolume)
  {
    Standard_TypeMismatch::Raise(
      "BRepOffsetAPI_MakeEvolved: myEvolved is accessed while in volume mode");
  }
  return myEvolved;
}

//=================================================================================================

void BRepOffsetAPI_MakeEvolved::Build(const Message_ProgressRange& /*theRange*/)
{
  if (myEvolved.IsDone())
  {
    myShape = myEvolved.Shape();
  }
  else if (myVolume.IsDone())
  {
    myShape = myVolume.Shape();
  }

  Done();
}

//=================================================================================================

const TopoDS_Shape& BRepOffsetAPI_MakeEvolved::Top() const
{
  return myEvolved.Top();
}

//=================================================================================================

const TopoDS_Shape& BRepOffsetAPI_MakeEvolved::Bottom() const
{
  return myEvolved.Bottom();
}

//=================================================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeEvolved::GeneratedShapes(
  const TopoDS_Shape& SpineShape,
  const TopoDS_Shape& ProfShape) const
{
  if (!myEvolved.IsDone())
    return anEmptyList;

  return myEvolved.GeneratedShapes(SpineShape, ProfShape);
}

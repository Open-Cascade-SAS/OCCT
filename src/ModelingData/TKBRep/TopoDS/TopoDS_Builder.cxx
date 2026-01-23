// Created on: 1991-04-09
// Created by: Remi LEQUETTE
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Standard_NullObject.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_FrozenShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>

//==================================================================================================

void TopoDS_Builder::MakeShape(TopoDS_Shape& S, const occ::handle<TopoDS_TShape>& T) const
{
  S.TShape(T);
  S.Location(TopLoc_Location());
  S.Orientation(TopAbs_FORWARD);
}

//==================================================================================================

void TopoDS_Builder::Add(TopoDS_Shape& aShape, const TopoDS_Shape& aComponent) const
{
  // From now the Component cannot be edited
  aComponent.TShape()->Free(false);

  // Note that freezing aComponent before testing if aShape is free
  // prevents from self-insertion
  // but aShape will be frozen when the Exception is raised
  if (aShape.Free())
  {
    static const unsigned int aTb[9] = {
      // COMPOUND to:
      (1 << ((unsigned int)TopAbs_COMPOUND)),
      // COMPSOLID to:
      (1 << ((unsigned int)TopAbs_COMPOUND)),
      // SOLID to:
      (1 << ((unsigned int)TopAbs_COMPOUND)) | (1 << ((unsigned int)TopAbs_COMPSOLID)),
      // SHELL to:
      (1 << ((unsigned int)TopAbs_COMPOUND)) | (1 << ((unsigned int)TopAbs_SOLID)),
      // FACE to:
      (1 << ((unsigned int)TopAbs_COMPOUND)) | (1 << ((unsigned int)TopAbs_SHELL)),
      // WIRE to:
      (1 << ((unsigned int)TopAbs_COMPOUND)) | (1 << ((unsigned int)TopAbs_FACE)),
      // EDGE to:
      (1 << ((unsigned int)TopAbs_COMPOUND)) | (1 << ((unsigned int)TopAbs_SOLID))
        | (1 << ((unsigned int)TopAbs_WIRE)),
      // VERTEX to:
      (1 << ((unsigned int)TopAbs_COMPOUND)) | (1 << ((unsigned int)TopAbs_SOLID))
        | (1 << ((unsigned int)TopAbs_FACE)) | (1 << ((unsigned int)TopAbs_EDGE)),
      // SHAPE to:
      0};

    const unsigned int iC = (unsigned int)aComponent.ShapeType();
    const unsigned int iS = (unsigned int)aShape.ShapeType();

    if ((aTb[iC] & (1 << iS)) != 0)
    {
      TopoDS_Shape aChild = aComponent;

      // compute the relative Orientation
      if (aShape.Orientation() == TopAbs_REVERSED)
        aChild.Reverse();

      // and the Relative Location
      const TopLoc_Location& aLoc = aShape.Location();
      if (!aLoc.IsIdentity())
        aChild.Move(aLoc.Inverted(), false);

      // Add to the subshapes list
      TopoDS_TShape* aTShape = aShape.TShape().get();
      aTShape->myShapes.Append(aChild);
      aTShape->Modified(true);
    }
    else
    {
      throw TopoDS_UnCompatibleShapes("TopoDS_Builder::Add");
    }
  }
  else
  {
    throw TopoDS_FrozenShape("TopoDS_Builder::Add");
  }
}

//==================================================================================================

void TopoDS_Builder::Remove(TopoDS_Shape& aShape, const TopoDS_Shape& aComponent) const
{
  // check if aShape is not Frozen
  TopoDS_FrozenShape_Raise_if(!aShape.Free(), "TopoDS_Builder::Remove");

  // compute the relative Orientation and Location of aComponent
  TopoDS_Shape S = aComponent;
  if (aShape.Orientation() == TopAbs_REVERSED)
    S.Reverse();
  S.Location(S.Location().Predivided(aShape.Location()), false);

  TopoDS_TShape*                           aTShape = aShape.TShape().get();
  NCollection_List<TopoDS_Shape>&          aList   = aTShape->myShapes;
  NCollection_List<TopoDS_Shape>::Iterator anIter(aList);

  while (anIter.More())
  {
    if (anIter.Value() == S)
    {
      aList.Remove(anIter);
      aTShape->Modified(true);
      return;
    }
    anIter.Next();
  }
}

// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <ShapePersistent_TopoDS.hxx>

#include <BRep_Builder.hxx>


enum
{
  ModifiedMask   = 2,
  CheckedMask    = 4,
  OrientableMask = 8,
  ClosedMask     = 16,
  InfiniteMask   = 32,
  ConvexMask     = 64
};


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void ShapePersistent_TopoDS::HShape::Read (StdObjMgt_ReadData& theReadData)
{
  theReadData >> myEntry;
  StdObject_Shape::read (theReadData);
}

void ShapePersistent_TopoDS::pTBase::setFlags
  (const Handle(TopoDS_TShape)& theTShape) const
{
  theTShape->Free       (Standard_False); // Always frozen when coming from DB
  theTShape->Modified   (myFlags & ModifiedMask);
  theTShape->Checked    (myFlags & CheckedMask);
  theTShape->Orientable (myFlags & OrientableMask);
  theTShape->Closed     (myFlags & ClosedMask);
  theTShape->Infinite   (myFlags & InfiniteMask);
  theTShape->Convex     (myFlags & ConvexMask);
}

static inline void AddShape
  (TopoDS_Shape& theParent, const Handle(StdObjMgt_Persistent)& theRef)
{
  Handle(ShapePersistent_TopoDS::HShape) aShape =
    Handle(ShapePersistent_TopoDS::HShape)::DownCast (theRef);

  if (aShape)
    BRep_Builder().Add (theParent, aShape->Import());
}

static inline void AddShape
  (TopoDS_Shape& theParent, const StdObject_Shape& theShape)
{
  BRep_Builder().Add (theParent, theShape.Import());
}

template <class ShapesArray>
void ShapePersistent_TopoDS::pTBase::addShapesT
  (TopoDS_Shape& theParent) const
{
  Handle(ShapesArray) aShapes = Handle(ShapesArray)::DownCast (myShapes);
  if (aShapes)
  {
    typename ShapesArray::Iterator anIter (*aShapes->Array());
    for (; anIter.More(); anIter.Next())
      AddShape (theParent, anIter.Value());
  }
}

template void ShapePersistent_TopoDS::pTBase::addShapesT
  <StdLPersistent_HArray1::Persistent> (TopoDS_Shape& theParent) const;

template void ShapePersistent_TopoDS::pTBase::addShapesT
  <StdPersistent_HArray1::Shape1> (TopoDS_Shape& theParent) const;

template <class Target>
Handle(TopoDS_TShape)
  ShapePersistent_TopoDS::pTSimple<Target>::createTShape() const
    { return new Target; }

template class ShapePersistent_TopoDS::pTSimple<TopoDS_TWire>;
template class ShapePersistent_TopoDS::pTSimple<TopoDS_TShell>;
template class ShapePersistent_TopoDS::pTSimple<TopoDS_TSolid>;
template class ShapePersistent_TopoDS::pTSimple<TopoDS_TCompSolid>;
template class ShapePersistent_TopoDS::pTSimple<TopoDS_TCompound>;

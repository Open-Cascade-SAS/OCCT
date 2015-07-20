// Created on: 2015-06-30
// Created by: Anton POLETAEV
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

#include <SelectMgr_SelectableObjectTrsfPersSet.hxx>

#include <Bnd_Box.hxx>
#include <BVH_LinearBuilder.hxx>
#include <SelectMgr_VectorTypes.hxx>

//=======================================================================
// function : SelectMgr_SelectableObjectTrsfPersSet
// purpose  :
//=======================================================================
SelectMgr_SelectableObjectTrsfPersSet::SelectMgr_SelectableObjectTrsfPersSet()
: myIsDirty (Standard_False),
  myBVH (new BVH_Tree<Standard_Real, 3>())
{
  myBuilder = new BVH_LinearBuilder<Standard_Real, 3> (1, 32);
}

//=======================================================================
// function : Size
// purpose  :
//=======================================================================
Standard_Integer SelectMgr_SelectableObjectTrsfPersSet::Size() const
{
  return myObjects.Size();
}

//=======================================================================
// function : Box
// purpose  :
//=======================================================================
Select3D_BndBox3d SelectMgr_SelectableObjectTrsfPersSet::Box (const Standard_Integer theIndex) const
{
  return *myObjectBoxes (theIndex + 1);
}

//=======================================================================
// function : Center
// purpose  :
//=======================================================================
Standard_Real SelectMgr_SelectableObjectTrsfPersSet::Center (const Standard_Integer theIndex,
                                                             const Standard_Integer theAxis) const
{
  const Select3D_BndBox3d& aBndBox = *myObjectBoxes (theIndex + 1);

  return (aBndBox.CornerMin()[theAxis] + aBndBox.CornerMax()[theAxis]) * 0.5;
}

//=======================================================================
// function : Swap
// purpose  :
//=======================================================================
void SelectMgr_SelectableObjectTrsfPersSet::Swap (const Standard_Integer theIndex1,
                                                  const Standard_Integer theIndex2)
{
  const Standard_Integer aStructIdx1 = theIndex1 + 1;
  const Standard_Integer aStructIdx2 = theIndex2 + 1;

  myObjects.Swap (aStructIdx1, aStructIdx2);
  myObjectBoxes.Swap (aStructIdx1, aStructIdx2);
}

//=======================================================================
// function : Append
// purpose  :
//=======================================================================
Standard_Boolean SelectMgr_SelectableObjectTrsfPersSet::Append (const Handle(SelectMgr_SelectableObject)& theObject)
{
  Standard_Integer aSize = Size();

  if (aSize < myObjects.Add (theObject))
  {
    MarkDirty();

    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
// function : Remove
// purpose  :
//=======================================================================
Standard_Boolean SelectMgr_SelectableObjectTrsfPersSet::Remove (const Handle(SelectMgr_SelectableObject)& theObject)
{
  const Standard_Integer anIndex = myObjects.FindIndex (theObject);

  if (anIndex != 0)
  {
    myObjects.Swap (Size(), anIndex);
    myObjects.RemoveLast();
    MarkDirty();

    return Standard_True;
  }

  return Standard_False;
}

//=======================================================================
// function : BVH
// purpose  :
//=======================================================================
const NCollection_Handle<BVH_Tree<Standard_Real, 3> >&
  SelectMgr_SelectableObjectTrsfPersSet::BVH (const Graphic3d_Mat4d& theProjectionMatrix,
                                              const Graphic3d_Mat4d& theWorldViewMatrix,
                                              const Graphic3d_WorldViewProjState& theWVPState)
{
  if (!myIsDirty && (myObjectBoxesState.IsValid() && !myObjectBoxesState.IsChanged(theWVPState)))
  {
    return myBVH;
  }

  myObjectBoxes.ReSize (myObjects.Size());

  for (Standard_Integer anObjectIdx = 1; anObjectIdx <= myObjects.Size(); ++anObjectIdx)
  {
    const Handle(SelectMgr_SelectableObject)& anObject = myObjects (anObjectIdx);

    Bnd_Box aBoundingBox;

    if (anObject->TransformPersistence().Flags && !(anObject->TransformPersistence().Flags & Graphic3d_TMF_2d))
    {
      anObject->BoundingBox (aBoundingBox);
      anObject->TransformPersistence().Apply (theProjectionMatrix, theWorldViewMatrix, 0, 0, aBoundingBox);
    }

    if (aBoundingBox.IsVoid())
    {
      myObjectBoxes.Add (new Select3D_BndBox3d());
    }
    else
    {
      gp_Pnt aMin = aBoundingBox.CornerMin();
      gp_Pnt aMax = aBoundingBox.CornerMax();

      myObjectBoxes.Add (new Select3D_BndBox3d (Select3D_Vec3 (aMin.X(), aMin.Y(), aMin.Z()),
                                                Select3D_Vec3 (aMax.X(), aMax.Y(), aMax.Z())));
    }
  }

  myBuilder->Build (this, myBVH.operator->(), BVH_Set<Standard_Real, 3>::Box());

  myObjectBoxesState = theWVPState;
  myObjectBoxes.Clear();
  myIsDirty = Standard_False;

  return myBVH;
}

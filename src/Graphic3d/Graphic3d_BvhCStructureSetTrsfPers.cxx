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

#include <Graphic3d_BvhCStructureSetTrsfPers.hxx>

#include <Graphic3d_CStructure.hxx>

// =======================================================================
// function : Graphic3d_BvhCStructureSetTrsfPers
// purpose  :
// =======================================================================
Graphic3d_BvhCStructureSetTrsfPers::Graphic3d_BvhCStructureSetTrsfPers(
  const Handle(BVH_Builder3d)& theBuilder)
    : myIsDirty(Standard_False),
      myBVH(new BVH_Tree<Standard_Real, 3>()),
      myBuilder(theBuilder)
{
  //
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
Standard_Integer Graphic3d_BvhCStructureSetTrsfPers::Size() const
{
  return myStructs.Size();
}

// =======================================================================
// function : Box
// purpose  :
// =======================================================================
Graphic3d_BndBox3d Graphic3d_BvhCStructureSetTrsfPers::Box(const Standard_Integer theIdx) const
{
  return *myStructBoxes(theIdx + 1);
}

// =======================================================================
// function : Center
// purpose  :
// =======================================================================
Standard_Real Graphic3d_BvhCStructureSetTrsfPers::Center(const Standard_Integer theIdx,
                                                         const Standard_Integer theAxis) const
{
  const Graphic3d_BndBox3d& aBndBox = *myStructBoxes(theIdx + 1);
  return (aBndBox.CornerMin()[theAxis] + aBndBox.CornerMax()[theAxis]) * 0.5;
}

// =======================================================================
// function : Swap
// purpose  :
// =======================================================================
void Graphic3d_BvhCStructureSetTrsfPers::Swap(const Standard_Integer theIdx1,
                                              const Standard_Integer theIdx2)
{
  const Standard_Integer aStructIdx1 = theIdx1 + 1;
  const Standard_Integer aStructIdx2 = theIdx2 + 1;

  myStructs.Swap(aStructIdx1, aStructIdx2);
  myStructBoxes.Swap(aStructIdx1, aStructIdx2);
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_BvhCStructureSetTrsfPers::Add(const Graphic3d_CStructure* theStruct)
{
  const Standard_Integer aSize = myStructs.Size();

  if (myStructs.Add(theStruct) > aSize) // new structure?
  {
    MarkDirty();

    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_BvhCStructureSetTrsfPers::Remove(const Graphic3d_CStructure* theStruct)
{
  const Standard_Integer anIndex = myStructs.FindIndex(theStruct);

  if (anIndex != 0)
  {
    myStructs.Swap(Size(), anIndex);
    myStructs.RemoveLast();
    MarkDirty();

    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void Graphic3d_BvhCStructureSetTrsfPers::Clear()
{
  myStructs.Clear();
  MarkDirty();
}

// =======================================================================
// function : GetStructureById
// purpose  :
// =======================================================================
const Graphic3d_CStructure* Graphic3d_BvhCStructureSetTrsfPers::GetStructureById(
  Standard_Integer theId)
{
  return myStructs.FindKey(theId + 1);
}

//=================================================================================================

const opencascade::handle<BVH_Tree<Standard_Real, 3>>& Graphic3d_BvhCStructureSetTrsfPers::BVH(
  const Handle(Graphic3d_Camera)&     theCamera,
  const Graphic3d_Mat4d&              theProjectionMatrix,
  const Graphic3d_Mat4d&              theWorldViewMatrix,
  const Standard_Integer              theViewportWidth,
  const Standard_Integer              theViewportHeight,
  const Graphic3d_WorldViewProjState& theWVPState)
{
  if (!myIsDirty && (myStructBoxesState.IsValid() && !myStructBoxesState.IsChanged(theWVPState)))
  {
    return myBVH;
  }

  myStructBoxes.ReSize(myStructs.Size());

  for (Standard_Integer aStructIdx = 1; aStructIdx <= myStructs.Size(); ++aStructIdx)
  {
    const Graphic3d_CStructure* aStructure = myStructs(aStructIdx);

    Handle(HBndBox3d) aBoundingBox = new HBndBox3d();
    *aBoundingBox                  = aStructure->BoundingBox();
    if (!aStructure->TransformPersistence().IsNull())
    {
      aStructure->TransformPersistence()->Apply(theCamera,
                                                theProjectionMatrix,
                                                theWorldViewMatrix,
                                                theViewportWidth,
                                                theViewportHeight,
                                                *aBoundingBox);
    }

    myStructBoxes.Add(aBoundingBox);
  }

  myBuilder->Build(this, myBVH.operator->(), BVH_Set<Standard_Real, 3>::Box());

  myStructBoxesState = theWVPState;
  myStructBoxes.Clear();
  myIsDirty = Standard_False;

  return myBVH;
}

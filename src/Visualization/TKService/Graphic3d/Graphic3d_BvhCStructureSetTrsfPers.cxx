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

//=================================================================================================

Graphic3d_BvhCStructureSetTrsfPers::Graphic3d_BvhCStructureSetTrsfPers(
  const occ::handle<BVH_Builder3d>& theBuilder)
    : myIsDirty(false),
      myBVH(new BVH_Tree<double, 3>()),
      myBuilder(theBuilder)
{}

//=================================================================================================

int Graphic3d_BvhCStructureSetTrsfPers::Size() const
{
  return myStructs.Size();
}

//=================================================================================================

Graphic3d_BndBox3d Graphic3d_BvhCStructureSetTrsfPers::Box(const int theIdx) const
{
  return *myStructBoxes(theIdx + 1);
}

//=================================================================================================

double Graphic3d_BvhCStructureSetTrsfPers::Center(const int theIdx, const int theAxis) const
{
  const Graphic3d_BndBox3d& aBndBox = *myStructBoxes(theIdx + 1);
  return (aBndBox.CornerMin()[theAxis] + aBndBox.CornerMax()[theAxis]) * 0.5;
}

//=================================================================================================

void Graphic3d_BvhCStructureSetTrsfPers::Swap(const int theIdx1, const int theIdx2)
{
  const int aStructIdx1 = theIdx1 + 1;
  const int aStructIdx2 = theIdx2 + 1;

  myStructs.Swap(aStructIdx1, aStructIdx2);
  myStructBoxes.Swap(aStructIdx1, aStructIdx2);
}

//=================================================================================================

bool Graphic3d_BvhCStructureSetTrsfPers::Add(const Graphic3d_CStructure* theStruct)
{
  const int aSize = myStructs.Size();

  if (myStructs.Add(theStruct) > aSize) // new structure?
  {
    MarkDirty();

    return true;
  }

  return false;
}

//=================================================================================================

bool Graphic3d_BvhCStructureSetTrsfPers::Remove(const Graphic3d_CStructure* theStruct)
{
  const int anIndex = myStructs.FindIndex(theStruct);

  if (anIndex != 0)
  {
    myStructs.Swap(Size(), anIndex);
    myStructs.RemoveLast();
    MarkDirty();

    return true;
  }

  return false;
}

//=================================================================================================

void Graphic3d_BvhCStructureSetTrsfPers::Clear()
{
  myStructs.Clear();
  MarkDirty();
}

//=================================================================================================

const Graphic3d_CStructure* Graphic3d_BvhCStructureSetTrsfPers::GetStructureById(int theId)
{
  return myStructs.FindKey(theId + 1);
}

//=================================================================================================

const opencascade::handle<BVH_Tree<double, 3>>& Graphic3d_BvhCStructureSetTrsfPers::BVH(
  const occ::handle<Graphic3d_Camera>& theCamera,
  const NCollection_Mat4<double>&      theProjectionMatrix,
  const NCollection_Mat4<double>&      theWorldViewMatrix,
  const int                            theViewportWidth,
  const int                            theViewportHeight,
  const Graphic3d_WorldViewProjState&  theWVPState)
{
  if (!myIsDirty && (myStructBoxesState.IsValid() && !myStructBoxesState.IsChanged(theWVPState)))
  {
    return myBVH;
  }

  myStructBoxes.ReSize(myStructs.Size());

  for (int aStructIdx = 1; aStructIdx <= myStructs.Size(); ++aStructIdx)
  {
    const Graphic3d_CStructure* aStructure = myStructs(aStructIdx);

    occ::handle<HBndBox3d> aBoundingBox = new HBndBox3d();
    *aBoundingBox                       = aStructure->BoundingBox();
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

  myBuilder->Build(this, myBVH.operator->(), BVH_Set<double, 3>::Box());

  myStructBoxesState = theWVPState;
  myStructBoxes.Clear();
  myIsDirty = false;

  return myBVH;
}

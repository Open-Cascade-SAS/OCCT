// Created on: 2013-12-25
// Created by: Varvara POSKONINA
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

#include <Graphic3d_BvhCStructureSet.hxx>

#include <BVH_BinnedBuilder.hxx>
#include <Graphic3d_CStructure.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_BvhCStructureSet, BVH_PrimitiveSet3d)

//=================================================================================================

Graphic3d_BvhCStructureSet::Graphic3d_BvhCStructureSet()
{
  myBuilder =
    new BVH_BinnedBuilder<double, 3>(BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
}

//=================================================================================================

int Graphic3d_BvhCStructureSet::Size() const
{
  return myStructs.Size();
}

//=================================================================================================

Graphic3d_BndBox3d Graphic3d_BvhCStructureSet::Box(const int theIdx) const
{
  return myStructs.FindKey(theIdx + 1)->BoundingBox();
}

//=================================================================================================

double Graphic3d_BvhCStructureSet::Center(const int theIdx, const int theAxis) const
{
  Graphic3d_BndBox3d aBndBox = myStructs.FindKey(theIdx + 1)->BoundingBox();

  const double aMin    = aBndBox.CornerMin()[theAxis];
  const double aMax    = aBndBox.CornerMax()[theAxis];
  const double aCenter = (aMin + aMax) * 0.5;
  return aCenter;
}

//=================================================================================================

void Graphic3d_BvhCStructureSet::Swap(const int theIdx1, const int theIdx2)
{
  myStructs.Swap(theIdx1 + 1, theIdx2 + 1);
}

//=================================================================================================

bool Graphic3d_BvhCStructureSet::Add(const Graphic3d_CStructure* theStruct)
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

bool Graphic3d_BvhCStructureSet::Remove(const Graphic3d_CStructure* theStruct)
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

void Graphic3d_BvhCStructureSet::Clear()
{
  myStructs.Clear();
  MarkDirty();
}

//=================================================================================================

const Graphic3d_CStructure* Graphic3d_BvhCStructureSet::GetStructureById(int theId)
{
  return myStructs.FindKey(theId + 1);
}

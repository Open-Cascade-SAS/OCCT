// Created on: 2014-05-30
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <BVH_LinearBuilder.hxx>

#include <Select3D_SensitiveSet.hxx>
#include <Select3D_BVHPrimitiveContent.hxx>

//=======================================================================
// function : Select3D_BVHPrimitiveContent
// purpose  : Initializes new linear BVH builder for the set of sensitives
//            theSensitiveSet given
//=======================================================================
Select3D_BVHPrimitiveContent::Select3D_BVHPrimitiveContent (const Handle(Select3D_SensitiveSet)& theSensitiveSet)
{
  mySensitiveSet = theSensitiveSet;
  myBuilder = new BVH_LinearBuilder<Standard_Real, 3> (myLeafNodeSize, 32);

  MarkDirty();
}

//=======================================================================
// function : Size
// purpose  : Returns the length of set of sensitives
//=======================================================================
Standard_Integer Select3D_BVHPrimitiveContent::Size() const
{
  return mySensitiveSet->Size();
}

//=======================================================================
// function : Box
// purpose  : Returns bounding box of sensitive with index theIdx
//=======================================================================
Select3D_BndBox3d Select3D_BVHPrimitiveContent::Box (const Standard_Integer theIdx) const
{
  return mySensitiveSet->Box (theIdx);
}

//=======================================================================
// function : Center
// purpose  : Returns center of sensitive with index theIdx in the set
//            along the given axis theAxis
//=======================================================================
Standard_Real Select3D_BVHPrimitiveContent::Center (const Standard_Integer theIdx,
                                                    const Standard_Integer theAxis) const
{
  return mySensitiveSet->Center (theIdx, theAxis);
}

//=======================================================================
// function : Swap
// purpose  : Swaps items with indexes theIdx1 and theIdx2 in the set
//=======================================================================
void Select3D_BVHPrimitiveContent::Swap (const Standard_Integer theIdx1,
                                         const Standard_Integer theIdx2)
{
  mySensitiveSet->Swap (theIdx1, theIdx2);
}

//=======================================================================
// function : GetBVH
// purpose  : Returns the tree built for set of sensitives
//=======================================================================
const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& Select3D_BVHPrimitiveContent::GetBVH()
{
  return BVH();
}

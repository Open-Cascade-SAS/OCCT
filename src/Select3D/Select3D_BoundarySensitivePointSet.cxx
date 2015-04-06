// Created on: 2014-11-10
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

#include <Select3D_BoundarySensitivePointSet.hxx>

//=======================================================================
// function : Select3D_BoundarySensitivePointSet
// purpose  : Creates new instance of Select3D_SensitivePoly with BVH tree
//            required and initializes it with the given array of points
//=======================================================================
Select3D_BoundarySensitivePointSet::Select3D_BoundarySensitivePointSet (const Handle(SelectBasics_EntityOwner)& OwnerId,
                                                                        const TColgp_Array1OfPnt& ThePoints)
  : Select3D_SensitivePoly (OwnerId, ThePoints, Standard_True)
{}

//=======================================================================
// function : Select3D_BoundarySensitivePointSet
// purpose  : Creates new instance of Select3D_SensitivePoly with BVH tree
//            required and initializes it with the given array of points
//=======================================================================
Select3D_BoundarySensitivePointSet::Select3D_BoundarySensitivePointSet (const Handle(SelectBasics_EntityOwner)& OwnerId,
                                                                        const Handle(TColgp_HArray1OfPnt)& ThePoints)
  : Select3D_SensitivePoly (OwnerId, ThePoints, Standard_True)
{}

//=======================================================================
// function : Matches
// purpose  : Checks whether the point set overlaps current selecting
//            volume
//=======================================================================
Standard_Boolean Select3D_BoundarySensitivePointSet::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                              SelectBasics_PickResult& thePickResult)
{
  return Select3D_SensitivePoly::Matches (theMgr, thePickResult);
}

//=======================================================================
// function : GetPoints
// purpose  : Initializes the given array theHArrayOfPnt by 3d
//            coordinates of vertices of the point set
//=======================================================================
void Select3D_BoundarySensitivePointSet::GetPoints (Handle(TColgp_HArray1OfPnt)& theHArrayOfPnt)
{
  Points3D (theHArrayOfPnt);
}

//=======================================================================
// function : BoundingBox
// purpose  : Returns bounding box of the point set. If location transformation
//            is set, it will be applied
//=======================================================================
Select3D_BndBox3d Select3D_BoundarySensitivePointSet::BoundingBox()
{
  return Select3D_SensitivePoly::BoundingBox();
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : Returns center of the point set. If location transformation
//            is set, it will be applied
//=======================================================================
gp_Pnt Select3D_BoundarySensitivePointSet::CenterOfGeometry() const
{
  return Select3D_SensitivePoly::CenterOfGeometry();
}

//=======================================================================
// function : BVH
// purpose  : Builds BVH tree for the point set
//=======================================================================
void Select3D_BoundarySensitivePointSet::BVH()
{
  Select3D_SensitivePoly::BVH();
}

//=======================================================================
// function : NbSubElements
// purpose  : Returns the amount of points in set
//=======================================================================
Standard_Integer Select3D_BoundarySensitivePointSet::NbSubElements()
{
  return Select3D_SensitivePoly::NbSubElements();
}

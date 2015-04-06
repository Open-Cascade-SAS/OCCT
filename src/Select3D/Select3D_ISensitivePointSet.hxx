// Created on: 2014-08-15
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

#ifndef _Select3D_ISensitivePointSet_HeaderFile
#define _Select3D_ISensitivePointSet_HeaderFile

#include <Standard.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
#include <Standard_Real.hxx>

#include <Select3D_BndBox3d.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectBasics_PickResult.hxx>
#include <SelectBasics_SelectingVolumeManager.hxx>

#include <Handle_SelectBasics_EntityOwner.hxx>
#include <Handle_TColgp_HArray1OfPnt.hxx>

class gp_Pnt;
class Standard_ConstructionError;
class Standard_OutOfRange;
class TColgp_Array1OfPnt;
class TColgp_HArray1OfPnt;
class TColgp_Array1OfPnt2d;
class TopLoc_Location;

//! Interface class to unify the work with both internal and boundary
//! sensitive sets of points.
class Select3D_ISensitivePointSet
{
public:

  //! Checks whether the point set overlaps current selecting volume
  virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                    SelectBasics_PickResult& thePickResult) = 0;

  //! Initializes the given array theHArrayOfPnt by 3d coordinates
  //! of vertices of the point set
  virtual void GetPoints (Handle(TColgp_HArray1OfPnt)& theHArrayOfPnt) = 0;

  //! Returns bounding box of the point set. If location
  //! transformation is set, it will be applied
  virtual Select3D_BndBox3d BoundingBox() = 0;

  //! Returns center of the point set. If location
  //! transformation is set, it will be applied
  virtual gp_Pnt CenterOfGeometry() const = 0;

  //! Builds BVH tree for the point set
  virtual void BVH() = 0;

  //! Returns the amount of points in set
  virtual Standard_Integer NbSubElements() = 0;
};

#endif // _Select3D_ISensitivePointSet_HeaderFile

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

#ifndef _Select3D_BoundarySensitivePointSet_HeaderFile
#define _Select3D_BoundarySensitivePointSet_HeaderFile

#include <Handle_SelectBasics_EntityOwner.hxx>
#include <Handle_TColgp_HArray1OfPnt.hxx>

#include <Select3D_ISensitivePointSet.hxx>
#include <Select3D_SensitivePoly.hxx>

class SelectBasics_EntityOwner;
class TColgp_Array1OfPnt;
class TColgp_HArray1OfPnt;

//! This class handles the selection of arbitrary point set with boundary type of sensitivity.
class Select3D_BoundarySensitivePointSet : public Select3D_ISensitivePointSet, public Select3D_SensitivePoly
{
public:

  //! Creates new instance of Select3D_SensitivePoly with BVH tree
  //! required and initializes it with the given array of points
  Standard_EXPORT Select3D_BoundarySensitivePointSet (const Handle(SelectBasics_EntityOwner)& OwnerId,
                                                      const TColgp_Array1OfPnt& ThePoints);

  //! Creates new instance of Select3D_SensitivePoly with BVH tree
  //! required and initializes it with the given array of points
  Standard_EXPORT Select3D_BoundarySensitivePointSet (const Handle(SelectBasics_EntityOwner)& OwnerId,
                                                      const Handle(TColgp_HArray1OfPnt)& ThePoints);

  //! Checks whether the point set overlaps current selecting volume
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Initializes the given array theHArrayOfPnt by 3d coordinates
  //! of vertices of the point set
  Standard_EXPORT virtual void GetPoints (Handle(TColgp_HArray1OfPnt)& theHArrayOfPnt) Standard_OVERRIDE;

  //! Returns bounding box of the point set. If there is a
  //! location transformation set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of the point set. If there is a
  //! location transformation set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Builds BVH tree for the point set
  Standard_EXPORT virtual void BVH() Standard_OVERRIDE;

  //! Returns the amount of points in set
  Standard_EXPORT virtual Standard_Integer NbSubElements() Standard_OVERRIDE;
};

#endif // _Select3D_BoundarySensitivePointSet_HeaderFile

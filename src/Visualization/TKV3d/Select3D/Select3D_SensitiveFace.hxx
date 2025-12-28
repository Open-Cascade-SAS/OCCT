// Created on: 1995-03-27
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Select3D_SensitiveFace_HeaderFile
#define _Select3D_SensitiveFace_HeaderFile

#include <Select3D_TypeOfSensitivity.hxx>
#include <Select3D_SensitiveSet.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <SelectBasics_SelectingVolumeManager.hxx>

//! Sensitive Entity to make a face selectable.
//! In some cases this class can raise Standard_ConstructionError and
//! Standard_OutOfRange exceptions. For more details see Select3D_SensitivePoly.
class Select3D_SensitiveFace : public Select3D_SensitiveEntity
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveFace, Select3D_SensitiveEntity)
public:
  //! Constructs a sensitive face object defined by the
  //! owner theOwnerId, the array of points thePoints, and
  //! the sensitivity type theType.
  //! The array of points is the outer polygon of the geometric face.
  Standard_EXPORT Select3D_SensitiveFace(const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
                                         const NCollection_Array1<gp_Pnt>&            thePoints,
                                         const Select3D_TypeOfSensitivity     theType);

  //! Constructs a sensitive face object defined by the
  //! owner theOwnerId, the array of points thePoints, and
  //! the sensitivity type theType.
  //! The array of points is the outer polygon of the geometric face.
  Standard_EXPORT Select3D_SensitiveFace(const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
                                         const occ::handle<NCollection_HArray1<gp_Pnt>>&   thePoints,
                                         const Select3D_TypeOfSensitivity     theType);

  //! Initializes the given array theHArrayOfPnt by 3d
  //! coordinates of vertices of the face
  Standard_EXPORT void GetPoints(occ::handle<NCollection_HArray1<gp_Pnt>>& theHArrayOfPnt);

  //! Checks whether the face overlaps current selecting volume
  Standard_EXPORT virtual bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                   SelectBasics_PickResult& thePickResult)
    override;

  Standard_EXPORT virtual occ::handle<Select3D_SensitiveEntity> GetConnected() override;

  //! Returns bounding box of the face. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() override;

  //! Returns center of the face. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const override;

  //! Builds BVH tree for the face
  Standard_EXPORT virtual void BVH() override;

  //! Returns TRUE if BVH tree is in invalidated state
  virtual bool ToBuildBVH() const override
  {
    return myFacePoints->ToBuildBVH();
  }

  //! Returns the amount of sub-entities (points or planar convex polygons)
  Standard_EXPORT virtual int NbSubElements() const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int  theDepth = -1) const override;

private:
  Select3D_TypeOfSensitivity mySensType; //!< Type of sensitivity: interior or boundary
  // clang-format off
  occ::handle<Select3D_SensitiveSet> myFacePoints;     //!< Wrapper for overlap detection created depending on sensitivity type
  // clang-format on
};

#endif // _Select3D_SensitiveFace_HeaderFile

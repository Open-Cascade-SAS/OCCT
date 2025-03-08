// Created on: 1995-03-10
// Created by: Mister rmi
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

#ifndef _Select3D_SensitivePoint_HeaderFile
#define _Select3D_SensitivePoint_HeaderFile

#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>

//! A framework to define sensitive 3D points.
class Select3D_SensitivePoint : public Select3D_SensitiveEntity
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitivePoint, Select3D_SensitiveEntity)
public:
  //! Constructs a sensitive point object defined by the
  //! owner OwnerId and the point Point.
  Standard_EXPORT Select3D_SensitivePoint(const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                          const gp_Pnt&                        thePoint);

  //! Returns the amount of sub-entities in sensitive
  Standard_EXPORT virtual Standard_Integer NbSubElements() const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Checks whether the point overlaps current selecting volume
  Standard_EXPORT virtual Standard_Boolean Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                   SelectBasics_PickResult& thePickResult)
    Standard_OVERRIDE;

  //! Returns the point used at the time of construction.
  const gp_Pnt& Point() const { return myPoint; }

  //! Returns center of point. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns bounding box of the point. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns TRUE if BVH tree is in invalidated state
  virtual Standard_Boolean ToBuildBVH() const Standard_OVERRIDE { return Standard_False; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

private:
  gp_Pnt myPoint; //!< 3d coordinates of the point
};

DEFINE_STANDARD_HANDLE(Select3D_SensitivePoint, Select3D_SensitiveEntity)

#endif // _Select3D_SensitivePoint_HeaderFile

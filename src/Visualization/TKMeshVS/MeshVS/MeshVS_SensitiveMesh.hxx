// Created on: 2007-01-29
// Created by: Sergey KOCHETKOV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef _MeshVS_SensitiveMesh_HeaderFile
#define _MeshVS_SensitiveMesh_HeaderFile

#include <Standard.hxx>

#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_BndBox3d.hxx>

//! This class provides custom mesh sensitive entity used in advanced mesh selection.
class MeshVS_SensitiveMesh : public Select3D_SensitiveEntity
{
public:
  Standard_EXPORT MeshVS_SensitiveMesh(const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                       const int                                 theMode = 0);

  Standard_EXPORT int GetMode() const;

  Standard_EXPORT virtual occ::handle<Select3D_SensitiveEntity> GetConnected() override;

  //! Checks whether sensitive overlaps current selecting volume.
  virtual bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
                       SelectBasics_PickResult&             thePickResult) override
  {
    (void)theMgr;
    (void)thePickResult;
    return false;
  }

  //! Returns the amount of mesh nodes
  Standard_EXPORT virtual int NbSubElements() const override;

  //! Returns bounding box of mesh
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() override;

  //! Returns center of mesh
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const override;

  DEFINE_STANDARD_RTTIEXT(MeshVS_SensitiveMesh, Select3D_SensitiveEntity)

private:
  int               myMode;
  Select3D_BndBox3d myBndBox;
};

#endif // _MeshVS_SensitiveMesh_HeaderFile

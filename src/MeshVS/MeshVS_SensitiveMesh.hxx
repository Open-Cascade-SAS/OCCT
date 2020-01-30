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
#include <Standard_Type.hxx>

#include <Bnd_Box.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_BndBox3d.hxx>

class Select3D_SensitiveEntity;

//! This class provides custom mesh sensitive entity used in advanced mesh selection.
class MeshVS_SensitiveMesh : public Select3D_SensitiveEntity
{
public:
  
  Standard_EXPORT MeshVS_SensitiveMesh (const Handle(SelectMgr_EntityOwner)& theOwner,
                                        const Standard_Integer theMode = 0);
  
  Standard_EXPORT Standard_Integer GetMode() const;
  
  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Checks whether sensitive overlaps current selecting volume.
  virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE
  {
    (void )theMgr;
    (void )thePickResult;
    return Standard_False;
  }

  //! Returns the amount of mesh nodes
  Standard_EXPORT virtual Standard_Integer NbSubElements() const Standard_OVERRIDE;

  //! Returns bounding box of mesh
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of mesh
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(MeshVS_SensitiveMesh,Select3D_SensitiveEntity)

private:

  Standard_Integer myMode;
  Select3D_BndBox3d myBndBox;
};

DEFINE_STANDARD_HANDLE(MeshVS_SensitiveMesh, Select3D_SensitiveEntity)

#endif // _MeshVS_SensitiveMesh_HeaderFile

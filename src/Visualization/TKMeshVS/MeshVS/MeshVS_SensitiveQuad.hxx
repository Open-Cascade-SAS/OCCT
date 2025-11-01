// Created on: 2016-03-02
// Created by: Varvara POSKONINA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _MeshVS_SensitiveQuad_HeaderFile
#define _MeshVS_SensitiveQuad_HeaderFile

#include <SelectMgr_EntityOwner.hxx>

#include <TColgp_Array1OfPnt.hxx>

//! This class contains description of planar quadrangle and defines methods
//! for its detection by OCCT BVH selection mechanism
class MeshVS_SensitiveQuad : public Select3D_SensitiveEntity
{
public:
  //! Creates a new instance and initializes quadrangle vertices with the given points
  Standard_EXPORT MeshVS_SensitiveQuad(const Handle(SelectMgr_EntityOwner)& theOwner,
                                       const TColgp_Array1OfPnt&            theQuadVerts);

  //! Creates a new instance and initializes quadrangle vertices with the given points
  Standard_EXPORT MeshVS_SensitiveQuad(const Handle(SelectMgr_EntityOwner)& theOwner,
                                       const gp_Pnt&                        thePnt1,
                                       const gp_Pnt&                        thePnt2,
                                       const gp_Pnt&                        thePnt3,
                                       const gp_Pnt&                        thePnt4);

  //! Returns the amount of sub-entities in sensitive
  virtual Standard_Integer NbSubElements() const Standard_OVERRIDE { return 1; };

  //! Returns a copy of this sensitive quadrangle
  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Checks whether the box overlaps current selecting volume
  Standard_EXPORT virtual Standard_Boolean Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                   SelectBasics_PickResult& thePickResult)
    Standard_OVERRIDE;

  //! Returns center of the box
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns coordinates of the box
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(MeshVS_SensitiveQuad, Select3D_SensitiveEntity)

private:
  gp_Pnt myVertices[4]; //!< 3d coordinates of quad's corners
};

DEFINE_STANDARD_HANDLE(MeshVS_SensitiveQuad, Select3D_SensitiveEntity)

#endif // _MeshVS_SensitiveQuad_HeaderFile

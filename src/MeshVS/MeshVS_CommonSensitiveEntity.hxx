// Created on: 2016-02-18
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

#ifndef _MeshVS_CommonSensitiveEntity_Header
#define _MeshVS_CommonSensitiveEntity_Header

#include <MeshVS_DataSource.hxx>
#include <MeshVS_Mesh.hxx>
#include <MeshVS_MeshSelectionMethod.hxx>
#include <Select3D_SensitiveSet.hxx>

//! Sensitive entity covering entire mesh for global selection.
class MeshVS_CommonSensitiveEntity : public Select3D_SensitiveSet
{
  DEFINE_STANDARD_RTTIEXT (MeshVS_CommonSensitiveEntity, Select3D_SensitiveSet)
public:

  //! Default constructor.
  Standard_EXPORT MeshVS_CommonSensitiveEntity (const Handle(SelectMgr_EntityOwner)& theOwner,
                                                const Handle(MeshVS_Mesh)& theParentMesh,
                                                const MeshVS_MeshSelectionMethod theSelMethod);

  //! Destructor.
  Standard_EXPORT virtual ~MeshVS_CommonSensitiveEntity();

  //! Number of elements.
  Standard_EXPORT virtual Standard_Integer NbSubElements() const Standard_OVERRIDE;

  //! Returns the amount of sub-entities of the complex entity
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns bounding box of sub-entity with index theIdx in sub-entity list
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIdx) const Standard_OVERRIDE;

  //! Returns geometry center of sensitive entity index theIdx along the given axis theAxis
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIdx,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2
  Standard_EXPORT virtual void Swap (const Standard_Integer theIdx1,
                                     const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Returns bounding box of the triangulation. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of a mesh
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Create a copy.
  virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE { return new MeshVS_CommonSensitiveEntity (*this); }

protected:

  //! Checks whether the entity with index theIdx overlaps the current selecting volume
  Standard_EXPORT virtual Standard_Boolean overlapsElement (SelectBasics_PickResult& thePickResult,
                                                            SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Boolean theIsFullInside) Standard_OVERRIDE;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  Standard_EXPORT virtual Standard_Boolean elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Boolean theIsFullInside) Standard_OVERRIDE;

  //! Calculates distance from the 3d projection of used-picked screen point to center of the geometry
  Standard_EXPORT virtual Standard_Real distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr) Standard_OVERRIDE;

  //! Protected copy constructor.
  Standard_EXPORT MeshVS_CommonSensitiveEntity (const MeshVS_CommonSensitiveEntity& theOther);

private:

  //! Return point for specified index.
  gp_Pnt getVertexByIndex (const Standard_Integer theNodeIdx) const;

private:

  Handle(MeshVS_DataSource)            myDataSource;   //!< mesh data source
  NCollection_Vector<Standard_Integer> myItemIndexes;  //!< indices for BVH tree reordering
  MeshVS_MeshSelectionMethod           mySelMethod;    //!< selection mode
  Standard_Integer                     myMaxFaceNodes; //!< maximum nodes within the element in mesh
  gp_Pnt                               myCOG;          //!< center of gravity
  Select3D_BndBox3d                    myBndBox;       //!< bounding box

};

DEFINE_STANDARD_HANDLE (MeshVS_CommonSensitiveEntity, Select3D_SensitiveSet)

#endif // _MeshVS_CommonSensitiveEntity_Header

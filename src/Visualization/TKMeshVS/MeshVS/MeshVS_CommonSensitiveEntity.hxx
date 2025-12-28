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
  DEFINE_STANDARD_RTTIEXT(MeshVS_CommonSensitiveEntity, Select3D_SensitiveSet)
public:
  //! Default constructor.
  Standard_EXPORT MeshVS_CommonSensitiveEntity(const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                               const occ::handle<MeshVS_Mesh>&           theParentMesh,
                                               const MeshVS_MeshSelectionMethod     theSelMethod);

  //! Destructor.
  Standard_EXPORT virtual ~MeshVS_CommonSensitiveEntity();

  //! Number of elements.
  Standard_EXPORT virtual int NbSubElements() const override;

  //! Returns the amount of sub-entities of the complex entity
  Standard_EXPORT virtual int Size() const override;

  //! Returns bounding box of sub-entity with index theIdx in sub-entity list
  Standard_EXPORT virtual Select3D_BndBox3d Box(const int theIdx) const
    override;

  //! Returns geometry center of sensitive entity index theIdx along the given axis theAxis
  Standard_EXPORT virtual double Center(const int theIdx,
                                               const int theAxis) const
    override;

  //! Swaps items with indexes theIdx1 and theIdx2
  Standard_EXPORT virtual void Swap(const int theIdx1,
                                    const int theIdx2) override;

  //! Returns bounding box of the triangulation. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() override;

  //! Returns center of a mesh
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const override;

  //! Create a copy.
  virtual occ::handle<Select3D_SensitiveEntity> GetConnected() override
  {
    return new MeshVS_CommonSensitiveEntity(*this);
  }

protected:
  //! Checks whether the entity with index theIdx overlaps the current selecting volume
  Standard_EXPORT virtual bool overlapsElement(
    SelectBasics_PickResult&             thePickResult,
    SelectBasics_SelectingVolumeManager& theMgr,
    int                     theElemIdx,
    bool                     theIsFullInside) override;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  Standard_EXPORT virtual bool elementIsInside(
    SelectBasics_SelectingVolumeManager& theMgr,
    int                     theElemIdx,
    bool                     theIsFullInside) override;

  //! Calculates distance from the 3d projection of used-picked screen point to center of the
  //! geometry
  Standard_EXPORT virtual double distanceToCOG(SelectBasics_SelectingVolumeManager& theMgr)
    override;

  //! Protected copy constructor.
  Standard_EXPORT MeshVS_CommonSensitiveEntity(const MeshVS_CommonSensitiveEntity& theOther);

private:
  //! Return point for specified index.
  gp_Pnt getVertexByIndex(const int theNodeIdx) const;

private:
  occ::handle<MeshVS_DataSource>            myDataSource;  //!< mesh data source
  NCollection_Vector<int> myItemIndexes; //!< indices for BVH tree reordering
  MeshVS_MeshSelectionMethod           mySelMethod;   //!< selection mode
                                                      // clang-format off
  int                     myMaxFaceNodes; //!< maximum nodes within the element in mesh
                                                      // clang-format on
  gp_Pnt            myCOG;                            //!< center of gravity
  Select3D_BndBox3d myBndBox;                         //!< bounding box
};

#endif // _MeshVS_CommonSensitiveEntity_Header

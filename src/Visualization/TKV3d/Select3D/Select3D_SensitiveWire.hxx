// Created on: 1996-10-17
// Created by: Odile OLIVIER
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Select3D_SensitiveWire_HeaderFile
#define _Select3D_SensitiveWire_HeaderFile

#include <Select3D_SensitiveSet.hxx>

//! A framework to define selection of a wire owner by an
//! elastic wire band.
class Select3D_SensitiveWire : public Select3D_SensitiveSet
{
public:
  //! Constructs a sensitive wire object defined by the
  //! owner theOwnerId
  Standard_EXPORT Select3D_SensitiveWire(const occ::handle<SelectMgr_EntityOwner>& theOwnerId);

  //! Adds the sensitive entity theSensitive to this framework.
  Standard_EXPORT void Add(const occ::handle<Select3D_SensitiveEntity>& theSensitive);

  //! Returns the amount of sub-entities
  Standard_EXPORT virtual int NbSubElements() const override;

  Standard_EXPORT virtual occ::handle<Select3D_SensitiveEntity> GetConnected() override;

  //! returns the sensitive edges stored in this wire
  Standard_EXPORT const NCollection_Vector<occ::handle<Select3D_SensitiveEntity>>& GetEdges();

  //! Sets the owner for all entities in wire
  Standard_EXPORT virtual void Set(const occ::handle<SelectMgr_EntityOwner>& theOwnerId)
    override;

  Standard_EXPORT occ::handle<Select3D_SensitiveEntity> GetLastDetected() const;

  //! Returns bounding box of the wire. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() override;

  //! Returns center of the wire. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const override;

  //! Returns the length of vector of sensitive entities
  Standard_EXPORT virtual int Size() const override;

  //! Returns bounding box of sensitive entity with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box(const int theIdx) const
    override;

  //! Returns geometry center of sensitive entity index theIdx in
  //! the vector along the given axis theAxis
  Standard_EXPORT virtual double Center(const int theIdx,
                                               const int theAxis) const
    override;

  //! Swaps items with indexes theIdx1 and theIdx2 in the vector
  Standard_EXPORT virtual void Swap(const int theIdx1,
                                    const int theIdx2) override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int  theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveWire, Select3D_SensitiveSet)

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

private:
  // clang-format off
  NCollection_Vector<occ::handle<Select3D_SensitiveEntity>> myEntities;          //!< Vector of sub-entities
  NCollection_Vector<int>                 myEntityIndexes;     //!< Indexes of entities for BVH build
  gp_Pnt                                               myCenter;            //!< Center of the whole wire
  mutable Select3D_BndBox3d                            myBndBox;            //!< Bounding box of the whole wire
  // clang-format on
};

#endif // _Select3D_SensitiveWire_HeaderFile

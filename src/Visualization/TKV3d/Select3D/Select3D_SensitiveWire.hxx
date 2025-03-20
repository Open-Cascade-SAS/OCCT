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
  Standard_EXPORT Select3D_SensitiveWire(const Handle(SelectMgr_EntityOwner)& theOwnerId);

  //! Adds the sensitive entity theSensitive to this framework.
  Standard_EXPORT void Add(const Handle(Select3D_SensitiveEntity)& theSensitive);

  //! Returns the amount of sub-entities
  Standard_EXPORT virtual Standard_Integer NbSubElements() const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! returns the sensitive edges stored in this wire
  Standard_EXPORT const NCollection_Vector<Handle(Select3D_SensitiveEntity)>& GetEdges();

  //! Sets the owner for all entities in wire
  Standard_EXPORT virtual void Set(const Handle(SelectMgr_EntityOwner)& theOwnerId)
    Standard_OVERRIDE;

  Standard_EXPORT Handle(Select3D_SensitiveEntity) GetLastDetected() const;

  //! Returns bounding box of the wire. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of the wire. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns the length of vector of sensitive entities
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns bounding box of sensitive entity with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box(const Standard_Integer theIdx) const
    Standard_OVERRIDE;

  //! Returns geometry center of sensitive entity index theIdx in
  //! the vector along the given axis theAxis
  Standard_EXPORT virtual Standard_Real Center(const Standard_Integer theIdx,
                                               const Standard_Integer theAxis) const
    Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2 in the vector
  Standard_EXPORT virtual void Swap(const Standard_Integer theIdx1,
                                    const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveWire, Select3D_SensitiveSet)

protected:
  //! Checks whether the entity with index theIdx overlaps the current selecting volume
  Standard_EXPORT virtual Standard_Boolean overlapsElement(
    SelectBasics_PickResult&             thePickResult,
    SelectBasics_SelectingVolumeManager& theMgr,
    Standard_Integer                     theElemIdx,
    Standard_Boolean                     theIsFullInside) Standard_OVERRIDE;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  Standard_EXPORT virtual Standard_Boolean elementIsInside(
    SelectBasics_SelectingVolumeManager& theMgr,
    Standard_Integer                     theElemIdx,
    Standard_Boolean                     theIsFullInside) Standard_OVERRIDE;

  //! Calculates distance from the 3d projection of used-picked screen point to center of the
  //! geometry
  Standard_EXPORT virtual Standard_Real distanceToCOG(SelectBasics_SelectingVolumeManager& theMgr)
    Standard_OVERRIDE;

private:
  // clang-format off
  NCollection_Vector<Handle(Select3D_SensitiveEntity)> myEntities;          //!< Vector of sub-entities
  NCollection_Vector<Standard_Integer>                 myEntityIndexes;     //!< Indexes of entities for BVH build
  gp_Pnt                                               myCenter;            //!< Center of the whole wire
  mutable Select3D_BndBox3d                            myBndBox;            //!< Bounding box of the whole wire
  // clang-format on
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveWire, Select3D_SensitiveEntity)

#endif // _Select3D_SensitiveWire_HeaderFile

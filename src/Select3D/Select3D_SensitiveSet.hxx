// Created on: 2014-05-29
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

#ifndef _Select3D_SensitiveSet_Header
#define _Select3D_SensitiveSet_Header

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Type.hxx>

#include <NCollection_Handle.hxx>

#include <Select3D_BndBox3d.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <Select3D_SensitiveEntity.hxx>

class Select3D_BVHPrimitiveContent;

//! This class is base class for handling overlap detection of complex sensitive
//! entities. It provides an interface for building BVH tree for some set of entities.
//! Thereby, each iteration of overlap detection is a traverse of BVH tree in fact.
//! To use speed-up hierarchical structure in a custom complex sensitive entity, it is
//! necessary to make that custom entity a descendant of this class and organize sub-entities
//! in some container which allows referencing to elements by index. Note that methods taking
//! index as a parameter are used for BVH build and the range of given index is [0; Size() - 1].
//! For example of usage see Select3D_SensitiveTriangulation.
class Select3D_SensitiveSet : public Select3D_SensitiveEntity
{
public:

  //! Creates new empty sensitive set and its content
  Standard_EXPORT Select3D_SensitiveSet (const Handle(SelectBasics_EntityOwner)& theOwnerId);

  Standard_EXPORT ~Select3D_SensitiveSet() {};

public:

  //! Returns the amount of sub-entities of the complex entity
  virtual Standard_Integer Size() const = 0;

  //! Returns bounding box of sub-entity with index theIdx in sub-entity list
  virtual Select3D_BndBox3d Box (const Standard_Integer theIdx) const = 0;

  //! Returns geometry center of sensitive entity index theIdx along the given axis theAxis
  virtual Standard_Real Center (const Standard_Integer theIdx,
                                const Standard_Integer theAxis) const = 0;

  //! Swaps items with indexes theIdx1 and theIdx2
  virtual void Swap (const Standard_Integer theIdx1,
                     const Standard_Integer theIdx2) = 0;

  //! Checks whether one or more entities of the set overlap current selecting volume.
  //! Implements the traverse of BVH tree built for the set
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Builds BVH tree for sensitive set.
  //! Must be called manually to build BVH tree for any sensitive set
  //! in case if its content was initialized not in a constructor,
  //! but element by element
  Standard_EXPORT void BVH();

  //! Marks BVH tree of the set as outdated. It will be rebuild
  //! at the next call of BVH()
  Standard_EXPORT void MarkDirty();

  //! Returns bounding box of the whole set.
  //! This method should be redefined in Select3D_SensitiveSet descendants
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of the whole set.
  //! This method should be redefined in Select3D_SensitiveSet descendants
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Destroys cross-reference to avoid memory leak
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Returns a number of nodes in 1 BVH leaf
  Standard_EXPORT Standard_Integer GetLeafNodeSize() const;

public:
  DEFINE_STANDARD_RTTI(Select3D_SensitiveSet)

protected:

  //! Checks whether the entity with index theIdx overlaps the current selecting volume
  virtual Standard_Boolean overlapsElement (SelectBasics_SelectingVolumeManager& theMgr,
                                            Standard_Integer theElemIdx,
                                            Standard_Real& theMatchDepth) = 0;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  virtual Standard_Boolean elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                            const Standard_Integer               theElemIdx) = 0;

  //! Calculates distance from the 3d projection of used-picked screen point to center of the geometry
  virtual Standard_Real distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr) = 0;

protected:
  Standard_Integer myDetectedIdx;     //!< Index of detected primitive in BVH sorted primitive array, for debug purposes

private:

  Standard_Boolean                                 myIsLeftChildQueuedFirst;     //!< Flag for slight randomization of BVH traverse
  NCollection_Handle<Select3D_BVHPrimitiveContent> myContent;                    //!< A link between sensitive entity and BVH_PrimitiveSet
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveSet, Select3D_SensitiveEntity)

#endif // _Select3D_SensitiveSet_Header

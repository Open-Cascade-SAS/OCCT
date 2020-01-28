// Created on: 1998-04-16
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Select3D_SensitiveGroup_HeaderFile
#define _Select3D_SensitiveGroup_HeaderFile

#include <Select3D_EntitySequence.hxx>
#include <Select3D_IndexedMapOfEntity.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Select3D_SensitiveSet.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>

//! A framework to define selection of a sensitive group
//!          by a sensitive entity which is a set of 3D sensitive entities.
//!          Remark: 2 modes are possible for rectangle selection
//!          the group is considered selected
//!          1) when all the entities inside are selected in the rectangle
//!          2) only one entity inside is selected by the rectangle
//!          By default the "Match All entities" mode is set.
class Select3D_SensitiveGroup : public Select3D_SensitiveSet
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveGroup, Select3D_SensitiveSet)
public:

  //! Constructs an empty sensitive group object.
  //! This is a set of sensitive 3D entities. The sensitive
  //! entities will be defined using the function Add to fill
  //! the entity owner OwnerId. If MatchAll is false, nothing can be added.
  Standard_EXPORT Select3D_SensitiveGroup (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                           const Standard_Boolean theIsMustMatchAll = Standard_True);

  //! Constructs a sensitive group object defined by the list
  //! TheList and the entity owner OwnerId. If MatchAll is false, nothing is done.
  Standard_EXPORT Select3D_SensitiveGroup (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                           Select3D_EntitySequence& theEntities,
                                           const Standard_Boolean theIsMustMatchAll = Standard_True);

  //! Gets group content
  const Select3D_IndexedMapOfEntity& Entities() const { return myEntities; }

  //! Access entity by index [1, NbSubElements()].
  const Handle(Select3D_SensitiveEntity)& SubEntity (const Standard_Integer theIndex) const
  {
    return myEntities.FindKey (theIndex);
  }

  //! Return last detected entity.
  Handle(Select3D_SensitiveEntity) LastDetectedEntity() const
  {
    const Standard_Integer anIndex = LastDetectedEntityIndex();
    return anIndex != -1 ? myEntities.FindKey (anIndex) : Handle(Select3D_SensitiveEntity)();
  }

  //! Return index of last detected entity.
  Standard_Integer LastDetectedEntityIndex() const { return myDetectedIdx != -1 ? myBVHPrimIndexes.Value (myDetectedIdx) : -1; }

  //! Adds the list of sensitive entities LL to the empty
  //! sensitive group object created at construction time.
  Standard_EXPORT void Add (Select3D_EntitySequence& theEntities);

  //! Adds the sensitive entity aSensitive to the non-empty
  //! sensitive group object created at construction time.
  Standard_EXPORT void Add (const Handle(Select3D_SensitiveEntity)& theSensitive);

  Standard_EXPORT void Remove (const Handle(Select3D_SensitiveEntity)& theSensitive);

  //! Removes all sensitive entities from the list used at the
  //! time of construction, or added using the function Add.
  Standard_EXPORT void Clear() Standard_OVERRIDE;

  //! Returns true if the sensitive entity aSensitive is in
  //! the list used at the time of construction, or added using the function Add.
  Standard_EXPORT Standard_Boolean IsIn (const Handle(Select3D_SensitiveEntity)& theSensitive) const;

  //! Sets the requirement that all sensitive entities in the
  //! list used at the time of construction, or added using
  //! the function Add must be matched.
  void SetMatchType (const Standard_Boolean theIsMustMatchAll) { myMustMatchAll = theIsMustMatchAll; }

  //! Returns true if all sensitive entities in the list used
  //! at the time of construction, or added using the function Add must be matched.
  Standard_Boolean MustMatchAll() const { return myMustMatchAll; }

  //! Returns TRUE if all sensitive entities should be checked within rectangular/polygonal selection, FALSE by default.
  //! Can be useful for sensitive entities holding detection results as class property.
  Standard_Boolean ToCheckOverlapAll() const
  {
    return myToCheckOverlapAll;
  }

  //! Returns TRUE if all sensitive entities should be checked within rectangular/polygonal selection, FALSE by default.
  //! Can be useful for sensitive entities holding detection results as class property.
  void SetCheckOverlapAll (Standard_Boolean theToCheckAll)
  {
    myToCheckOverlapAll = theToCheckAll;
  }

  //! Checks whether the group overlaps current selecting volume
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Returns the amount of sub-entities
  Standard_EXPORT virtual Standard_Integer NbSubElements() const Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Sets the owner for all entities in group
  Standard_EXPORT void Set (const Handle(SelectMgr_EntityOwner)& theOwnerId) Standard_OVERRIDE;

  //! Returns bounding box of the group. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of entity set. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns bounding box of sensitive entity with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIdx) const Standard_OVERRIDE;

  //! Returns geometry center of sensitive entity index theIdx in
  //! the vector along the given axis theAxis
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIdx,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2 in the vector
  Standard_EXPORT virtual void Swap (const Standard_Integer theIdx1,
                                     const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Returns the length of vector of sensitive entities
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

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

private:

  Select3D_IndexedMapOfEntity          myEntities;           //!< Grouped sensitive entities
  Standard_Boolean                     myMustMatchAll;       //!< Determines whether all entities in the group should be overlapped or not
  Standard_Boolean                     myToCheckOverlapAll;  //!< flag to check overlapping with all entities within rectangular/polygonal selection
  gp_Pnt                               myCenter;             //!< Center of geometry of the group
  mutable Select3D_BndBox3d            myBndBox;             //!< Bounding box of the group
  NCollection_Vector<Standard_Integer> myBVHPrimIndexes;     //!< Vector of sub-entities indexes for BVH tree build
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveGroup, Select3D_SensitiveEntity)

#endif // _Select3D_SensitiveGroup_HeaderFile

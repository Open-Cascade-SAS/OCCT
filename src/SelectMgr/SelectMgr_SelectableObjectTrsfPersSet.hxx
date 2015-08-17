// Created on: 2015-06-30
// Created by: Anton POLETAEV
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _SelectMgr_SelectableObjectTrsfPersSet_HeaderFile
#define _SelectMgr_SelectableObjectTrsfPersSet_HeaderFile

#include <BVH_Builder.hxx>
#include <BVH_Set.hxx>
#include <BVH_Tree.hxx>
#include <Graphic3d_Camera.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Select3D_BndBox3d.hxx>
#include <SelectMgr_SelectableObject.hxx>

//! Primitive set specialized for transformation persistent selectable objects.
//! Provides built-in mechanism to invalidate tree when world view projection state changes.
//! Due to frequent invalidation of BVH tree the choice of BVH tree builder is made
//! in favor of BVH linear builder (quick rebuild).
class SelectMgr_SelectableObjectTrsfPersSet : public BVH_Set<Standard_Real, 3>
{
private:

  typedef NCollection_Handle<Select3D_BndBox3d> HBndBox3d;

  Handle(SelectMgr_SelectableObject) EMPTY_OBJ;

public:

  //! Creates new empty objects set and initializes BVH tree
  //! builder to Linear builder with 1 element per list.
  Standard_EXPORT SelectMgr_SelectableObjectTrsfPersSet();

  //! Returns size of objects set.
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns bounding box of object with index theIndex.
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIndex) const Standard_OVERRIDE;

  //! Returns center of object with index theIndex in the set along the given axis theAxis.
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIndex,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIndex1 and theIndex2 in the set.
  Standard_EXPORT virtual void Swap (const Standard_Integer theIndex1,
                                     const Standard_Integer theIndex2) Standard_OVERRIDE;

  //! Adds new selectable object to the set.
  //! @return true if structure added, otherwise returns false (structure already in the set).
  Standard_EXPORT Standard_Boolean Append (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Removes selectable object from set.
  //! @return true if structure removed, otherwise returns false (structure is not in the set).
  Standard_EXPORT Standard_Boolean Remove (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Returns object from set by theIndex given.
  const Handle(SelectMgr_SelectableObject)& GetObjectById (const Standard_Integer theIndex) const
  {
    return myObjects.FindKey (theIndex + 1);
  }

  //! Marks object state as outdated (needs BVH rebuilding).
  void MarkDirty()
  {
    myIsDirty = Standard_True;
  }

  //! Returns true if this objects set contains theObject given
  Standard_Boolean Contains (const Handle(SelectMgr_SelectableObject)& theObject) const
  {
    return myObjects.Contains (theObject);
  }

  //! Returns BVH tree for the given world view projection (builds it if necessary).
  Standard_EXPORT const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& BVH (const Graphic3d_Mat4d& theProjectionMatrix,
                                                                              const Graphic3d_Mat4d& theWorldViewMatrix,
                                                                              const Graphic3d_WorldViewProjState& theWVPState);

private:

  //! Marks internal object state as outdated.
  Standard_Boolean myIsDirty;

  //! Constructed bottom-level BVH.
  NCollection_Handle<BVH_Tree<Standard_Real, 3> > myBVH;

  //! Builder for bottom-level BVH.
  NCollection_Handle<BVH_Builder<Standard_Real, 3> > myBuilder;

  //! Set of transform persistence objects.
  NCollection_IndexedMap<Handle(SelectMgr_SelectableObject)> myObjects;

  //! Cached set of bounding boxes precomputed for transformation persistent selectable objects.
  //! Cache exists only during computation of BVH Tree. Bounding boxes are world view projection
  //! dependent and should by synchronized.
  NCollection_IndexedMap<HBndBox3d> myObjectBoxes;

  //! State of world view projection used for generation of transformation persistence bounding boxes.
  Graphic3d_WorldViewProjState myObjectBoxesState;
};

#endif

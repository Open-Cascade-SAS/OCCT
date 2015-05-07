// Created on: 2014-08-15
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

#ifndef _SelectMgr_SelectableObjectSet_HeaderFile
#define _SelectMgr_SelectableObjectSet_HeaderFile

#include <BVH_PrimitiveSet.hxx>

#include <Select3D_BndBox3d.hxx>
#include <Handle_SelectMgr_SelectionManager.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_VectorTypes.hxx>

#include <NCollection_IndexedMap.hxx>

//! The purpose of this class is to organize all selectable objects into
//! data structure, allowing to build BVH tree. For selectable objects
//! binned BVH builder is used with 32 bins and 1 element per leaf.
class SelectMgr_SelectableObjectSet : public BVH_PrimitiveSet<Standard_Real, 3>
{
  Handle(SelectMgr_SelectableObject) EMPTY_OBJ;

public:

  //! Creates new empty objects set and initializes BVH tree
  //! builder to Binned builder with 1 element per list
  SelectMgr_SelectableObjectSet();

  //! Releases resources of selectable object set.
  virtual ~SelectMgr_SelectableObjectSet() { }

  //! Adds new object to the set and marks BVH tree for rebuild
  void Append (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Removes object theObject from set and marks BVH tree for rebuild
  void Remove (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Returns bounding box of object with index theIndex
  virtual Select3D_BndBox3d Box (const Standard_Integer theIndex) const Standard_OVERRIDE;

  //! Returns center of object with index theIndex in the set
  //! along the given axis theAxis
  virtual Standard_Real Center (const Standard_Integer theIndex,
                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIndex1 and theIndex2 in the set
  virtual void Swap (const Standard_Integer theIndex1,
                     const Standard_Integer theIndex2) Standard_OVERRIDE;

  //! Returns size of objects set
  virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns object from set by theIndex given
  const Handle(SelectMgr_SelectableObject)& GetObjectById (const Standard_Integer theIndex) const;

  //! Returns true if this objects set contains theObject given
  Standard_Boolean Contains (const Handle(SelectMgr_SelectableObject)& theObject) const;

private:

  NCollection_IndexedMap<Handle(SelectMgr_SelectableObject)> myObjects;
};

#endif // _SelectMgr_SelectableObjectSet_HeaderFile

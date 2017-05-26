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

#include <BVH_Builder.hxx>
#include <BVH_Tree.hxx>
#include <Graphic3d_Mat4d.hxx>
#include <Graphic3d_WorldViewProjState.hxx>
#include <NCollection_Handle.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Select3D_BVHBuilder3d.hxx>
#include <SelectMgr_SelectableObject.hxx>

//! The purpose of this class is to organize all selectable objects into data structure, allowing to build 
//! set of BVH trees for each transformation persistence subclass of selectable objects. This allow to minify
//! number of updates for BVH trees - for example 2D persistent object subclass depends only on camera's projection
//! and the corresponding BVH tree needs to be updated when camera's projection parameters change, while another
//! tree for non-persistent objects can be left unchanged in this case.
class SelectMgr_SelectableObjectSet
{
public:

  //! This enumeration declares names for subsets of selectable objects. Each subset has independent BVH tree.
  //! The class maintains subsets of selectable objects by their persistence flag. This allows to restric
  //! rebuilding of the trees for particular subset when the camera change does not implicitly require it:
  //! - BVHSubset_3d refers to the subset of normal world-space 3D objects. Associated BVH tree does not depend
  //! on the camera's state at all.
  //! This subset uses binned BVH builder with 32 bins and 1 element per leaf.
  //! - BVHSubset_3dPersistent refers to the subset of 3D persistent selectable objects (rotate, pan, zoom persistence).
  //! Associated BVH tree needs to be updated when either the camera's projection and position change.
  //! This subset uses linear BVH builder with 32 levels of depth and 1 element per leaf.
  //! - BVHSubset_2dPersistent refers to the subset of 2D persistent selectable objects. Associated BVH tree
  //! needs to be updated only when camera's projection changes. Bounding volumes for this object subclass
  //! is represented directly in eye space coordinates.
  //! This subset uses linear BVH builder with 32 levels of depth and 1 element per leaf.
  enum BVHSubset
  {
    BVHSubset_3d,
    BVHSubset_3dPersistent,
    BVHSubset_2dPersistent,
    BVHSubsetNb
  };

public:

  //! Class to iterate sequentually over all objects from every subset.
  class Iterator
  {
    //! Short-cut definition of map iterator type
    typedef NCollection_IndexedMap<Handle(SelectMgr_SelectableObject)>::Iterator ObjectMapIterator;

  public:

    //! Default constructor without initialization.
    Iterator() : mySet (NULL), mySubsetIdx (BVHSubsetNb) {}

    //! Constructs and initializes the iterator.
    Iterator (const SelectMgr_SelectableObjectSet& theSet) { Init (theSet); }

    //! Initializes the iterator.
    void Init (const SelectMgr_SelectableObjectSet& theSet)
    {
      mySet       = &theSet;
      mySubsetIdx = 0;
      mySubsetIt  = ObjectMapIterator (theSet.myObjects[mySubsetIdx]);
      More();
    }

    //! Returns false when there is no more objects to iterate over.
    Standard_Boolean More()
    {
      if (mySubsetIt.More())
      {
        return Standard_True;
      }
      else if ((mySubsetIdx == BVHSubsetNb - 1) || mySet == NULL)
      {
        return Standard_False;
      }
      mySubsetIt = ObjectMapIterator (mySet->myObjects[++mySubsetIdx]);
      return More();
    }

    //! Steps to next selectable object in the set.
    void Next() { mySubsetIt.Next(); }

    //! Returns current object.
    const Handle(SelectMgr_SelectableObject)& Value() const { return mySubsetIt.Value(); }

  private:
    const SelectMgr_SelectableObjectSet* mySet;
    Standard_Integer mySubsetIdx;
    ObjectMapIterator mySubsetIt;
  };

public:

  //! Creates new empty objects set and initializes BVH tree builders for each subset.
  Standard_EXPORT SelectMgr_SelectableObjectSet();

  //! Releases resources of selectable object set.
  virtual ~SelectMgr_SelectableObjectSet() { }

  //! Adds the new selectable object to the set. The selectable object is placed into one of the
  //! predefined subsets depending on its persistence type. After adding an object, this method
  //! marks the corresponding BVH tree for rebuild.
  //! @return true if selectable object is added, otherwise returns false (selectable object is already in the set).
  Standard_EXPORT Standard_Boolean Append (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Removes the selectable object from the set. The selectable object is removed from the subset
  //! it has been placed into. After removing an object, this method marks the corresponding
  //! BVH tree for rebuild.
  //! @return true if selectable object is removed, otherwise returns false (selectable object is not in the set).
  Standard_EXPORT Standard_Boolean Remove (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Performs necessary updates when object's persistence types changes.
  //! This method should be called right after changing transformation persistence flags of the
  //! objects and before updating BVH tree - to provide up-to-date state of the object set.
  Standard_EXPORT void ChangeSubset (const Handle(SelectMgr_SelectableObject)& theObject);

  //! Updates outdated BVH trees and remembers the last state of the
  //! camera view-projection matrices and viewport (window) dimensions.
  Standard_EXPORT void UpdateBVH (const Handle(Graphic3d_Camera)& theCamera,
                                  const Graphic3d_Mat4d& theProjectionMat,
                                  const Graphic3d_Mat4d& theWorldViewMat,
                                  const Graphic3d_WorldViewProjState& theViewState,
                                  const Standard_Integer theViewportWidth,
                                  const Standard_Integer theViewportHeight);

  //! Marks every BVH subset for update.
  Standard_EXPORT void MarkDirty();

  //! Returns true if this objects set contains theObject given.
  Standard_Boolean Contains (const Handle(SelectMgr_SelectableObject)& theObject) const
  {
    return myObjects[BVHSubset_3d].Contains (theObject)
        || myObjects[BVHSubset_3dPersistent].Contains (theObject)
        || myObjects[BVHSubset_2dPersistent].Contains (theObject);
  }

  //! Returns true if the object set does not contain any selectable objects.
  Standard_Boolean IsEmpty() const
  {
    return myObjects[BVHSubset_3d].IsEmpty()
        && myObjects[BVHSubset_3dPersistent].IsEmpty()
        && myObjects[BVHSubset_2dPersistent].IsEmpty();
  }

  //! Returns true if the specified object subset is empty.
  Standard_Boolean IsEmpty (const BVHSubset theSubset) const
  {
    return myObjects[theSubset].IsEmpty();
  }

  //! Returns object from subset theSubset by theIndex given. The method allows to get selectable object
  //! referred by the index of an element of the subset's BVH tree.
  const Handle(SelectMgr_SelectableObject)& GetObjectById (const BVHSubset theSubset,
                                                           const Standard_Integer theIndex) const
  {
    return myObjects[theSubset].FindKey (theIndex + 1);
  }

  //! Returns computed BVH for the theSubset given.
  const opencascade::handle<BVH_Tree<Standard_Real, 3> >& BVH(const BVHSubset theSubset) const
  {
    return myBVH[theSubset];
  }

private:

  //! Returns an appropriate subset of theObject given depending on its persistence type.
  Standard_Integer appropriateSubset (const Handle(SelectMgr_SelectableObject)& theObject)
  {
    if (theObject->TransformPersistence().IsNull())
    {
      return SelectMgr_SelectableObjectSet::BVHSubset_3d;
    }
    else if (theObject->TransformPersistence()->Mode() == Graphic3d_TMF_2d)
    {
      return SelectMgr_SelectableObjectSet::BVHSubset_2dPersistent;
    }
    else
    {
      return SelectMgr_SelectableObjectSet::BVHSubset_3dPersistent;
    }
  }

  //! Returns current subset of theObject given.
  Standard_Integer currentSubset (const Handle(SelectMgr_SelectableObject)& theObject)
  {
    for (Standard_Integer aSubsetIdx = 0; aSubsetIdx < BVHSubsetNb; ++aSubsetIdx)
    {
      if (myObjects[aSubsetIdx].Contains (theObject))
      {
        return aSubsetIdx;
      }
    }
    return -1;
  }

private:

  NCollection_IndexedMap<Handle(SelectMgr_SelectableObject)> myObjects[BVHSubsetNb]; //!< Map of objects for each subset
  opencascade::handle<BVH_Tree<Standard_Real, 3> >           myBVH[BVHSubsetNb];     //!< BVH tree computed for each subset
  Handle(Select3D_BVHBuilder3d)                              myBuilder[BVHSubsetNb]; //!< Builder allocated for each subset
  Standard_Boolean                                           myIsDirty[BVHSubsetNb]; //!< Dirty flag for each subset
  Graphic3d_WorldViewProjState                               myLastViewState; //!< Last view-projection state used for construction of BVH
  Standard_Integer                                           myLastWidth; //!< Last viewport's (window's) width used for construction of BVH
  Standard_Integer                                           myLastHeight; //!< Last viewport's (window's) height used for construction of BVH
  friend class Iterator;
};

#endif // _SelectMgr_SelectableObjectSet_HeaderFile

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

#include <SelectMgr_SelectableObjectSet.hxx>
#include <SelectMgr_VectorTypes.hxx>

#include <BVH_BinnedBuilder.hxx>
#include <BVH_LinearBuilder.hxx>

namespace
{
  //! Short-cut definition of indexed data map of selectable objects
  typedef NCollection_IndexedMap<Handle(SelectMgr_SelectableObject)> ObjectsMap;

  //-------------------------------------------------------------------------------------
  // Adaptor over regular objects subset of SelectMgr_SelectableObjectSet for BVH builder
  //-------------------------------------------------------------------------------------

  //! This class provides direct access to fields of SelectMgr_SelectableObjectSet
  //! so the BVH builder could explicitly arrange objects in the map as necessary
  //! to provide synchronization of indexes with constructed BVH tree.
  class BVHBuilderAdaptorRegular : public BVH_Set<Standard_Real, 3>
  {
  public:

    //! Construct adaptor.
    BVHBuilderAdaptorRegular (ObjectsMap& theObjects) : myObjects (theObjects) {};

    //! Returns bounding box of object with index theIndex
    virtual Select3D_BndBox3d Box (const Standard_Integer theIndex) const Standard_OVERRIDE
    {
      const Handle(SelectMgr_SelectableObject)& anObject = myObjects.FindKey (theIndex + 1);
      Bnd_Box aBox;
      anObject->BoundingBox (aBox);
      if (aBox.IsVoid())
        return Select3D_BndBox3d();

      return Select3D_BndBox3d (SelectMgr_Vec3 (aBox.CornerMin().X(), aBox.CornerMin().Y(), aBox.CornerMin().Z()),
                                SelectMgr_Vec3 (aBox.CornerMax().X(), aBox.CornerMax().Y(), aBox.CornerMax().Z()));
    }

    //! Returns bounding box of the whole subset.
    virtual Select3D_BndBox3d Box() const Standard_OVERRIDE
    {
      if (!myBox.IsValid())
      {
        myBox = BVH_Set<Standard_Real, 3>::Box();
      }
      return myBox;
    }

    //! Make inherited method Box() visible to avoid CLang warning
    using BVH_Set<Standard_Real, 3>::Box;

    //! Returns center of object with index theIndex in the set
    //! along the given axis theAxis
    virtual Standard_Real Center (const Standard_Integer theIndex,
                                  const Standard_Integer theAxis) const Standard_OVERRIDE
    {
      const Select3D_BndBox3d aBndBox = Box (theIndex);

      return (aBndBox.CornerMin()[theAxis] +
              aBndBox.CornerMax()[theAxis]) * 0.5;
    }

    //! Returns size of objects set.
    virtual Standard_Integer Size() const Standard_OVERRIDE
    {
      return myObjects.Size();
    }

    //! Swaps items with indexes theIndex1 and theIndex2 in the set
    virtual void Swap (const Standard_Integer theIndex1, const Standard_Integer theIndex2) Standard_OVERRIDE
    {
      myObjects.Swap (theIndex1 + 1, theIndex2 + 1);
    }

  private:
    BVHBuilderAdaptorRegular& operator=(BVHBuilderAdaptorRegular) { return *this; }

  private:
    ObjectsMap& myObjects;
    mutable Select3D_BndBox3d myBox;
  };

  //----------------------------------------------------------------------------------------
  // Adaptor over persistent objects subset of SelectMgr_SelectableObjectSet for BVH builder
  //----------------------------------------------------------------------------------------

  //! This class provides direct access to fields of SelectMgr_SelectableObjectSet
  //! so the BVH builder could explicitly arrange objects in the map as necessary
  //! to provide synchronization of indexes with constructed BVH tree.
  class BVHBuilderAdaptorPersistent : public BVH_Set<Standard_Real, 3>
  {
  public:

    //! Construct adaptor.
    //! @param theCamera, theProjectionMat, theWorldViewMat,
    //!        theWidth, theHeight [in] view properties used for computation of
    //!        bounding boxes within the world view camera space.
    BVHBuilderAdaptorPersistent (ObjectsMap& theObjects,
                                 const Handle(Graphic3d_Camera)& theCamera,
                                 const Graphic3d_Mat4d& theProjectionMat,
                                 const Graphic3d_Mat4d& theWorldViewMat,
                                 const Standard_Integer theWidth,
                                 const Standard_Integer theHeight)
    : myObjects (theObjects)
    {
      myBoundings.ReSize (myObjects.Size());
      for (Standard_Integer anI = 1; anI <= myObjects.Size(); ++anI)
      {
        const Handle(SelectMgr_SelectableObject)& anObject = myObjects (anI);

        Bnd_Box aBoundingBox;
        anObject->BoundingBox (aBoundingBox);
        if (aBoundingBox.IsVoid()
         || anObject->TransformPersistence().IsNull())
        {
          myBoundings.Add (new Select3D_HBndBox3d());
        }
        else
        {
          anObject->TransformPersistence()->Apply (theCamera, theProjectionMat, theWorldViewMat, theWidth, theHeight, aBoundingBox);

          const gp_Pnt aMin = aBoundingBox.CornerMin();
          const gp_Pnt aMax = aBoundingBox.CornerMax();
          myBoundings.Add (new Select3D_HBndBox3d (Select3D_Vec3 (aMin.X(), aMin.Y(), aMin.Z()),
                                                   Select3D_Vec3 (aMax.X(), aMax.Y(), aMax.Z())));
        }
      }
    }

    //! Returns bounding box of object with index theIndex
    virtual Select3D_BndBox3d Box (const Standard_Integer theIndex) const Standard_OVERRIDE
    {
      return *myBoundings (theIndex + 1);
    }

    //! Returns bounding box of the whole subset.
    virtual Select3D_BndBox3d Box() const Standard_OVERRIDE
    {
      if (!myBox.IsValid())
      {
        myBox = BVH_Set<Standard_Real, 3>::Box();
      }
      return myBox;
    }

    //! Make inherited method Box() visible to avoid CLang warning
    using BVH_Set<Standard_Real, 3>::Box;

    //! Returns center of object with index theIndex in the set
    //! along the given axis theAxis
    virtual Standard_Real Center (const Standard_Integer theIndex,
                                  const Standard_Integer theAxis) const Standard_OVERRIDE
    {
      const Select3D_BndBox3d& aBoundingBox = *myBoundings (theIndex + 1);

      return (aBoundingBox.CornerMin()[theAxis] + aBoundingBox.CornerMax()[theAxis]) * 0.5;
    }

    //! Returns size of objects set.
    virtual Standard_Integer Size() const Standard_OVERRIDE
    {
      return myObjects.Size();
    }

    //! Swaps items with indexes theIndex1 and theIndex2 in the set
    virtual void Swap (const Standard_Integer theIndex1, const Standard_Integer theIndex2) Standard_OVERRIDE
    {
      const Standard_Integer aStructIdx1 = theIndex1 + 1;
      const Standard_Integer aStructIdx2 = theIndex2 + 1;

      myObjects.Swap (aStructIdx1, aStructIdx2);
      myBoundings.Swap (aStructIdx1, aStructIdx2);
    }

  private:
    BVHBuilderAdaptorPersistent& operator=(BVHBuilderAdaptorPersistent) { return *this; }

  private:
    ObjectsMap& myObjects;
    mutable Select3D_BndBox3d myBox;
    typedef NCollection_Shared<Select3D_BndBox3d> Select3D_HBndBox3d;
    NCollection_IndexedMap<Handle(Select3D_HBndBox3d)> myBoundings;
  };

  static const Graphic3d_Mat4d SelectMgr_SelectableObjectSet_THE_IDENTITY_MAT;
}

//=============================================================================
// Function: Constructor
// Purpose :
//=============================================================================
SelectMgr_SelectableObjectSet::SelectMgr_SelectableObjectSet()
: myLastWidth (0),
  myLastHeight (0)
{
  myBVH[BVHSubset_2dPersistent] = new BVH_Tree<Standard_Real, 3>();
  myBVH[BVHSubset_3dPersistent] = new BVH_Tree<Standard_Real, 3>();
  myBVH[BVHSubset_3d]           = new BVH_Tree<Standard_Real, 3>();

  myBuilder[BVHSubset_2dPersistent] = new BVH_LinearBuilder<Standard_Real, 3>    (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_3dPersistent] = new BVH_LinearBuilder<Standard_Real, 3>    (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_3d]           = new BVH_BinnedBuilder<Standard_Real, 3, 4> (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth, Standard_True);

  myIsDirty[BVHSubset_2dPersistent] = Standard_False;
  myIsDirty[BVHSubset_3dPersistent] = Standard_False;
  myIsDirty[BVHSubset_3d]           = Standard_False;
}

//=============================================================================
// Function: Append
// Purpose :
//=============================================================================
Standard_Boolean SelectMgr_SelectableObjectSet::Append (const Handle(SelectMgr_SelectableObject)& theObject)
{
  // get an appropriate BVH subset to insert the object into it
  const Standard_Integer aSubsetIdx = appropriateSubset (theObject);

  // check that the object is excluded from other subsets
  if (myObjects[(aSubsetIdx + 1) % BVHSubsetNb].Contains (theObject)
   || myObjects[(aSubsetIdx + 2) % BVHSubsetNb].Contains (theObject))
  {
    return Standard_False;
  }

  // try adding it into the appropriate object subset
  const Standard_Integer aSize = myObjects[aSubsetIdx].Size();

  if (aSize < myObjects[aSubsetIdx].Add (theObject))
  {
    myIsDirty[aSubsetIdx] = Standard_True;

    return Standard_True;
  }

  return Standard_False;
}

//=============================================================================
// Function: Remove
// Purpose :
//=============================================================================
Standard_Boolean SelectMgr_SelectableObjectSet::Remove (const Handle(SelectMgr_SelectableObject)& theObject)
{
  // remove object from the first found subset
  for (Standard_Integer aSubsetIdx = 0; aSubsetIdx < BVHSubsetNb; ++aSubsetIdx)
  {
    const Standard_Integer anIndex = myObjects[aSubsetIdx].FindIndex (theObject);

    if (anIndex != 0)
    {
      const Standard_Integer aSize = myObjects[aSubsetIdx].Size();

      if (anIndex != aSize)
      {
        myObjects[aSubsetIdx].Swap (anIndex, aSize);
      }

      myObjects[aSubsetIdx].RemoveLast();
      myIsDirty[aSubsetIdx] = Standard_True;

      return Standard_True;
    }
  }

  return Standard_False;
}

//=============================================================================
// Function: ChangeSubset
// Purpose :
//=============================================================================
void SelectMgr_SelectableObjectSet::ChangeSubset (const Handle(SelectMgr_SelectableObject)& theObject)
{
  // do not do anything is object is not in the map
  const Standard_Integer aCurrSubsetIdx = currentSubset (theObject);

  if (aCurrSubsetIdx < 0)
  {
    return;
  }

  // check whether the subset need to be changed at all
  const Standard_Integer aSubsetIdx = appropriateSubset (theObject);

  if (aCurrSubsetIdx == aSubsetIdx)
  {
    return;
  }

  // replace object in the maps
  Remove (theObject);
  Append (theObject);
}

//=============================================================================
// Function: UpdateBVH
// Purpose :
//=============================================================================
void SelectMgr_SelectableObjectSet::UpdateBVH (const Handle(Graphic3d_Camera)& theCamera,
                                               const Graphic3d_Mat4d& theProjectionMat,
                                               const Graphic3d_Mat4d& theWorldViewMat,
                                               const Graphic3d_WorldViewProjState& theViewState,
                                               const Standard_Integer theViewportWidth,
                                               const Standard_Integer theViewportHeight)
{
  // -----------------------------------------
  // check and update 3D BVH tree if necessary
  // -----------------------------------------
  if (!IsEmpty (BVHSubset_3d) && myIsDirty[BVHSubset_3d])
  {
    // construct adaptor over private fields to provide direct access for the BVH builder
    BVHBuilderAdaptorRegular anAdaptor (myObjects[BVHSubset_3d]);

    // update corresponding BVH tree data structure
    myBuilder[BVHSubset_3d]->Build (&anAdaptor, myBVH[BVHSubset_3d].get(), anAdaptor.Box());

    // release dirty state
    myIsDirty[BVHSubset_3d] = Standard_False;
  }

  if (!theCamera.IsNull())
  {
    const Standard_Boolean isWindowSizeChanged =
      (myLastHeight != theViewportHeight) || (myLastWidth != theViewportWidth);

    // -----------------------------------------------------
    // check and update 3D persistence BVH tree if necessary
    // -----------------------------------------------------
    if (!IsEmpty (BVHSubset_3dPersistent) &&
         (myIsDirty[BVHSubset_3dPersistent] || myLastViewState.IsChanged (theViewState) || isWindowSizeChanged))
    {
      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor (myObjects[BVHSubset_3dPersistent],
        theCamera, theProjectionMat, theWorldViewMat, theViewportWidth, theViewportHeight);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_3dPersistent]->Build (&anAdaptor, myBVH[BVHSubset_3dPersistent].get(), anAdaptor.Box());
    }

    // -----------------------------------------------------
    // check and update 2D persistence BVH tree if necessary
    // -----------------------------------------------------
    if (!IsEmpty (BVHSubset_2dPersistent) &&
         (myIsDirty[BVHSubset_2dPersistent] || myLastViewState.IsProjectionChanged (theViewState) || isWindowSizeChanged))
    {
      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor (myObjects[BVHSubset_2dPersistent],
        theCamera, theProjectionMat, SelectMgr_SelectableObjectSet_THE_IDENTITY_MAT, theViewportWidth, theViewportHeight);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_2dPersistent]->Build (&anAdaptor, myBVH[BVHSubset_2dPersistent].get(), anAdaptor.Box());
    }

    // release dirty state for every subset
    myIsDirty[BVHSubset_3dPersistent] = Standard_False;
    myIsDirty[BVHSubset_2dPersistent] = Standard_False;

    // keep last view state
    myLastViewState = theViewState;
  }

  // keep last window state
  myLastWidth  = theViewportWidth;
  myLastHeight = theViewportHeight;
}

//=============================================================================
// Function: MarkDirty
// Purpose :
//=============================================================================
void SelectMgr_SelectableObjectSet::MarkDirty()
{
  myIsDirty[BVHSubset_3d]           = Standard_True;
  myIsDirty[BVHSubset_3dPersistent] = Standard_True;
  myIsDirty[BVHSubset_2dPersistent] = Standard_True;
}

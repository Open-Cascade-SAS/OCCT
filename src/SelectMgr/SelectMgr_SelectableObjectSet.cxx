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
    BVHBuilderAdaptorRegular& operator=(const BVHBuilderAdaptorRegular&) { return *this; }

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
                                 const Graphic3d_Vec2i& theWinSize)
    : myObjects (theObjects)
    {
      myBoundings.ReSize (myObjects.Size());
      for (Standard_Integer anI = 1; anI <= myObjects.Size(); ++anI)
      {
        const Handle(SelectMgr_SelectableObject)& anObject = myObjects (anI);

        Bnd_Box aBoundingBox;
        anObject->BoundingBox (aBoundingBox);
        if (!aBoundingBox.IsVoid()
         && !anObject->TransformPersistence().IsNull())
        {
          anObject->TransformPersistence()->Apply (theCamera,
                                                   theProjectionMat, theWorldViewMat,
                                                   theWinSize.x(), theWinSize.y(),
                                                   aBoundingBox);
        }

        // processing presentations with own transform persistence
        for (PrsMgr_Presentations::Iterator aPrsIter (anObject->Presentations()); aPrsIter.More(); aPrsIter.Next())
        {
          const Handle(PrsMgr_Presentation)& aPrs3d = aPrsIter.Value();
          if (!aPrs3d->CStructure()->HasGroupTransformPersistence())
          {
            continue;
          }

          for (Graphic3d_SequenceOfGroup::Iterator aGroupIter (aPrs3d->Groups()); aGroupIter.More(); aGroupIter.Next())
          {
            const Handle(Graphic3d_Group)& aGroup = aGroupIter.Value();
            const Graphic3d_BndBox4f& aBndBox = aGroup->BoundingBox();
            if (aGroup->TransformPersistence().IsNull()
            || !aBndBox.IsValid())
            {
              continue;
            }

            Bnd_Box aGroupBox;
            aGroupBox.Update (aBndBox.CornerMin().x(), aBndBox.CornerMin().y(), aBndBox.CornerMin().z(),
                              aBndBox.CornerMax().x(), aBndBox.CornerMax().y(), aBndBox.CornerMax().z());
            aGroup->TransformPersistence()->Apply (theCamera,
                                                   theProjectionMat, theWorldViewMat,
                                                   theWinSize.x(), theWinSize.y(),
                                                   aGroupBox);
            aBoundingBox.Add (aGroupBox);
          }
        }

        if (aBoundingBox.IsVoid())
        {
          myBoundings.Add (new Select3D_HBndBox3d());
        }
        else
        {
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
    BVHBuilderAdaptorPersistent& operator=(const BVHBuilderAdaptorPersistent&) { return *this; }

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
{
  myBVH[BVHSubset_ortho2dPersistent] = new BVH_Tree<Standard_Real, 3>();
  myBVH[BVHSubset_ortho3dPersistent] = new BVH_Tree<Standard_Real, 3>();
  myBVH[BVHSubset_2dPersistent]      = new BVH_Tree<Standard_Real, 3>();
  myBVH[BVHSubset_3dPersistent]      = new BVH_Tree<Standard_Real, 3>();
  myBVH[BVHSubset_3d]                = new BVH_Tree<Standard_Real, 3>();

  myBuilder[BVHSubset_ortho2dPersistent] = new BVH_LinearBuilder<Standard_Real, 3>    (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_ortho3dPersistent] = new BVH_LinearBuilder<Standard_Real, 3>    (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_2dPersistent]      = new BVH_LinearBuilder<Standard_Real, 3>    (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_3dPersistent]      = new BVH_LinearBuilder<Standard_Real, 3>    (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_3d]                = new BVH_BinnedBuilder<Standard_Real, 3, 4> (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth, Standard_True);

  myIsDirty[BVHSubset_ortho2dPersistent] = Standard_False;
  myIsDirty[BVHSubset_ortho3dPersistent] = Standard_False;
  myIsDirty[BVHSubset_2dPersistent]      = Standard_False;
  myIsDirty[BVHSubset_3dPersistent]      = Standard_False;
  myIsDirty[BVHSubset_3d]                = Standard_False;
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
  if (currentSubset (theObject) != -1)
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
void SelectMgr_SelectableObjectSet::UpdateBVH (const Handle(Graphic3d_Camera)& theCam,
                                               const Graphic3d_Vec2i& theWinSize)
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

  if (!theCam.IsNull())
  {
    const Standard_Boolean           isWinSizeChanged = myLastWinSize != theWinSize;
    const Graphic3d_Mat4d&              aProjMat      = theCam->ProjectionMatrix();
    const Graphic3d_Mat4d&              aWorldViewMat = theCam->OrientationMatrix();
    const Graphic3d_WorldViewProjState& aViewState    = theCam->WorldViewProjState();

    // -----------------------------------------------------
    // check and update 3D persistence BVH tree if necessary
    // -----------------------------------------------------
    if (!IsEmpty (BVHSubset_3dPersistent)
     && (myIsDirty[BVHSubset_3dPersistent]
      || myLastViewState.IsChanged (aViewState)
      || isWinSizeChanged))
    {
      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor (myObjects[BVHSubset_3dPersistent],
                                             theCam, aProjMat, aWorldViewMat, theWinSize);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_3dPersistent]->Build (&anAdaptor, myBVH[BVHSubset_3dPersistent].get(), anAdaptor.Box());
    }

    // -----------------------------------------------------
    // check and update 2D persistence BVH tree if necessary
    // -----------------------------------------------------
    if (!IsEmpty (BVHSubset_2dPersistent)
     && (myIsDirty[BVHSubset_2dPersistent]
      || myLastViewState.IsProjectionChanged (aViewState)
      || isWinSizeChanged))
    {
      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor (myObjects[BVHSubset_2dPersistent],
                                             theCam, aProjMat, SelectMgr_SelectableObjectSet_THE_IDENTITY_MAT, theWinSize);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_2dPersistent]->Build (&anAdaptor, myBVH[BVHSubset_2dPersistent].get(), anAdaptor.Box());
    }

    // -------------------------------------------------------------------
    // check and update 3D orthographic persistence BVH tree if necessary
    // -------------------------------------------------------------------
    if (!IsEmpty (BVHSubset_ortho3dPersistent)
      && (myIsDirty[BVHSubset_ortho3dPersistent]
       || myLastViewState.IsChanged (aViewState)
       || isWinSizeChanged))
    {
      Handle(Graphic3d_Camera) aNewOrthoCam = new Graphic3d_Camera (*theCam); // If OrthoPers, copy camera and set to orthographic projection
      aNewOrthoCam->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);

      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor (myObjects[BVHSubset_ortho3dPersistent],
                                             aNewOrthoCam, aNewOrthoCam->ProjectionMatrix(), 
                                             aNewOrthoCam->OrientationMatrix(), theWinSize);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_ortho3dPersistent]->Build (&anAdaptor, myBVH[BVHSubset_ortho3dPersistent].get(), anAdaptor.Box());
    }

    // -------------------------------------------------------------------
    // check and update 2D orthographic persistence BVH tree if necessary
    // -------------------------------------------------------------------
    if (!IsEmpty (BVHSubset_ortho2dPersistent)
      && (myIsDirty[BVHSubset_ortho2dPersistent]
       || myLastViewState.IsProjectionChanged (aViewState)
       || isWinSizeChanged))
    {
      Handle(Graphic3d_Camera) aNewOrthoCam = new Graphic3d_Camera (*theCam); // If OrthoPers, copy camera and set to orthographic projection
      aNewOrthoCam->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);

      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor (myObjects[BVHSubset_ortho2dPersistent],
                                             aNewOrthoCam, aNewOrthoCam->ProjectionMatrix(), 
                                             SelectMgr_SelectableObjectSet_THE_IDENTITY_MAT, theWinSize);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_ortho2dPersistent]->Build (&anAdaptor, myBVH[BVHSubset_ortho2dPersistent].get(), anAdaptor.Box());
    }

    // release dirty state for every subset
    myIsDirty[BVHSubset_3dPersistent]      = Standard_False;
    myIsDirty[BVHSubset_2dPersistent]      = Standard_False;
    myIsDirty[BVHSubset_ortho3dPersistent] = Standard_False;
    myIsDirty[BVHSubset_ortho2dPersistent] = Standard_False;

    // keep last view state
    myLastViewState = aViewState;
  }

  // keep last window state
  myLastWinSize = theWinSize;
}

//=============================================================================
// Function: MarkDirty
// Purpose :
//=============================================================================
void SelectMgr_SelectableObjectSet::MarkDirty()
{
  myIsDirty[BVHSubset_3d]                = Standard_True;
  myIsDirty[BVHSubset_3dPersistent]      = Standard_True;
  myIsDirty[BVHSubset_2dPersistent]      = Standard_True;
  myIsDirty[BVHSubset_ortho3dPersistent] = Standard_True;
  myIsDirty[BVHSubset_ortho2dPersistent] = Standard_True;
}
//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void SelectMgr_SelectableObjectSet::DumpJson (Standard_OStream& theOStream, Standard_Integer) const 
{
  for (Standard_Integer aSubsetIdx = 0; aSubsetIdx < BVHSubsetNb; ++aSubsetIdx)
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, aSubsetIdx)

    Standard_Boolean IsDirty = myIsDirty[aSubsetIdx];
    OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, IsDirty)

    for (NCollection_IndexedMap<Handle(SelectMgr_SelectableObject)>::Iterator anObjectIt (myObjects[aSubsetIdx]);
         anObjectIt.More(); anObjectIt.Next())
    {
      const Handle(SelectMgr_SelectableObject)& SelectableObject = anObjectIt.Value();
      OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, SelectableObject.get())
    }

    TCollection_AsciiString separator;
    OCCT_DUMP_FIELD_VALUE_STRING (theOStream, separator)
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myLastWinSize.x())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myLastWinSize.y())
}

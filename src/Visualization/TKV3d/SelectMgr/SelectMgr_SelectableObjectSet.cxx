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
#include <gp_Trsf.hxx>
#include <NCollection_Mat4.hxx>
#include <NCollection_Vec3.hxx>
#include <NCollection_Vec4.hxx>

#include <BVH_BinnedBuilder.hxx>
#include <BVH_LinearBuilder.hxx>

namespace
{
//! Short-cut definition of indexed data map of selectable objects
typedef NCollection_IndexedMap<occ::handle<SelectMgr_SelectableObject>> ObjectsMap;

//=================================================================================================

//! This class provides direct access to fields of SelectMgr_SelectableObjectSet
//! so the BVH builder could explicitly arrange objects in the map as necessary
//! to provide synchronization of indexes with constructed BVH tree.
class BVHBuilderAdaptorRegular : public BVH_Set<double, 3>
{
public:
  //! Construct adaptor.
  BVHBuilderAdaptorRegular(ObjectsMap& theObjects)
      : myObjects(theObjects) {};

  //! Returns bounding box of object with index theIndex
  virtual Select3D_BndBox3d Box(const int theIndex) const override
  {
    const occ::handle<SelectMgr_SelectableObject>& anObject = myObjects.FindKey(theIndex + 1);
    Bnd_Box                                        aBox;
    anObject->BoundingBox(aBox);
    if (aBox.IsVoid())
      return Select3D_BndBox3d();

    return Select3D_BndBox3d(
      NCollection_Vec3<double>(aBox.CornerMin().X(), aBox.CornerMin().Y(), aBox.CornerMin().Z()),
      NCollection_Vec3<double>(aBox.CornerMax().X(), aBox.CornerMax().Y(), aBox.CornerMax().Z()));
  }

  //! Returns bounding box of the whole subset.
  virtual Select3D_BndBox3d Box() const override
  {
    if (!myBox.IsValid())
    {
      myBox = BVH_Set<double, 3>::Box();
    }
    return myBox;
  }

  //! Make inherited method Box() visible to avoid CLang warning
  using BVH_Set<double, 3>::Box;

  //! Returns center of object with index theIndex in the set
  //! along the given axis theAxis
  virtual double Center(const int theIndex, const int theAxis) const override
  {
    const Select3D_BndBox3d aBndBox = Box(theIndex);

    return (aBndBox.CornerMin()[theAxis] + aBndBox.CornerMax()[theAxis]) * 0.5;
  }

  //! Returns size of objects set.
  virtual int Size() const override { return myObjects.Size(); }

  //! Swaps items with indexes theIndex1 and theIndex2 in the set
  virtual void Swap(const int theIndex1, const int theIndex2) override
  {
    myObjects.Swap(theIndex1 + 1, theIndex2 + 1);
  }

private:
  BVHBuilderAdaptorRegular& operator=(const BVHBuilderAdaptorRegular&) { return *this; }

private:
  ObjectsMap&               myObjects;
  mutable Select3D_BndBox3d myBox;
};

//=================================================================================================

//! This class provides direct access to fields of SelectMgr_SelectableObjectSet
//! so the BVH builder could explicitly arrange objects in the map as necessary
//! to provide synchronization of indexes with constructed BVH tree.
class BVHBuilderAdaptorPersistent : public BVH_Set<double, 3>
{
public:
  //! Construct adaptor.
  //! @param theCamera, theProjectionMat, theWorldViewMat,
  //!        theWidth, theHeight [in] view properties used for computation of
  //!        bounding boxes within the world view camera space.
  BVHBuilderAdaptorPersistent(ObjectsMap&                          theObjects,
                              const occ::handle<Graphic3d_Camera>& theCamera,
                              const NCollection_Mat4<double>&      theProjectionMat,
                              const NCollection_Mat4<double>&      theWorldViewMat,
                              const NCollection_Vec2<int>&         theWinSize)
      : myObjects(theObjects)
  {
    myBoundings.ReSize(myObjects.Size());
    for (int anI = 1; anI <= myObjects.Size(); ++anI)
    {
      const occ::handle<SelectMgr_SelectableObject>& anObject = myObjects(anI);

      Bnd_Box aBoundingBox;
      anObject->BoundingBox(aBoundingBox);
      if (!aBoundingBox.IsVoid() && !anObject->TransformPersistence().IsNull())
      {
        anObject->TransformPersistence()->Apply(theCamera,
                                                theProjectionMat,
                                                theWorldViewMat,
                                                theWinSize.x(),
                                                theWinSize.y(),
                                                aBoundingBox);
      }

      // processing presentations with own transform persistence
      for (NCollection_Sequence<occ::handle<PrsMgr_Presentation>>::Iterator aPrsIter(
             anObject->Presentations());
           aPrsIter.More();
           aPrsIter.Next())
      {
        const occ::handle<PrsMgr_Presentation>& aPrs3d = aPrsIter.Value();
        if (!aPrs3d->CStructure()->HasGroupTransformPersistence())
        {
          continue;
        }

        for (NCollection_Sequence<occ::handle<Graphic3d_Group>>::Iterator aGroupIter(
               aPrs3d->Groups());
             aGroupIter.More();
             aGroupIter.Next())
        {
          const occ::handle<Graphic3d_Group>& aGroup  = aGroupIter.Value();
          const Graphic3d_BndBox4f&           aBndBox = aGroup->BoundingBox();
          if (aGroup->TransformPersistence().IsNull() || !aBndBox.IsValid())
          {
            continue;
          }

          Bnd_Box aGroupBox;
          aGroupBox.Update(aBndBox.CornerMin().x(),
                           aBndBox.CornerMin().y(),
                           aBndBox.CornerMin().z(),
                           aBndBox.CornerMax().x(),
                           aBndBox.CornerMax().y(),
                           aBndBox.CornerMax().z());
          aGroup->TransformPersistence()->Apply(theCamera,
                                                theProjectionMat,
                                                theWorldViewMat,
                                                theWinSize.x(),
                                                theWinSize.y(),
                                                aGroupBox);
          aBoundingBox.Add(aGroupBox);
        }
      }

      if (aBoundingBox.IsVoid())
      {
        myBoundings.Add(new Select3D_HBndBox3d());
      }
      else
      {
        const gp_Pnt aMin = aBoundingBox.CornerMin();
        const gp_Pnt aMax = aBoundingBox.CornerMax();
        myBoundings.Add(
          new Select3D_HBndBox3d(NCollection_Vec3<double>(aMin.X(), aMin.Y(), aMin.Z()),
                                 NCollection_Vec3<double>(aMax.X(), aMax.Y(), aMax.Z())));
      }
    }
  }

  //! Returns bounding box of object with index theIndex
  virtual Select3D_BndBox3d Box(const int theIndex) const override
  {
    return *myBoundings(theIndex + 1);
  }

  //! Returns bounding box of the whole subset.
  virtual Select3D_BndBox3d Box() const override
  {
    if (!myBox.IsValid())
    {
      myBox = BVH_Set<double, 3>::Box();
    }
    return myBox;
  }

  //! Make inherited method Box() visible to avoid CLang warning
  using BVH_Set<double, 3>::Box;

  //! Returns center of object with index theIndex in the set
  //! along the given axis theAxis
  virtual double Center(const int theIndex, const int theAxis) const override
  {
    const Select3D_BndBox3d& aBoundingBox = *myBoundings(theIndex + 1);

    return (aBoundingBox.CornerMin()[theAxis] + aBoundingBox.CornerMax()[theAxis]) * 0.5;
  }

  //! Returns size of objects set.
  virtual int Size() const override { return myObjects.Size(); }

  //! Swaps items with indexes theIndex1 and theIndex2 in the set
  virtual void Swap(const int theIndex1, const int theIndex2) override
  {
    const int aStructIdx1 = theIndex1 + 1;
    const int aStructIdx2 = theIndex2 + 1;

    myObjects.Swap(aStructIdx1, aStructIdx2);
    myBoundings.Swap(aStructIdx1, aStructIdx2);
  }

private:
  BVHBuilderAdaptorPersistent& operator=(const BVHBuilderAdaptorPersistent&) { return *this; }

private:
  ObjectsMap&                                             myObjects;
  mutable Select3D_BndBox3d                               myBox;
  typedef NCollection_Shared<Select3D_BndBox3d>           Select3D_HBndBox3d;
  NCollection_IndexedMap<occ::handle<Select3D_HBndBox3d>> myBoundings;
};

static const NCollection_Mat4<double> SelectMgr_SelectableObjectSet_THE_IDENTITY_MAT;
} // namespace

//=================================================================================================

SelectMgr_SelectableObjectSet::SelectMgr_SelectableObjectSet()
{
  myBVH[BVHSubset_ortho2dPersistent] = new BVH_Tree<double, 3>();
  myBVH[BVHSubset_ortho3dPersistent] = new BVH_Tree<double, 3>();
  myBVH[BVHSubset_2dPersistent]      = new BVH_Tree<double, 3>();
  myBVH[BVHSubset_3dPersistent]      = new BVH_Tree<double, 3>();
  myBVH[BVHSubset_3d]                = new BVH_Tree<double, 3>();

  myBuilder[BVHSubset_ortho2dPersistent] =
    new BVH_LinearBuilder<double, 3>(BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_ortho3dPersistent] =
    new BVH_LinearBuilder<double, 3>(BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_2dPersistent] =
    new BVH_LinearBuilder<double, 3>(BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_3dPersistent] =
    new BVH_LinearBuilder<double, 3>(BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
  myBuilder[BVHSubset_3d] = new BVH_BinnedBuilder<double, 3, 4>(BVH_Constants_LeafNodeSizeSingle,
                                                                BVH_Constants_MaxTreeDepth,
                                                                true);

  myIsDirty[BVHSubset_ortho2dPersistent] = false;
  myIsDirty[BVHSubset_ortho3dPersistent] = false;
  myIsDirty[BVHSubset_2dPersistent]      = false;
  myIsDirty[BVHSubset_3dPersistent]      = false;
  myIsDirty[BVHSubset_3d]                = false;
}

//=================================================================================================

bool SelectMgr_SelectableObjectSet::Append(const occ::handle<SelectMgr_SelectableObject>& theObject)
{
  // get an appropriate BVH subset to insert the object into it
  const int aSubsetIdx = appropriateSubset(theObject);

  // check that the object is excluded from other subsets
  if (currentSubset(theObject) != -1)
  {
    return false;
  }

  // try adding it into the appropriate object subset
  const int aSize = myObjects[aSubsetIdx].Size();

  if (aSize < myObjects[aSubsetIdx].Add(theObject))
  {
    myIsDirty[aSubsetIdx] = true;

    return true;
  }

  return false;
}

//=================================================================================================

bool SelectMgr_SelectableObjectSet::Remove(const occ::handle<SelectMgr_SelectableObject>& theObject)
{
  // remove object from the first found subset
  for (int aSubsetIdx = 0; aSubsetIdx < BVHSubsetNb; ++aSubsetIdx)
  {
    const int anIndex = myObjects[aSubsetIdx].FindIndex(theObject);

    if (anIndex != 0)
    {
      const int aSize = myObjects[aSubsetIdx].Size();

      if (anIndex != aSize)
      {
        myObjects[aSubsetIdx].Swap(anIndex, aSize);
      }

      myObjects[aSubsetIdx].RemoveLast();
      myIsDirty[aSubsetIdx] = true;

      return true;
    }
  }

  return false;
}

//=================================================================================================

void SelectMgr_SelectableObjectSet::ChangeSubset(
  const occ::handle<SelectMgr_SelectableObject>& theObject)
{
  // do not do anything is object is not in the map
  const int aCurrSubsetIdx = currentSubset(theObject);

  if (aCurrSubsetIdx < 0)
  {
    return;
  }

  // check whether the subset need to be changed at all
  const int aSubsetIdx = appropriateSubset(theObject);

  if (aCurrSubsetIdx == aSubsetIdx)
  {
    return;
  }

  // replace object in the maps
  Remove(theObject);
  Append(theObject);
}

//=================================================================================================

void SelectMgr_SelectableObjectSet::UpdateBVH(const occ::handle<Graphic3d_Camera>& theCam,
                                              const NCollection_Vec2<int>&         theWinSize)
{
  // -----------------------------------------
  // check and update 3D BVH tree if necessary
  // -----------------------------------------
  if (!IsEmpty(BVHSubset_3d) && myIsDirty[BVHSubset_3d])
  {
    // construct adaptor over private fields to provide direct access for the BVH builder
    BVHBuilderAdaptorRegular anAdaptor(myObjects[BVHSubset_3d]);

    // update corresponding BVH tree data structure
    myBuilder[BVHSubset_3d]->Build(&anAdaptor, myBVH[BVHSubset_3d].get(), anAdaptor.Box());

    // release dirty state
    myIsDirty[BVHSubset_3d] = false;
  }

  if (!theCam.IsNull())
  {
    const bool                          isWinSizeChanged = myLastWinSize != theWinSize;
    const NCollection_Mat4<double>&     aProjMat         = theCam->ProjectionMatrix();
    const NCollection_Mat4<double>&     aWorldViewMat    = theCam->OrientationMatrix();
    const Graphic3d_WorldViewProjState& aViewState       = theCam->WorldViewProjState();

    // -----------------------------------------------------
    // check and update 3D persistence BVH tree if necessary
    // -----------------------------------------------------
    if (!IsEmpty(BVHSubset_3dPersistent)
        && (myIsDirty[BVHSubset_3dPersistent] || myLastViewState.IsChanged(aViewState)
            || isWinSizeChanged))
    {
      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor(myObjects[BVHSubset_3dPersistent],
                                            theCam,
                                            aProjMat,
                                            aWorldViewMat,
                                            theWinSize);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_3dPersistent]->Build(&anAdaptor,
                                               myBVH[BVHSubset_3dPersistent].get(),
                                               anAdaptor.Box());
    }

    // -----------------------------------------------------
    // check and update 2D persistence BVH tree if necessary
    // -----------------------------------------------------
    if (!IsEmpty(BVHSubset_2dPersistent)
        && (myIsDirty[BVHSubset_2dPersistent] || myLastViewState.IsProjectionChanged(aViewState)
            || isWinSizeChanged))
    {
      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor(myObjects[BVHSubset_2dPersistent],
                                            theCam,
                                            aProjMat,
                                            SelectMgr_SelectableObjectSet_THE_IDENTITY_MAT,
                                            theWinSize);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_2dPersistent]->Build(&anAdaptor,
                                               myBVH[BVHSubset_2dPersistent].get(),
                                               anAdaptor.Box());
    }

    // -------------------------------------------------------------------
    // check and update 3D orthographic persistence BVH tree if necessary
    // -------------------------------------------------------------------
    if (!IsEmpty(BVHSubset_ortho3dPersistent)
        && (myIsDirty[BVHSubset_ortho3dPersistent] || myLastViewState.IsChanged(aViewState)
            || isWinSizeChanged))
    {
      // clang-format off
      occ::handle<Graphic3d_Camera> aNewOrthoCam = new Graphic3d_Camera (*theCam); // If OrthoPers, copy camera and set to orthographic projection
      // clang-format on
      aNewOrthoCam->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);

      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor(myObjects[BVHSubset_ortho3dPersistent],
                                            aNewOrthoCam,
                                            aNewOrthoCam->ProjectionMatrix(),
                                            aNewOrthoCam->OrientationMatrix(),
                                            theWinSize);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_ortho3dPersistent]->Build(&anAdaptor,
                                                    myBVH[BVHSubset_ortho3dPersistent].get(),
                                                    anAdaptor.Box());
    }

    // -------------------------------------------------------------------
    // check and update 2D orthographic persistence BVH tree if necessary
    // -------------------------------------------------------------------
    if (!IsEmpty(BVHSubset_ortho2dPersistent)
        && (myIsDirty[BVHSubset_ortho2dPersistent]
            || myLastViewState.IsProjectionChanged(aViewState) || isWinSizeChanged))
    {
      // clang-format off
      occ::handle<Graphic3d_Camera> aNewOrthoCam = new Graphic3d_Camera (*theCam); // If OrthoPers, copy camera and set to orthographic projection
      // clang-format on
      aNewOrthoCam->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);

      // construct adaptor over private fields to provide direct access for the BVH builder
      BVHBuilderAdaptorPersistent anAdaptor(myObjects[BVHSubset_ortho2dPersistent],
                                            aNewOrthoCam,
                                            aNewOrthoCam->ProjectionMatrix(),
                                            SelectMgr_SelectableObjectSet_THE_IDENTITY_MAT,
                                            theWinSize);

      // update corresponding BVH tree data structure
      myBuilder[BVHSubset_ortho2dPersistent]->Build(&anAdaptor,
                                                    myBVH[BVHSubset_ortho2dPersistent].get(),
                                                    anAdaptor.Box());
    }

    // release dirty state for every subset
    myIsDirty[BVHSubset_3dPersistent]      = false;
    myIsDirty[BVHSubset_2dPersistent]      = false;
    myIsDirty[BVHSubset_ortho3dPersistent] = false;
    myIsDirty[BVHSubset_ortho2dPersistent] = false;

    // keep last view state
    myLastViewState = aViewState;
  }

  // keep last window state
  myLastWinSize = theWinSize;
}

//=================================================================================================

void SelectMgr_SelectableObjectSet::MarkDirty()
{
  myIsDirty[BVHSubset_3d]                = true;
  myIsDirty[BVHSubset_3dPersistent]      = true;
  myIsDirty[BVHSubset_2dPersistent]      = true;
  myIsDirty[BVHSubset_ortho3dPersistent] = true;
  myIsDirty[BVHSubset_ortho2dPersistent] = true;
}

//=================================================================================================

void SelectMgr_SelectableObjectSet::DumpJson(Standard_OStream& theOStream, int) const
{
  for (int aSubsetIdx = 0; aSubsetIdx < BVHSubsetNb; ++aSubsetIdx)
  {
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aSubsetIdx)

    bool IsDirty = myIsDirty[aSubsetIdx];
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, IsDirty)

    for (NCollection_IndexedMap<occ::handle<SelectMgr_SelectableObject>>::Iterator anObjectIt(
           myObjects[aSubsetIdx]);
         anObjectIt.More();
         anObjectIt.Next())
    {
      const occ::handle<SelectMgr_SelectableObject>& SelectableObject = anObjectIt.Value();
      OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, SelectableObject.get())
    }

    TCollection_AsciiString separator;
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, separator)
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLastWinSize.x())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myLastWinSize.y())
}

// Created on: 1995-02-15
// Created by: Roberc Coublanc
// Copyright (c) 1995-1999 Matra Datavision
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

#include <SelectMgr_ViewerSelector.hxx>

#include <BVH_Tree.hxx>
#include <gp_GTrsf.hxx>
#include <gp_Pnt.hxx>
#include <OSD_Environment.hxx>
#include <Precision.hxx>
#include <SelectBasics_EntityOwner.hxx>
#include <SelectBasics_SensitiveEntity.hxx>
#include <SelectBasics_PickResult.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_SortCriterion.hxx>
#include <SelectMgr_SensitiveEntitySet.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_ViewerSelector, Standard_Transient)

namespace {
  // Comparison operator for sorting selection results
  class CompareResults
  {
  public:
   
    CompareResults (const SelectMgr_IndexedDataMapOfOwnerCriterion& aMapOfCriterion)
      : myMapOfCriterion (aMapOfCriterion)
    {
    }

    Standard_Boolean operator() (Standard_Integer theLeft, Standard_Integer theRight) const
    {
      return myMapOfCriterion.FindFromIndex(theLeft) > myMapOfCriterion.FindFromIndex(theRight);
    }

  private:
    void operator = (const CompareResults&);

  private:
    const SelectMgr_IndexedDataMapOfOwnerCriterion&  myMapOfCriterion;
  };

  //! Compute 3d position for detected entity.
  inline void updatePoint3d (SelectMgr_SortCriterion& theCriterion,
                             const gp_GTrsf& theInversedTrsf,
                             SelectMgr_SelectingVolumeManager& theMgr)
  {
    if (theMgr.GetActiveSelectionType() != SelectMgr_SelectingVolumeManager::Point)
    {
      return;
    }

    theCriterion.Point = theMgr.DetectedPoint (theCriterion.Depth);
    gp_GTrsf anInvTrsf = theInversedTrsf;
    if (theCriterion.Entity->HasInitLocation())
    {
      anInvTrsf = theCriterion.Entity->InvInitLocation() * anInvTrsf;
    }
    if (anInvTrsf.Form() != gp_Identity)
    {
      anInvTrsf.Inverted().Transforms (theCriterion.Point.ChangeCoord());
    }
  }

  static const Graphic3d_Mat4d SelectMgr_ViewerSelector_THE_IDENTITY_MAT;
}

//==================================================
// Function: Initialize
// Purpose :
//==================================================
SelectMgr_ViewerSelector::SelectMgr_ViewerSelector():
preferclosest(Standard_True),
myToUpdateTolerance (Standard_True),
myCurRank (0),
myIsLeftChildQueuedFirst (Standard_False),
myEntityIdx (0)
{
  myEntitySetBuilder = new BVH_BinnedBuilder<Standard_Real, 3, 4> (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth, Standard_True);
}

//==================================================
// Function: Activate
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::Activate (const Handle(SelectMgr_Selection)& theSelection)
{
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    theSelection->Sensitive()->SetActiveForSelection();
  }

  theSelection->SetSelectionState (SelectMgr_SOS_Activated);

  myTolerances.Add (theSelection->Sensitivity());
  myToUpdateTolerance = Standard_True;
}

//==================================================
// Function: Deactivate
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::Deactivate (const Handle(SelectMgr_Selection)& theSelection)
{
  for (theSelection->Init(); theSelection->More(); theSelection->Next())
  {
    theSelection->Sensitive()->ResetSelectionActiveStatus();
  }

  theSelection->SetSelectionState (SelectMgr_SOS_Deactivated);

  myTolerances.Decrement (theSelection->Sensitivity());
  myToUpdateTolerance = Standard_True;
}

//==================================================
// Function: Clear
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::Clear()
{
  mystored.Clear();
}

//=======================================================================
// function: isToScaleFrustum
// purpose : Checks if the entity given requires to scale current selecting frustum
//=======================================================================
Standard_Boolean SelectMgr_ViewerSelector::isToScaleFrustum (const Handle(SelectBasics_SensitiveEntity)& theEntity)
{
  return mySelectingVolumeMgr.GetActiveSelectionType() == SelectMgr_SelectingVolumeManager::Point
    && sensitivity (theEntity) < myTolerances.Tolerance();
}

//=======================================================================
// function: sensitivity
// purpose : In case if custom tolerance is set, this method will return sum of entity
//           sensitivity and custom tolerance.
//=======================================================================
Standard_Integer SelectMgr_ViewerSelector::sensitivity (const Handle(SelectBasics_SensitiveEntity)& theEntity) const
{
  return myTolerances.IsCustomTolSet() ?
    theEntity->SensitivityFactor() + myTolerances.CustomTolerance() : theEntity->SensitivityFactor();
}

//=======================================================================
// function: checkOverlap
// purpose : Internal function that checks if a particular sensitive
//           entity theEntity overlaps current selecting volume precisely
//=======================================================================
void SelectMgr_ViewerSelector::checkOverlap (const Handle(SelectBasics_SensitiveEntity)& theEntity,
                                             const gp_GTrsf& theInversedTrsf,
                                             SelectMgr_SelectingVolumeManager& theMgr)
{
  Handle(SelectMgr_EntityOwner) anOwner (Handle(SelectMgr_EntityOwner)::DownCast (theEntity->OwnerId()));
  Handle(SelectMgr_SelectableObject) aSelectable;
  Standard_Boolean toRestoresViewClipEnabled = Standard_False;
  if (!anOwner.IsNull())
  {
    aSelectable = anOwner->Selectable();
    if (!aSelectable->ClipPlanes().IsNull()
      && aSelectable->ClipPlanes()->ToOverrideGlobal())
    {
      theMgr.SetViewClippingEnabled (Standard_False);
      toRestoresViewClipEnabled = Standard_True;
    }
    else if (!aSelectable->TransformPersistence().IsNull())
    {
      if (aSelectable->TransformPersistence()->IsZoomOrRotate()
      && !theMgr.ViewClipping().IsNull())
      {
        // Zoom/rotate persistence object lives in two worlds at the same time.
        // Global clipping planes can not be trivially applied without being converted
        // into local space of transformation persistence object.
        // As more simple alternative - just clip entire object by its anchor point defined in the world space.
        const Handle(Graphic3d_SequenceOfHClipPlane)& aViewPlanes = theMgr.ViewClipping();

        const gp_Pnt anAnchor = aSelectable->TransformPersistence()->AnchorPoint();
        for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aViewPlanes); aPlaneIt.More(); aPlaneIt.Next())
        {
          const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
          if (!aPlane->IsOn())
          {
            continue;
          }

          const Graphic3d_Vec4d& aPlaneEquation = aPlane->GetEquation();
          const Graphic3d_Vec4d  aCheckPnt (anAnchor.X(), anAnchor.Y(), anAnchor.Z(), 1.0);
          if (aPlaneEquation.Dot (aCheckPnt) < 0.0) // vertex is outside the half-space
          {
            return;
          }
        }
      }

      theMgr.SetViewClippingEnabled (Standard_False);
      toRestoresViewClipEnabled = Standard_True;
    }
  }

  SelectBasics_PickResult aPickResult;
  const Standard_Boolean isMatched = theEntity->Matches(theMgr, aPickResult);
  if (toRestoresViewClipEnabled)
  {
    theMgr.SetViewClippingEnabled (Standard_True);
  }

  if (!isMatched
    || anOwner.IsNull())
  {
    return;
  }

  if (HasDepthClipping (anOwner)
  &&  theMgr.GetActiveSelectionType() == SelectMgr_SelectingVolumeManager::Point)
  {
    Standard_Boolean isClipped = mySelectingVolumeMgr.IsClipped (*aSelectable->ClipPlanes(),
                                                                  aPickResult.Depth());
    if (isClipped)
      return;
  }

  SelectMgr_SortCriterion aCriterion;
  myZLayerOrderMap.Find (aSelectable->ZLayer(), aCriterion.ZLayerPosition);
  aCriterion.Entity    = theEntity;
  aCriterion.Priority  = anOwner->Priority();
  aCriterion.Depth     = aPickResult.Depth();
  aCriterion.MinDist   = aPickResult.DistToGeomCenter();
  aCriterion.Tolerance = theEntity->SensitivityFactor() / 33.0;
  aCriterion.ToPreferClosest = preferclosest;

  const Standard_Integer aPrevStoredIndex = mystored.FindIndex (anOwner);
  if (aPrevStoredIndex != 0)
  {
    if (theMgr.GetActiveSelectionType() != SelectBasics_SelectingVolumeManager::Box)
    {
      SelectMgr_SortCriterion& aPrevCriterion = mystored.ChangeFromIndex (aPrevStoredIndex);
      if (aCriterion > aPrevCriterion)
      {
        updatePoint3d (aCriterion, theInversedTrsf, theMgr);
        aPrevCriterion = aCriterion;
      }
    }
  }
  else
  {
    updatePoint3d (aCriterion, theInversedTrsf, theMgr);
    mystored.Add (anOwner, aCriterion);
  }
}

//=======================================================================
// function: computeFrustum
// purpose : Internal function that checks if a current selecting frustum
//           needs to be scaled and transformed for the entity and performs
//           necessary calculations
//=======================================================================
void SelectMgr_ViewerSelector::computeFrustum (const Handle(SelectBasics_SensitiveEntity)& theEnt,
                                               const SelectMgr_SelectingVolumeManager&     theMgr,
                                               const gp_GTrsf&                             theInvTrsf,
                                               SelectMgr_FrustumCache&                     theCachedMgrs,
                                               SelectMgr_SelectingVolumeManager&           theResMgr)
{
  Standard_Integer aScale = isToScaleFrustum (theEnt) ? sensitivity (theEnt) : 1;
  const gp_GTrsf aTrsfMtr = theEnt->HasInitLocation() ? theEnt->InvInitLocation() * theInvTrsf : theInvTrsf;
  const Standard_Boolean toScale = aScale != 1;
  const Standard_Boolean toTransform = aTrsfMtr.Form() != gp_Identity;
  if (toScale && toTransform)
  {
    theResMgr = theMgr.ScaleAndTransform (aScale, aTrsfMtr, NULL);
  }
  else if (toScale)
  {
    if (!theCachedMgrs.IsBound (aScale))
    {
      theCachedMgrs.Bind (aScale, theMgr.ScaleAndTransform (aScale, gp_Trsf(), NULL));
    }
    theResMgr = theCachedMgrs.Find (aScale);
  }
  else if (toTransform)
  {
    theResMgr = theMgr.ScaleAndTransform (1, aTrsfMtr, NULL);
  }
}

//=======================================================================
// function: traverseObject
// purpose : Internal function that checks if there is possible overlap
//           between some entity of selectable object theObject and
//           current selecting volume
//=======================================================================
void SelectMgr_ViewerSelector::traverseObject (const Handle(SelectMgr_SelectableObject)& theObject,
                                               const SelectMgr_SelectingVolumeManager& theMgr,
                                               const Handle(Graphic3d_Camera)& theCamera,
                                               const Graphic3d_Mat4d& theProjectionMat,
                                               const Graphic3d_Mat4d& theWorldViewMat,
                                               const Standard_Integer theViewportWidth,
                                               const Standard_Integer theViewportHeight)
{
  Handle(SelectMgr_SensitiveEntitySet)& anEntitySet = myMapOfObjectSensitives.ChangeFind (theObject);
  if (anEntitySet->Size() == 0)
  {
    return;
  }

  const opencascade::handle<BVH_Tree<Standard_Real, 3> >& aSensitivesTree = anEntitySet->BVH();
  gp_GTrsf aInversedTrsf;
  if (theObject->HasTransformation() || !theObject->TransformPersistence().IsNull())
  {
    if (theObject->TransformPersistence().IsNull())
    {
      aInversedTrsf = theObject->InversedTransformation();
    }
    else
    {
      gp_GTrsf aTPers;
      Graphic3d_Mat4d aMat = theObject->TransformPersistence()->Compute (theCamera, theProjectionMat, theWorldViewMat, theViewportWidth, theViewportHeight);

      aTPers.SetValue (1, 1, aMat.GetValue (0, 0));
      aTPers.SetValue (1, 2, aMat.GetValue (0, 1));
      aTPers.SetValue (1, 3, aMat.GetValue (0, 2));
      aTPers.SetValue (2, 1, aMat.GetValue (1, 0));
      aTPers.SetValue (2, 2, aMat.GetValue (1, 1));
      aTPers.SetValue (2, 3, aMat.GetValue (1, 2));
      aTPers.SetValue (3, 1, aMat.GetValue (2, 0));
      aTPers.SetValue (3, 2, aMat.GetValue (2, 1));
      aTPers.SetValue (3, 3, aMat.GetValue (2, 2));
      aTPers.SetTranslationPart (gp_XYZ (aMat.GetValue (0, 3), aMat.GetValue (1, 3), aMat.GetValue (2, 3)));

      aInversedTrsf = (aTPers * gp_GTrsf (theObject->Transformation())).Inverted();
    }
  }

  SelectMgr_SelectingVolumeManager aMgr = aInversedTrsf.Form() != gp_Identity
                                        ? theMgr.ScaleAndTransform (1, aInversedTrsf, NULL)
                                        : theMgr;

  SelectMgr_FrustumCache aScaledTrnsfFrustums;

  Standard_Integer aNode = 0; // a root node
  if (!aMgr.Overlaps (aSensitivesTree->MinPoint (0),
                      aSensitivesTree->MaxPoint (0)))
  {
    return;
  }
  Standard_Integer aStack[BVH_Constants_MaxTreeDepth];
  Standard_Integer aHead = -1;
  for (;;)
  {
    if (!aSensitivesTree->IsOuter (aNode))
    {
      const Standard_Integer aLeftChildIdx  = aSensitivesTree->Child<0> (aNode);
      const Standard_Integer aRightChildIdx = aSensitivesTree->Child<1> (aNode);
      const Standard_Boolean isLeftChildIn  =  aMgr.Overlaps (aSensitivesTree->MinPoint (aLeftChildIdx),
                                                              aSensitivesTree->MaxPoint (aLeftChildIdx));
      const Standard_Boolean isRightChildIn = aMgr.Overlaps (aSensitivesTree->MinPoint (aRightChildIdx),
                                                             aSensitivesTree->MaxPoint (aRightChildIdx));
      if (isLeftChildIn
          && isRightChildIn)
      {
        aNode = aLeftChildIdx;
        ++aHead;
        aStack[aHead] = aRightChildIdx;
      }
      else if (isLeftChildIn
        || isRightChildIn)
      {
        aNode = isLeftChildIn ? aLeftChildIdx : aRightChildIdx;
      }
      else
      {
        if (aHead < 0)
        {
          break;
        }

        aNode = aStack[aHead];
        --aHead;
      }
    }
    else
    {
      Standard_Integer aStartIdx = aSensitivesTree->BegPrimitive (aNode);
      Standard_Integer anEndIdx = aSensitivesTree->EndPrimitive (aNode);
      for (Standard_Integer anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
      {
        const Handle(SelectMgr_SensitiveEntity)& aSensitive =
          anEntitySet->GetSensitiveById (anIdx);
        if (aSensitive->IsActiveForSelection())
        {
          const Handle(SelectBasics_SensitiveEntity)& anEnt = aSensitive->BaseSensitive();
          SelectMgr_SelectingVolumeManager aTmpMgr = aMgr;
          computeFrustum (anEnt, theMgr, aInversedTrsf, aScaledTrnsfFrustums, aTmpMgr);
          checkOverlap (anEnt, aInversedTrsf, aTmpMgr);
        }
      }
      if (aHead < 0)
      {
        break;
      }

      aNode = aStack[aHead];
      --aHead;
    }
  }
}

//=======================================================================
// function: TraverseSensitives
// purpose : Traverses BVH containing all added selectable objects and
//           finds candidates for further search of overlap
//=======================================================================
void SelectMgr_ViewerSelector::TraverseSensitives()
{
  mystored.Clear();

  Standard_Integer aWidth;
  Standard_Integer aHeight;
  mySelectingVolumeMgr.WindowSize (aWidth, aHeight);
  mySelectableObjects.UpdateBVH (mySelectingVolumeMgr.Camera(),
                                 mySelectingVolumeMgr.ProjectionMatrix(),
                                 mySelectingVolumeMgr.WorldViewMatrix(),
                                 mySelectingVolumeMgr.WorldViewProjState(),
                                 aWidth, aHeight);

  for (Standard_Integer aBVHSetIt = 0; aBVHSetIt < SelectMgr_SelectableObjectSet::BVHSubsetNb; ++aBVHSetIt)
  {
    SelectMgr_SelectableObjectSet::BVHSubset aBVHSubset =
      static_cast<SelectMgr_SelectableObjectSet::BVHSubset> (aBVHSetIt);

    if (mySelectableObjects.IsEmpty (aBVHSubset))
    {
      continue;
    }

    gp_GTrsf aTFrustum;

    SelectMgr_SelectingVolumeManager aMgr (Standard_False);

    // for 2D space selection transform selecting volumes to perform overap testing
    // directly in camera's eye space omitting the camera position, which is not
    // needed there at all
    if (aBVHSubset == SelectMgr_SelectableObjectSet::BVHSubset_2dPersistent)
    {
      const Graphic3d_Mat4d& aMat = mySelectingVolumeMgr.WorldViewMatrix();
      aTFrustum.SetValue (1, 1, aMat.GetValue (0, 0));
      aTFrustum.SetValue (1, 2, aMat.GetValue (0, 1));
      aTFrustum.SetValue (1, 3, aMat.GetValue (0, 2));
      aTFrustum.SetValue (2, 1, aMat.GetValue (1, 0));
      aTFrustum.SetValue (2, 2, aMat.GetValue (1, 1));
      aTFrustum.SetValue (2, 3, aMat.GetValue (1, 2));
      aTFrustum.SetValue (3, 1, aMat.GetValue (2, 0));
      aTFrustum.SetValue (3, 2, aMat.GetValue (2, 1));
      aTFrustum.SetValue (3, 3, aMat.GetValue (2, 2));
      aTFrustum.SetTranslationPart (gp_XYZ (aMat.GetValue (0, 3), aMat.GetValue (1, 3), aMat.GetValue (2, 3)));

      // define corresponding frustum builder parameters
      Handle(SelectMgr_FrustumBuilder) aBuilder = new SelectMgr_FrustumBuilder();
      aBuilder->SetProjectionMatrix (mySelectingVolumeMgr.ProjectionMatrix());
      aBuilder->SetWorldViewMatrix (SelectMgr_ViewerSelector_THE_IDENTITY_MAT);
      aBuilder->SetWindowSize (aWidth, aHeight);
      aMgr = mySelectingVolumeMgr.ScaleAndTransform (1, aTFrustum, aBuilder);
    }
    else
    {
      aMgr = mySelectingVolumeMgr;
    }

    const Handle(Graphic3d_Camera)& aCamera = mySelectingVolumeMgr.Camera();
    const Graphic3d_Mat4d& aProjectionMat   = mySelectingVolumeMgr.ProjectionMatrix();
    const Graphic3d_Mat4d& aWorldViewMat    = aBVHSubset != SelectMgr_SelectableObjectSet::BVHSubset_2dPersistent
                                            ? mySelectingVolumeMgr.WorldViewMatrix()
                                            : SelectMgr_ViewerSelector_THE_IDENTITY_MAT;

    const opencascade::handle<BVH_Tree<Standard_Real, 3> >& aBVHTree = mySelectableObjects.BVH (aBVHSubset);

    Standard_Integer aNode = 0;
    if (!aMgr.Overlaps (aBVHTree->MinPoint (0), aBVHTree->MaxPoint (0)))
    {
      continue;
    }

    Standard_Integer aStack[BVH_Constants_MaxTreeDepth];
    Standard_Integer aHead = -1;
    for (;;)
    {
      if (!aBVHTree->IsOuter (aNode))
      {
        const Standard_Integer aLeftChildIdx  = aBVHTree->Child<0> (aNode);
        const Standard_Integer aRightChildIdx = aBVHTree->Child<1> (aNode);
        const Standard_Boolean isLeftChildIn  =
          aMgr.Overlaps (aBVHTree->MinPoint (aLeftChildIdx), aBVHTree->MaxPoint (aLeftChildIdx));
        const Standard_Boolean isRightChildIn =
          aMgr.Overlaps (aBVHTree->MinPoint (aRightChildIdx), aBVHTree->MaxPoint (aRightChildIdx));
        if (isLeftChildIn
          && isRightChildIn)
        {
          aNode = aLeftChildIdx;
          ++aHead;
          aStack[aHead] = aRightChildIdx;
        }
        else if (isLeftChildIn
          || isRightChildIn)
        {
          aNode = isLeftChildIn ? aLeftChildIdx : aRightChildIdx;
        }
        else
        {
          if (aHead < 0)
          {
            break;
          }

          aNode = aStack[aHead];
          --aHead;
        }
      }
      else
      {
        Standard_Integer aStartIdx = aBVHTree->BegPrimitive (aNode);
        Standard_Integer anEndIdx  = aBVHTree->EndPrimitive (aNode);
        for (Standard_Integer anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
        {
          const Handle(SelectMgr_SelectableObject)& aSelectableObject =
            mySelectableObjects.GetObjectById (aBVHSubset, anIdx);

          traverseObject (aSelectableObject, aMgr, aCamera, aProjectionMat, aWorldViewMat, aWidth, aHeight);
        }
        if (aHead < 0)
        {
          break;
        }

        aNode = aStack[aHead];
        --aHead;
      }
    }
  }

  SortResult();
}

//==================================================
// Function: Picked
// Purpose :
//==================================================
Handle(SelectMgr_EntityOwner) SelectMgr_ViewerSelector
::Picked() const
{
  Standard_Integer RankInMap = myIndexes->Value (myCurRank);
  const Handle(SelectBasics_EntityOwner)& toto = mystored.FindKey(RankInMap);
  Handle(SelectMgr_EntityOwner) Ownr = Handle(SelectMgr_EntityOwner)::DownCast (toto);
  return Ownr;
}

//=======================================================================
//function : Picked
//purpose  :
//=======================================================================
Handle(SelectMgr_EntityOwner) SelectMgr_ViewerSelector::Picked (const Standard_Integer theRank) const
{
  Handle(SelectMgr_EntityOwner) anOwner;
  if (theRank < 1 || theRank > NbPicked())
  {
    return anOwner;
  }

  const Standard_Integer anOwnerIdx = myIndexes->Value (theRank);
  const Handle(SelectBasics_EntityOwner)& aStoredOwner = mystored.FindKey (anOwnerIdx);
  anOwner = Handle(SelectMgr_EntityOwner)::DownCast (aStoredOwner);
  return anOwner;
}

//=======================================================================
//function : PickedData
//purpose  :
//=======================================================================
const SelectMgr_SortCriterion& SelectMgr_ViewerSelector::PickedData(const Standard_Integer theRank) const
{
  Standard_OutOfRange_Raise_if (theRank < 1 || theRank > NbPicked(), "SelectMgr_ViewerSelector::PickedData() out of range index");
  const Standard_Integer anOwnerIdx = myIndexes->Value (theRank);
  return mystored.FindFromIndex (anOwnerIdx);
}

//===================================================
//
//       INTERNAL METHODS ....
//
//==================================================

//==================================================
// Function: SetEntitySetBuilder
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::SetEntitySetBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder)
{
  myEntitySetBuilder = theBuilder;
  for (SelectMgr_MapOfObjectSensitives::Iterator aSetIter (myMapOfObjectSensitives); aSetIter.More(); aSetIter.Next())
  {
    aSetIter.ChangeValue()->SetBuilder (myEntitySetBuilder);
  }
}

//==================================================
// Function: Contains
// Purpose :
//==================================================
Standard_Boolean SelectMgr_ViewerSelector::Contains (const Handle(SelectMgr_SelectableObject)& theObject) const
{
  return mySelectableObjects.Contains (theObject);
}

//==================================================
// Function: ActiveModes
// Purpose : return all the  modes with a given state for an object
//==================================================
Standard_Boolean SelectMgr_ViewerSelector::Modes (const Handle(SelectMgr_SelectableObject)& theSelectableObject,
                                                  TColStd_ListOfInteger& theModeList,
                                                  const SelectMgr_StateOfSelection theWantedState) const
{
  Standard_Boolean hasActivatedStates = Contains (theSelectableObject);
  for (theSelectableObject->Init(); theSelectableObject->More(); theSelectableObject->Next())
  {
      if (theWantedState == SelectMgr_SOS_Any)
      {
        theModeList.Append (theSelectableObject->CurrentSelection()->Mode());
      }
      else if (theWantedState == theSelectableObject->CurrentSelection()->GetSelectionState())
      {
        theModeList.Append (theSelectableObject->CurrentSelection()->Mode());
      }
  }

  return hasActivatedStates;
}

//==================================================
// Function: IsActive
// Purpose :
//==================================================
Standard_Boolean SelectMgr_ViewerSelector::IsActive (const Handle(SelectMgr_SelectableObject)& theSelectableObject,
                                                     const Standard_Integer theMode) const
{
  if (!Contains (theSelectableObject))
    return Standard_False;

  for (theSelectableObject->Init(); theSelectableObject->More(); theSelectableObject->Next())
  {
    if (theMode == theSelectableObject->CurrentSelection()->Mode())
    {
      return theSelectableObject->CurrentSelection()->GetSelectionState() == SelectMgr_SOS_Activated;
    }
  }

  return Standard_False;
}

//==================================================
// Function: IsInside
// Purpose :
//==================================================
Standard_Boolean SelectMgr_ViewerSelector::IsInside (const Handle(SelectMgr_SelectableObject)& theSelectableObject,
                                                     const Standard_Integer theMode) const
{
  if (!Contains (theSelectableObject))
    return Standard_False;

  for (theSelectableObject->Init(); theSelectableObject->More(); theSelectableObject->Next())
  {
    if (theMode == theSelectableObject->CurrentSelection()->Mode())
    {
      return theSelectableObject->CurrentSelection()->GetSelectionState() != SelectMgr_SOS_Unknown;
    }
  }

  return Standard_False;
}


//=======================================================================
//function : Status
//purpose  :
//=======================================================================

SelectMgr_StateOfSelection SelectMgr_ViewerSelector::Status (const Handle(SelectMgr_Selection)& theSelection) const
{
  return theSelection->GetSelectionState();
}

//==================================================
// Function: Status
// Purpose : gives Information about selectors
//==================================================

TCollection_AsciiString SelectMgr_ViewerSelector::Status (const Handle(SelectMgr_SelectableObject)& theSelectableObject) const
{
  TCollection_AsciiString aStatus ("Status Object :\n\t");

  for (theSelectableObject->Init(); theSelectableObject->More(); theSelectableObject->Next())
  {
    if (theSelectableObject->CurrentSelection()->GetSelectionState() != SelectMgr_SOS_Unknown)
    {
      aStatus = aStatus + "Mode " +
        TCollection_AsciiString (theSelectableObject->CurrentSelection()->Mode()) +
        " present - ";
      if (theSelectableObject->CurrentSelection()->GetSelectionState() == SelectMgr_SOS_Activated)
      {
        aStatus = aStatus + " Active \n\t";
      }
      else
      {
        aStatus = aStatus + " Inactive \n\t";
      }
    }
  }

  if (!Contains (theSelectableObject))
  {
    aStatus = aStatus + "Not Present in the selector\n\n";
  }

  return aStatus;
}

//=======================================================================
//function : SortResult
//purpose  :  there is a certain number of entities ranged by criteria
//            (depth, size, priority, mouse distance from borders or
//            CDG of the detected primitive. Parsing :
//             maximum priorities .
//             then a reasonable compromise between depth and distance...
// finally the ranges are stored in myindexes depending on the parsing.
// so, it is possible to only read
//=======================================================================
void SelectMgr_ViewerSelector::SortResult()
{
  if(mystored.IsEmpty()) return;

  const Standard_Integer anExtent = mystored.Extent();
  if(myIndexes.IsNull() || anExtent != myIndexes->Length())
    myIndexes = new TColStd_HArray1OfInteger (1, anExtent);

  TColStd_Array1OfInteger& anIndexArray = myIndexes->ChangeArray1();
  for (Standard_Integer anIndexIter = 1; anIndexIter <= anExtent; ++anIndexIter)
  {
    anIndexArray.SetValue (anIndexIter, anIndexIter);
  }
  std::sort (anIndexArray.begin(), anIndexArray.end(), CompareResults (mystored));
}

//=======================================================================
//function : HasDepthClipping
//purpose  : Stub
//=======================================================================
Standard_Boolean SelectMgr_ViewerSelector::HasDepthClipping (const Handle(SelectMgr_EntityOwner)& /*theOwner*/) const
{
  return Standard_False;
}

//=======================================================================
// function : AddSelectableObject
// purpose  : Adds new object to the map of selectable objects
//=======================================================================
void SelectMgr_ViewerSelector::AddSelectableObject (const Handle(SelectMgr_SelectableObject)& theObject)
{
  if (!myMapOfObjectSensitives.IsBound (theObject))
  {
    mySelectableObjects.Append (theObject);
    Handle(SelectMgr_SensitiveEntitySet) anEntitySet = new SelectMgr_SensitiveEntitySet (myEntitySetBuilder);
    myMapOfObjectSensitives.Bind (theObject, anEntitySet);
  }
}

//=======================================================================
// function : AddSelectionToObject
// purpose  : Adds new selection to the object and builds its BVH tree
//=======================================================================
void SelectMgr_ViewerSelector::AddSelectionToObject (const Handle(SelectMgr_SelectableObject)& theObject,
                                                     const Handle(SelectMgr_Selection)& theSelection)
{
  if (Handle(SelectMgr_SensitiveEntitySet)* anEntitySet = myMapOfObjectSensitives.ChangeSeek (theObject))
  {
    (*anEntitySet)->Append (theSelection);
    (*anEntitySet)->BVH();
  }
  else
  {
    AddSelectableObject (theObject);
    AddSelectionToObject (theObject, theSelection);
  }
}

//=======================================================================
// function : MoveSelectableObject
// purpose  :
//=======================================================================
void SelectMgr_ViewerSelector::MoveSelectableObject (const Handle(SelectMgr_SelectableObject)& theObject)
{
  mySelectableObjects.ChangeSubset (theObject);
}

//=======================================================================
// function : RemoveSelectableObject
// purpose  : Removes selectable object from map of selectable ones
//=======================================================================
void SelectMgr_ViewerSelector::RemoveSelectableObject (const Handle(SelectMgr_SelectableObject)& theObject)
{
  Handle(SelectMgr_SelectableObject) anObj = theObject;
  if (myMapOfObjectSensitives.UnBind (theObject))
  {
    mySelectableObjects.Remove (theObject);
  }
}

//=======================================================================
// function : RemoveSelectionOfObject
// purpose  : Removes selection of the object and marks its BVH tree
//            for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RemoveSelectionOfObject (const Handle(SelectMgr_SelectableObject)& theObject,
                                                        const Handle(SelectMgr_Selection)& theSelection)
{
  if (Handle(SelectMgr_SensitiveEntitySet)* anEntitySet = myMapOfObjectSensitives.ChangeSeek (theObject))
  {
    (*anEntitySet)->Remove (theSelection);
  }
}

//=======================================================================
// function : RebuildObjectsTree
// purpose  : Marks BVH of selectable objects for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RebuildObjectsTree (const Standard_Boolean theIsForce)
{
  mySelectableObjects.MarkDirty();

  if (theIsForce)
  {
    Standard_Integer aViewportWidth, aViewportHeight;
    mySelectingVolumeMgr.WindowSize (aViewportWidth, aViewportHeight);

    Standard_Integer aWidth;
    Standard_Integer aHeight;
    mySelectingVolumeMgr.WindowSize (aWidth, aHeight);
    mySelectableObjects.UpdateBVH (mySelectingVolumeMgr.Camera(),
                                   mySelectingVolumeMgr.ProjectionMatrix(),
                                   mySelectingVolumeMgr.WorldViewMatrix(),
                                   mySelectingVolumeMgr.WorldViewProjState(),
                                   aWidth, aHeight);
  }
}

//=======================================================================
// function : RebuildSensitivesTree
// purpose  : Marks BVH of sensitive entities of particular selectable
//            object for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RebuildSensitivesTree (const Handle(SelectMgr_SelectableObject)& theObject,
                                                      const Standard_Boolean theIsForce)
{
  if (!Contains (theObject))
    return;

  Handle(SelectMgr_SensitiveEntitySet)& anEntitySet = myMapOfObjectSensitives.ChangeFind (theObject);
  anEntitySet->MarkDirty();

  if (theIsForce)
  {
    anEntitySet->BVH();
  }
}

//=======================================================================
// function : resetSelectionActivationStatus
// purpose  : Marks all added sensitive entities of all objects as
//            non-selectable
//=======================================================================
void SelectMgr_ViewerSelector::ResetSelectionActivationStatus()
{
  for (SelectMgr_MapOfObjectSensitivesIterator aSensitivesIter (myMapOfObjectSensitives); aSensitivesIter.More(); aSensitivesIter.Next())
  {
    Handle(SelectMgr_SensitiveEntitySet)& anEntitySet = aSensitivesIter.ChangeValue();
    const Standard_Integer anEntitiesNb = anEntitySet->Size();
    for (Standard_Integer anIdx = 0; anIdx < anEntitiesNb; ++anIdx)
    {
      anEntitySet->GetSensitiveById (anIdx)->ResetSelectionActiveStatus();
    }
  }
}

//=======================================================================
// function : DetectedEntity
// purpose  : Returns sensitive entity that was detected during the
//            previous run of selection algorithm
//=======================================================================
const Handle(SelectBasics_SensitiveEntity)& SelectMgr_ViewerSelector::DetectedEntity() const
{
  const Standard_Integer aRankInMap = myIndexes->Value(myCurRank);
  return mystored.FindFromIndex (aRankInMap).Entity;
}

//=======================================================================
// function : ActiveOwners
// purpose  : Returns the list of active entity owners
//=======================================================================
void SelectMgr_ViewerSelector::ActiveOwners (NCollection_List<Handle(SelectBasics_EntityOwner)>& theOwners) const
{
  for (SelectMgr_MapOfObjectSensitivesIterator anIter (myMapOfObjectSensitives); anIter.More(); anIter.Next())
  {
    const Handle(SelectMgr_SensitiveEntitySet)& anEntitySet = anIter.Value();
    const Standard_Integer anEntitiesNb = anEntitySet->Size();
    for (Standard_Integer anIdx = 0; anIdx < anEntitiesNb; ++anIdx)
    {
      const Handle(SelectMgr_SensitiveEntity)& aSensitive = anEntitySet->GetSensitiveById (anIdx);
      if (aSensitive->IsActiveForSelection())
      {
        theOwners.Append (aSensitive->BaseSensitive()->OwnerId());
      }
    }
  }
}

//=======================================================================
//function : AllowOverlapDetection
//purpose  : Sets the detection type: if theIsToAllow is false,
//           only fully included sensitives will be detected, otherwise
//           the algorithm will mark both included and overlapped entities
//           as matched
//=======================================================================
void SelectMgr_ViewerSelector::AllowOverlapDetection (const Standard_Boolean theIsToAllow)
{
  mySelectingVolumeMgr.AllowOverlapDetection (theIsToAllow);
}

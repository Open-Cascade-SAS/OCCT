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
#include <Select3D_SensitiveEntity.hxx>
#include <SelectBasics_PickResult.hxx>
#include <SelectMgr.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_FrustumBuilder.hxx>
#include <SelectMgr_SelectionImageFiller.hxx>
#include <SelectMgr_SensitiveEntitySet.hxx>
#include <SelectMgr_SortCriterion.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <V3d_View.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_ViewerSelector, Standard_Transient)

namespace
{
  //! Comparison operator for sorting selection results
  class CompareResults
  {
  public:

    CompareResults (const SelectMgr_IndexedDataMapOfOwnerCriterion& theMapOfCriterion,
                    bool theToPreferClosest)
    : myMapOfCriterion (&theMapOfCriterion),
      myToPreferClosest (theToPreferClosest) {}

    bool operator() (int theLeft, int theRight) const
    {
      const SelectMgr_SortCriterion& anElemLeft  = myMapOfCriterion->FindFromIndex (theLeft);
      const SelectMgr_SortCriterion& anElemRight = myMapOfCriterion->FindFromIndex (theRight);
      if (myToPreferClosest)
      {
        return anElemLeft.IsCloserDepth (anElemRight);
      }
      else
      {
        return anElemLeft.IsHigherPriority (anElemRight);
      }
    }

  private:
    const SelectMgr_IndexedDataMapOfOwnerCriterion* myMapOfCriterion;
    bool myToPreferClosest;
  };

  static const Graphic3d_Mat4d SelectMgr_ViewerSelector_THE_IDENTITY_MAT;
}

//=======================================================================
// function : updatePoint3d
// purpose  :
//=======================================================================
void SelectMgr_ViewerSelector::updatePoint3d (SelectMgr_SortCriterion& theCriterion,
                                              const SelectBasics_PickResult& thePickResult,
                                              const occ::handle<Select3D_SensitiveEntity>& theEntity,
                                              const gp_GTrsf& theInversedTrsf,
                                              const SelectMgr_SelectingVolumeManager& theMgr) const
{
  if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point)
  {
    return;
  }

  bool hasNormal = false;
  if (thePickResult.HasPickedPoint())
  {
    theCriterion.Point  = thePickResult.PickedPoint();
    theCriterion.Normal = thePickResult.SurfaceNormal();
    const float aNormLen2 = theCriterion.Normal.SquareModulus();
    if (aNormLen2 > ShortRealEpsilon())
    {
      hasNormal = true;
      theCriterion.Normal *= 1.0f / sqrtf (aNormLen2);
    }
  }
  else if (!thePickResult.IsValid())
  {
    theCriterion.Point = thePickResult.PickedPoint();
    return;
  }
  else
  {
    theCriterion.Point = theMgr.DetectedPoint (theCriterion.Depth);
  }

  gp_GTrsf anInvTrsf = theInversedTrsf;
  if (theCriterion.Entity->HasInitLocation())
  {
    anInvTrsf = theCriterion.Entity->InvInitLocation() * anInvTrsf;
  }
  if (anInvTrsf.Form() != gp_Identity)
  {
    const gp_GTrsf anInvInvTrsd = anInvTrsf.Inverted();
    anInvInvTrsd.Transforms (theCriterion.Point.ChangeCoord());
    if (hasNormal)
    {
      Graphic3d_Mat4d aMat4;
      anInvInvTrsd.GetMat4 (aMat4);
      const Graphic3d_Vec4d aNormRes = aMat4 * Graphic3d_Vec4d (Graphic3d_Vec3d (theCriterion.Normal), 0.0);
      theCriterion.Normal = Graphic3d_Vec3 (aNormRes.xyz());
    }
  }

  const double aSensFactor = myDepthTolType == SelectMgr_TypeOfDepthTolerance_SensitivityFactor ? theEntity->SensitivityFactor() : myDepthTolerance;
  switch (myDepthTolType)
  {
    case SelectMgr_TypeOfDepthTolerance_Uniform:
    {
      theCriterion.Tolerance = myDepthTolerance;
      break;
    }
    case SelectMgr_TypeOfDepthTolerance_UniformPixels:
    case SelectMgr_TypeOfDepthTolerance_SensitivityFactor:
    {
      if (theMgr.Camera().IsNull())
      {
        // fallback for an arbitrary projection matrix
        theCriterion.Tolerance = aSensFactor / 33.0;
      }
      else if (theMgr.Camera()->IsOrthographic())
      {
        theCriterion.Tolerance = myCameraScale * aSensFactor;
      }
      else
      {
        const double aDistFromEye = Abs ((theCriterion.Point.XYZ() - myCameraEye.XYZ()).Dot (myCameraDir.XYZ()));
        theCriterion.Tolerance = aDistFromEye * myCameraScale * aSensFactor;
      }
      break;
    }
  }
}

//==================================================
// Function: Initialize
// Purpose :
//==================================================
SelectMgr_ViewerSelector::SelectMgr_ViewerSelector()
: myDepthTolerance (0.0),
  myDepthTolType (SelectMgr_TypeOfDepthTolerance_SensitivityFactor),
  myToPreferClosest (true),
  myCameraScale (1.0),
  myToPrebuildBVH (false),
  myIsSorted (false),
  myIsLeftChildQueuedFirst (false)
{
  myEntitySetBuilder = new BVH_BinnedBuilder<double, 3, 4> (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth, true);
}

//=======================================================================
// Function: SetPixelTolerance
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector::SetPixelTolerance (const int theTolerance)
{
  if (myTolerances.Tolerance() == theTolerance)
  {
    return;
  }
  if (theTolerance < 0)
  {
    myTolerances.ResetDefaults();
  }
  else
  {
    myTolerances.SetCustomTolerance (theTolerance);
  }
}

//==================================================
// Function: Activate
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::Activate (const occ::handle<SelectMgr_Selection>& theSelection)
{
  for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter (theSelection->Entities()); aSelEntIter.More(); aSelEntIter.Next())
  {
    aSelEntIter.Value()->SetActiveForSelection();
  }

  if (theSelection->GetSelectionState() != SelectMgr_SOS_Activated)
  {
    theSelection->SetSelectionState (SelectMgr_SOS_Activated);

    myTolerances.Add (theSelection->Sensitivity());
  }
}

//==================================================
// Function: Deactivate
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::Deactivate (const occ::handle<SelectMgr_Selection>& theSelection)
{
  for (NCollection_Vector<occ::handle<SelectMgr_SensitiveEntity>>::Iterator aSelEntIter (theSelection->Entities()); aSelEntIter.More(); aSelEntIter.Next())
  {
    aSelEntIter.Value()->ResetSelectionActiveStatus();
  }

  if (theSelection->GetSelectionState() == SelectMgr_SOS_Activated)
  {
    theSelection->SetSelectionState (SelectMgr_SOS_Deactivated);

    myTolerances.Decrement (theSelection->Sensitivity());
  }
}

//=======================================================================
// function: isToScaleFrustum
// purpose : Checks if the entity given requires to scale current selecting frustum
//=======================================================================
bool SelectMgr_ViewerSelector::isToScaleFrustum (const occ::handle<Select3D_SensitiveEntity>& theEntity)
{
  return mySelectingVolumeMgr.IsScalableActiveVolume()
    && sensitivity (theEntity) < myTolerances.Tolerance();
}

//=======================================================================
// function: sensitivity
// purpose : In case if custom tolerance is set, this method will return sum of entity
//           sensitivity and custom tolerance.
//=======================================================================
int SelectMgr_ViewerSelector::sensitivity (const occ::handle<Select3D_SensitiveEntity>& theEntity) const
{
  return myTolerances.IsCustomTolSet() ?
    theEntity->SensitivityFactor() + myTolerances.CustomTolerance() : theEntity->SensitivityFactor();
}

//=======================================================================
// function: checkOverlap
// purpose : Internal function that checks if a particular sensitive
//           entity theEntity overlaps current selecting volume precisely
//=======================================================================
void SelectMgr_ViewerSelector::checkOverlap (const occ::handle<Select3D_SensitiveEntity>& theEntity,
                                             const gp_GTrsf& theInversedTrsf,
                                             SelectMgr_SelectingVolumeManager& theMgr)
{
  const occ::handle<SelectMgr_EntityOwner>& anOwner = theEntity->OwnerId();
  occ::handle<SelectMgr_SelectableObject> aSelectable = !anOwner.IsNull() ? anOwner->Selectable() : occ::handle<SelectMgr_SelectableObject>();
  SelectBasics_PickResult aPickResult;
  const bool isMatched = theEntity->Matches(theMgr, aPickResult);
  if (!isMatched
    || anOwner.IsNull())
  {
    return;
  }

  SelectMgr_SortCriterion aCriterion;
  myZLayerOrderMap.Find (!aSelectable.IsNull() ? aSelectable->ZLayer() : Graphic3d_ZLayerId_Default, aCriterion.ZLayerPosition);
  aCriterion.Entity    = theEntity;
  aCriterion.Priority  = anOwner->Priority();
  aCriterion.Depth     = aPickResult.Depth();
  aCriterion.MinDist   = aPickResult.DistToGeomCenter();

  if (SelectMgr_SortCriterion* aPrevCriterion = mystored.ChangeSeek (anOwner))
  {
    ++aPrevCriterion->NbOwnerMatches;
    aCriterion.NbOwnerMatches = aPrevCriterion->NbOwnerMatches;
    if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Box)
    {
      updatePoint3d (aCriterion, aPickResult, theEntity, theInversedTrsf, theMgr);
      if (aCriterion.IsCloserDepth (*aPrevCriterion))
      {
        *aPrevCriterion = aCriterion;
      }
    }
  }
  else
  {
    aCriterion.NbOwnerMatches = 1;
    updatePoint3d (aCriterion, aPickResult, theEntity, theInversedTrsf, theMgr);
    mystored.Add (anOwner, aCriterion);
  }
}

//=======================================================================
// Function: updateZLayers
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector::updateZLayers (const occ::handle<V3d_View>& theView)
{
  myZLayerOrderMap.Clear();
  TColStd_SequenceOfInteger aZLayers;
  theView->Viewer()->GetAllZLayers (aZLayers);
  int aPos = 0;
  bool isPrevDepthWrite = true;
  for (TColStd_SequenceOfInteger::Iterator aLayerIter (aZLayers); aLayerIter.More(); aLayerIter.Next())
  {
    Graphic3d_ZLayerSettings aSettings = theView->Viewer()->ZLayerSettings (aLayerIter.Value());
    if (aSettings.ToClearDepth()
     || isPrevDepthWrite != aSettings.ToEnableDepthWrite())
    {
      ++aPos;
    }
    isPrevDepthWrite = aSettings.ToEnableDepthWrite();
    myZLayerOrderMap.Bind (aLayerIter.Value(), aPos);
  }
}

//=======================================================================
// function: computeFrustum
// purpose :
//=======================================================================
void SelectMgr_ViewerSelector::computeFrustum (const occ::handle<Select3D_SensitiveEntity>& theEnt,
                                               const SelectMgr_SelectingVolumeManager& theMgrGlobal,
                                               const SelectMgr_SelectingVolumeManager& theMgrObject,
                                               const gp_GTrsf& theInvTrsf,
                                               NCollection_DataMap<int, SelectMgr_SelectingVolumeManager>& theCachedMgrs,
                                               SelectMgr_SelectingVolumeManager& theResMgr)
{
  int aScale = isToScaleFrustum (theEnt) ? sensitivity (theEnt) : 1;
  const gp_GTrsf aTrsfMtr = theEnt->HasInitLocation() ? theEnt->InvInitLocation() * theInvTrsf : theInvTrsf;
  const bool toScale = aScale != 1;
  const bool toTransform = aTrsfMtr.Form() != gp_Identity;
  if (toScale && toTransform)
  {
    theResMgr = theMgrGlobal.ScaleAndTransform (aScale, aTrsfMtr, NULL);
    theResMgr.SetViewClipping (theMgrObject);
  }
  else if (toScale)
  {
    if (!theCachedMgrs.Find (aScale, theResMgr))
    {
      theResMgr = theMgrGlobal.ScaleAndTransform (aScale, gp_Trsf(), NULL);
      theCachedMgrs.Bind (aScale, theResMgr);
    }
    theResMgr.SetViewClipping (theMgrObject);
  }
  else if (toTransform)
  {
    theResMgr = theMgrGlobal.ScaleAndTransform (1, aTrsfMtr, NULL);
    theResMgr.SetViewClipping (theMgrObject);
  }
  else
  {
    theResMgr = theMgrObject;
  }
}

//=======================================================================
// function: traverseObject
// purpose : Internal function that checks if there is possible overlap
//           between some entity of selectable object theObject and
//           current selecting volume
//=======================================================================
void SelectMgr_ViewerSelector::traverseObject (const occ::handle<SelectMgr_SelectableObject>& theObject,
                                               const SelectMgr_SelectingVolumeManager& theMgr,
                                               const occ::handle<Graphic3d_Camera>& theCamera,
                                               const Graphic3d_Mat4d& theProjectionMat,
                                               const Graphic3d_Mat4d& theWorldViewMat,
                                               const Graphic3d_Vec2i& theWinSize)
{
  occ::handle<SelectMgr_SensitiveEntitySet>& anEntitySet = myMapOfObjectSensitives.ChangeFind (theObject);
  if (anEntitySet->Size() == 0)
  {
    return;
  }

  const bool hasEntityTrsfPers = anEntitySet->HasEntityWithPersistence()
                             && !theCamera.IsNull();
  const bool hasEntityFlipped = anEntitySet->HasEntityWithFlipping();

  const opencascade::handle<BVH_Tree<double, 3> >& aSensitivesTree = anEntitySet->BVH();
  gp_GTrsf aInversedTrsf;
  if (theObject->HasTransformation() || !theObject->TransformPersistence().IsNull())
  {
    if (theObject->TransformPersistence().IsNull())
    {
      aInversedTrsf = theObject->InversedTransformation();
    }
    else
    {
      if (theCamera.IsNull())
      {
        return;
      }

      const Graphic3d_Mat4d aMat = theObject->TransformPersistence()->Compute (theCamera,
                                                                               theProjectionMat, theWorldViewMat,
                                                                               theWinSize.x(), theWinSize.y());
      gp_GTrsf aTPers;
      aTPers.SetMat4 (aMat);
      aInversedTrsf = (aTPers * gp_GTrsf (theObject->Transformation())).Inverted();
    }
  }

  SelectMgr_SelectingVolumeManager aMgr = aInversedTrsf.Form() != gp_Identity
                                        ? theMgr.ScaleAndTransform (1, aInversedTrsf, NULL)
                                        : theMgr;
  if (!hasEntityTrsfPers
   && !hasEntityFlipped
   && !aMgr.OverlapsBox (aSensitivesTree->MinPoint (0),
                         aSensitivesTree->MaxPoint (0)))
  {
    return;
  }

  if (!theObject->ClipPlanes().IsNull()
    && theObject->ClipPlanes()->ToOverrideGlobal())
  {
    aMgr.SetViewClipping (occ::handle<Graphic3d_SequenceOfHClipPlane>(), theObject->ClipPlanes(), &theMgr);
  }
  else if (!theObject->TransformPersistence().IsNull())
  {
    if (theObject->TransformPersistence()->IsZoomOrRotate()
    && !theMgr.ViewClipping().IsNull())
    {
      // Zoom/rotate persistence object lives in two worlds at the same time.
      // Global clipping planes can not be trivially applied without being converted
      // into local space of transformation persistence object.
      // As more simple alternative - just clip entire object by its anchor point defined in the world space.
      const gp_Pnt anAnchor = theObject->TransformPersistence()->AnchorPoint();
      for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*theMgr.ViewClipping()); aPlaneIt.More(); aPlaneIt.Next())
      {
        const occ::handle<Graphic3d_ClipPlane>& aPlane = aPlaneIt.Value();
        if (!aPlane->IsOn())
        {
          continue;
        }

        const Graphic3d_Vec4d aCheckPnt (anAnchor.X(), anAnchor.Y(), anAnchor.Z(), 1.0);
        if (aPlane->ProbePoint (aCheckPnt) == Graphic3d_ClipState_Out)
        {
          return;
        }
      }
    }

    aMgr.SetViewClipping (occ::handle<Graphic3d_SequenceOfHClipPlane>(), theObject->ClipPlanes(), &theMgr);
  }
  else if (!theObject->ClipPlanes().IsNull()
        && !theObject->ClipPlanes()->IsEmpty())
  {
    aMgr.SetViewClipping (theMgr.ViewClipping(), theObject->ClipPlanes(), &theMgr);
  }

  if (!theMgr.ViewClipping().IsNull() &&
      theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Box)
  {
    Graphic3d_BndBox3d aBBox (aSensitivesTree->MinPoint (0), aSensitivesTree->MaxPoint (0));
    // If box selection is active, and the whole sensitive tree is out of the clip planes
    // selection is empty for this object
    const occ::handle<Graphic3d_SequenceOfHClipPlane>& aViewPlanes = theMgr.ViewClipping();

    for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aViewPlanes); aPlaneIt.More(); aPlaneIt.Next())
    {
      const occ::handle<Graphic3d_ClipPlane>& aPlane = aPlaneIt.Value();
      if (!aPlane->IsOn())
      {
        continue;
      }

      Graphic3d_ClipState aState = aPlane->ProbeBox (aBBox);
      if (aState == Graphic3d_ClipState_Out) // do not process only whole trees, next check on the tree node
      {
        return;
      }
    }
  }

  const int aFirstStored = mystored.Extent() + 1;

  int aStack[BVH_Constants_MaxTreeDepth];
  int aHead = -1;
  int aNode = 0; // a root node
  NCollection_DataMap<int, SelectMgr_SelectingVolumeManager> aScaledTrnsfFrustums;
  SelectMgr_SelectingVolumeManager aTmpMgr;
  for (;;)
  {
    if (!aSensitivesTree->IsOuter (aNode))
    {
      const int aLeftChildIdx  = aSensitivesTree->Child<0> (aNode);
      const int aRightChildIdx = aSensitivesTree->Child<1> (aNode);
      const bool isLeftChildIn  = hasEntityTrsfPers
                                           || hasEntityFlipped
                                           || aMgr.OverlapsBox (aSensitivesTree->MinPoint (aLeftChildIdx),
                                                                aSensitivesTree->MaxPoint (aLeftChildIdx));
      const bool isRightChildIn = hasEntityTrsfPers
                                           || hasEntityFlipped
                                           || aMgr.OverlapsBox (aSensitivesTree->MinPoint (aRightChildIdx),
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
      bool aClipped = false;
      if (!theMgr.ViewClipping().IsNull() &&
          theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Box)
      {
        Graphic3d_BndBox3d aBBox (aSensitivesTree->MinPoint (aNode), aSensitivesTree->MaxPoint (aNode));
        // If box selection is active, and the whole sensitive tree is out of the clip planes
        // selection is empty for this object
        const occ::handle<Graphic3d_SequenceOfHClipPlane>& aViewPlanes = theMgr.ViewClipping();

        for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (*aViewPlanes); aPlaneIt.More(); aPlaneIt.Next())
        {
          const occ::handle<Graphic3d_ClipPlane>& aPlane = aPlaneIt.Value();
          if (!aPlane->IsOn())
          {
            continue;
          }
          Graphic3d_ClipState aState = aPlane->ProbeBox (aBBox);
          if (aState == Graphic3d_ClipState_Out)
          {
            aClipped = true;
            break;
          }
          if (aState == Graphic3d_ClipState_On && !mySelectingVolumeMgr.IsOverlapAllowed()) // partially clipped
          {
            if (aPlane->ProbeBoxTouch (aBBox))
              continue;
            aClipped = true;
            break;
          }
        }
      }
      if (!aClipped)
      {
        const int aStartIdx = aSensitivesTree->BegPrimitive (aNode);
        const int anEndIdx  = aSensitivesTree->EndPrimitive (aNode);
        for (int anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
        {
          const occ::handle<SelectMgr_SensitiveEntity>& aSensitive = anEntitySet->GetSensitiveById (anIdx);
          if (!aSensitive->IsActiveForSelection())
          {
            continue;
          }

          const occ::handle<Select3D_SensitiveEntity>& anEnt = aSensitive->BaseSensitive();

          gp_GTrsf aInvSensTrsf = aInversedTrsf;
          if (!anEnt->TransformPersistence().IsNull())
          {
            if (theCamera.IsNull())
            {
              continue;
            }
            const Graphic3d_Mat4d aMat = anEnt->TransformPersistence()->Compute (theCamera,
                                                                                 theProjectionMat, theWorldViewMat,
                                                                                 theWinSize.x(), theWinSize.y());
            gp_GTrsf aTPers;
            aTPers.SetMat4 (aMat);
            aInvSensTrsf = (aTPers * gp_GTrsf(theObject->Transformation())).Inverted();
          }

          gp_GTrsf aFlippingTrsf;
          if (!anEnt->Flipper().IsNull())
          {
            const Graphic3d_Mat4d aMat = anEnt->Flipper()->Compute (theWorldViewMat);
            aFlippingTrsf.SetMat4 (aMat);
          }

          gp_GTrsf aInvFlippingAndPers = aFlippingTrsf * aInvSensTrsf;
          computeFrustum (anEnt, theMgr, aMgr, aInvFlippingAndPers, aScaledTrnsfFrustums, aTmpMgr);
          checkOverlap (anEnt, aInvFlippingAndPers, aTmpMgr);
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

  // in case of Box/Polyline selection - keep only Owners having all Entities detected
  if (mySelectingVolumeMgr.IsOverlapAllowed()
  || (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Box
   && theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Polyline))
  {
    return;
  }

  for (int aStoredIter = mystored.Extent(); aStoredIter >= aFirstStored; --aStoredIter)
  {
    const SelectMgr_SortCriterion& aCriterion = mystored.FindFromIndex (aStoredIter);
    const occ::handle<SelectMgr_EntityOwner>& anOwner = aCriterion.Entity->OwnerId();
    int aNbOwnerEntities = 0;
    anEntitySet->Owners().Find (anOwner, aNbOwnerEntities);
    if (aNbOwnerEntities > aCriterion.NbOwnerMatches)
    {
      mystored.RemoveFromIndex (aStoredIter);
    }
  }
}

//=======================================================================
// function: TraverseSensitives
// purpose : Traverses BVH containing all added selectable objects and
//           finds candidates for further search of overlap
//=======================================================================
void SelectMgr_ViewerSelector::TraverseSensitives (const int theViewId)
{
  SelectMgr_BVHThreadPool::Sentry aSentry (myBVHThreadPool);

  mystored.Clear();
  myIsSorted = false;

  Graphic3d_Vec2i aWinSize;
  mySelectingVolumeMgr.WindowSize (aWinSize.x(), aWinSize.y());
  const double aPixelSize = aWinSize.x() > 0 && aWinSize.y() > 0
                          ? Max (1.0 / aWinSize.x(), 1.0 / aWinSize.y())
                          : 1.0;

  const occ::handle<Graphic3d_Camera>& aCamera = mySelectingVolumeMgr.Camera();
  Graphic3d_Mat4d aProjectionMat, aWorldViewMat;
  Graphic3d_WorldViewProjState aViewState;
  if (!aCamera.IsNull())
  {
    aProjectionMat = aCamera->ProjectionMatrix();
    aWorldViewMat = aCamera->OrientationMatrix();
    aViewState = aCamera->WorldViewProjState();

    myCameraEye = aCamera->Eye().XYZ();
    myCameraDir = aCamera->Direction().XYZ();
  }
  mySelectableObjects.UpdateBVH (aCamera, aWinSize);

  for (int aBVHSetIt = 0; aBVHSetIt < SelectMgr_SelectableObjectSet::BVHSubsetNb; ++aBVHSetIt)
  {
    const SelectMgr_SelectableObjectSet::BVHSubset aBVHSubset = (SelectMgr_SelectableObjectSet::BVHSubset )aBVHSetIt;
    if (mySelectableObjects.IsEmpty (aBVHSubset))
    {
      continue;
    }
    if (aCamera.IsNull()
     && aBVHSubset != SelectMgr_SelectableObjectSet::BVHSubset_3d)
    {
      continue;
    }

    SelectMgr_SelectingVolumeManager aMgr;

    // for 2D space selection transform selecting volumes to perform overlap testing
    // directly in camera's eye space omitting the camera position, which is not
    // needed there at all
    if (aBVHSubset == SelectMgr_SelectableObjectSet::BVHSubset_2dPersistent 
     || aBVHSubset == SelectMgr_SelectableObjectSet::BVHSubset_ortho2dPersistent)
    {
      gp_GTrsf aTFrustum;
      aTFrustum.SetValue (1, 1, aWorldViewMat.GetValue (0, 0));
      aTFrustum.SetValue (1, 2, aWorldViewMat.GetValue (0, 1));
      aTFrustum.SetValue (1, 3, aWorldViewMat.GetValue (0, 2));
      aTFrustum.SetValue (2, 1, aWorldViewMat.GetValue (1, 0));
      aTFrustum.SetValue (2, 2, aWorldViewMat.GetValue (1, 1));
      aTFrustum.SetValue (2, 3, aWorldViewMat.GetValue (1, 2));
      aTFrustum.SetValue (3, 1, aWorldViewMat.GetValue (2, 0));
      aTFrustum.SetValue (3, 2, aWorldViewMat.GetValue (2, 1));
      aTFrustum.SetValue (3, 3, aWorldViewMat.GetValue (2, 2));
      aTFrustum.SetTranslationPart (gp_XYZ (aWorldViewMat.GetValue (0, 3),
                                            aWorldViewMat.GetValue (1, 3),
                                            aWorldViewMat.GetValue (2, 3)));

      // define corresponding frustum builder parameters for 2d persistence.
      occ::handle<SelectMgr_FrustumBuilder> aBuilder = new SelectMgr_FrustumBuilder();
      occ::handle<Graphic3d_Camera> aNewCamera = new Graphic3d_Camera();
      aNewCamera->CopyMappingData (aCamera);
      aNewCamera->SetIdentityOrientation();
      if (aBVHSubset == SelectMgr_SelectableObjectSet::BVHSubset_ortho2dPersistent)
      {
        aNewCamera->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
      }
      aWorldViewMat = aNewCamera->OrientationMatrix(); // should be identity matrix
      aProjectionMat = aNewCamera->ProjectionMatrix(); // should be the same to aProjectionMat
      aBuilder->SetCamera (aNewCamera);
      aBuilder->SetWindowSize (aWinSize.x(), aWinSize.y());
      aMgr = mySelectingVolumeMgr.ScaleAndTransform (1, aTFrustum, aBuilder);
    }
    else if (aBVHSubset == SelectMgr_SelectableObjectSet::BVHSubset_ortho3dPersistent)
    {
      // define corresponding frustum builder parameters for 3d orthographic persistence.
      occ::handle<SelectMgr_FrustumBuilder> aBuilder = new SelectMgr_FrustumBuilder();
      occ::handle<Graphic3d_Camera> aNewCamera = new Graphic3d_Camera (*aCamera);
      aNewCamera->SetProjectionType (Graphic3d_Camera::Projection_Orthographic);
      aWorldViewMat = aNewCamera->OrientationMatrix(); // should be the same to aWorldViewMat
      aProjectionMat = aNewCamera->ProjectionMatrix(); // should be orthographic projection
      aBuilder->SetCamera (aNewCamera);
      aBuilder->SetWindowSize (aWinSize.x(), aWinSize.y());
      aMgr = mySelectingVolumeMgr.CopyWithBuilder (aBuilder);
    }
    else
    {
      aMgr = mySelectingVolumeMgr;
    }

    if (!aMgr.Camera().IsNull())
    {
      myCameraScale = aMgr.Camera()->IsOrthographic()
                   ? aMgr.Camera()->Scale()
                   : 2.0 * Tan (aMgr.Camera()->FOVy() * M_PI / 360.0);
      myCameraScale *= aPixelSize;
    }

    const opencascade::handle<BVH_Tree<double, 3> >& aBVHTree = mySelectableObjects.BVH (aBVHSubset);

    int aNode = 0;
    if (!aMgr.OverlapsBox (aBVHTree->MinPoint (0), aBVHTree->MaxPoint (0)))
    {
      continue;
    }

    int aStack[BVH_Constants_MaxTreeDepth];
    int aHead = -1;
    for (;;)
    {
      if (!aBVHTree->IsOuter (aNode))
      {
        const int aLeftChildIdx  = aBVHTree->Child<0> (aNode);
        const int aRightChildIdx = aBVHTree->Child<1> (aNode);
        const bool isLeftChildIn  =
          aMgr.OverlapsBox (aBVHTree->MinPoint (aLeftChildIdx), aBVHTree->MaxPoint (aLeftChildIdx));
        const bool isRightChildIn =
          aMgr.OverlapsBox (aBVHTree->MinPoint (aRightChildIdx), aBVHTree->MaxPoint (aRightChildIdx));
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
        const int aStartIdx = aBVHTree->BegPrimitive (aNode);
        const int anEndIdx  = aBVHTree->EndPrimitive (aNode);
        for (int anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
        {
          const occ::handle<SelectMgr_SelectableObject>& aSelObj = mySelectableObjects.GetObjectById (aBVHSubset, anIdx);
          const occ::handle<Graphic3d_ViewAffinity>& aViewAffinity = aSelObj->ViewAffinity();
          if (theViewId == -1 || aViewAffinity->IsVisible (theViewId))
          {
            traverseObject (aSelObj, aMgr, aCamera, aProjectionMat, aWorldViewMat, aWinSize);
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

  SortResult();
}

//==================================================
// Function: ClearPicked
// Purpose :
//==================================================
void SelectMgr_ViewerSelector::ClearPicked()
{
  mystored.Clear();
  myIsSorted = true;
}

//==================================================
// Function: RemovePicked
// Purpose :
//==================================================
bool SelectMgr_ViewerSelector::RemovePicked (const occ::handle<SelectMgr_SelectableObject>& theObject)
{
  if (mystored.IsEmpty()
  || !mySelectableObjects.Contains (theObject))
  {
    return false;
  }

  bool isRemoved = false;
  for (int aPickIter = 1; aPickIter <= mystored.Extent(); ++aPickIter)
  {
    const occ::handle<SelectMgr_EntityOwner>& aStoredOwner = mystored.FindKey (aPickIter);
    if (!aStoredOwner.IsNull()
      && aStoredOwner->IsSameSelectable (theObject))
    {
      mystored.RemoveFromIndex (aPickIter);
      --aPickIter;
      isRemoved = true;
    }
  }
  if (isRemoved)
  {
    myIsSorted = false;
  }
  return isRemoved;
}

//=======================================================================
//function : Picked
//purpose  :
//=======================================================================
occ::handle<SelectMgr_EntityOwner> SelectMgr_ViewerSelector::Picked (const int theRank) const
{
  if (theRank < 1 || theRank > NbPicked())
  {
    return occ::handle<SelectMgr_EntityOwner>();
  }

  if (!myIsSorted)
  {
    SortResult();
  }

  const int anOwnerIdx = myIndexes.Value (theRank);
  const occ::handle<SelectMgr_EntityOwner>& aStoredOwner = mystored.FindKey (anOwnerIdx);
  return aStoredOwner;
}

//=======================================================================
//function : PickedData
//purpose  :
//=======================================================================
const SelectMgr_SortCriterion& SelectMgr_ViewerSelector::PickedData(const int theRank) const
{
  Standard_OutOfRange_Raise_if (theRank < 1 || theRank > NbPicked(), "SelectMgr_ViewerSelector::PickedData() out of range index");
  if (!myIsSorted)
  {
    SortResult();
  }

  const int anOwnerIdx = myIndexes.Value (theRank);
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
void SelectMgr_ViewerSelector::SetEntitySetBuilder (const occ::handle<Select3D_BVHBuilder3d>& theBuilder)
{
  myEntitySetBuilder = theBuilder;
  for (NCollection_DataMap<occ::handle<SelectMgr_SelectableObject>, occ::handle<SelectMgr_SensitiveEntitySet>>::Iterator aSetIter (myMapOfObjectSensitives); aSetIter.More(); aSetIter.Next())
  {
    aSetIter.ChangeValue()->SetBuilder (myEntitySetBuilder);
  }
}

//==================================================
// Function: Contains
// Purpose :
//==================================================
bool SelectMgr_ViewerSelector::Contains (const occ::handle<SelectMgr_SelectableObject>& theObject) const
{
  return mySelectableObjects.Contains (theObject);
}

//==================================================
// Function: ActiveModes
// Purpose : return all the  modes with a given state for an object
//==================================================
bool SelectMgr_ViewerSelector::Modes (const occ::handle<SelectMgr_SelectableObject>& theSelectableObject,
                                                  TColStd_ListOfInteger& theModeList,
                                                  const SelectMgr_StateOfSelection theWantedState) const
{
  bool hasActivatedStates = Contains (theSelectableObject);
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theSelectableObject->Selections()); aSelIter.More(); aSelIter.Next())
  {
    if (theWantedState == SelectMgr_SOS_Any)
    {
      theModeList.Append (aSelIter.Value()->Mode());
    }
    else if (theWantedState == aSelIter.Value()->GetSelectionState())
    {
      theModeList.Append (aSelIter.Value()->Mode());
    }
  }

  return hasActivatedStates;
}

//==================================================
// Function: IsActive
// Purpose :
//==================================================
bool SelectMgr_ViewerSelector::IsActive (const occ::handle<SelectMgr_SelectableObject>& theSelectableObject,
                                                     const int theMode) const
{
  if (!Contains (theSelectableObject))
    return false;

  const occ::handle<SelectMgr_Selection>& aSel = theSelectableObject->Selection (theMode);
  return !aSel.IsNull()
       && aSel->GetSelectionState() == SelectMgr_SOS_Activated;
}

//==================================================
// Function: IsInside
// Purpose :
//==================================================
bool SelectMgr_ViewerSelector::IsInside (const occ::handle<SelectMgr_SelectableObject>& theSelectableObject,
                                                     const int theMode) const
{
  if (!Contains (theSelectableObject))
    return false;

  const occ::handle<SelectMgr_Selection>& aSel = theSelectableObject->Selection (theMode);
  return !aSel.IsNull()
       && aSel->GetSelectionState() != SelectMgr_SOS_Unknown;
}

//=======================================================================
//function : Status
//purpose  :
//=======================================================================

SelectMgr_StateOfSelection SelectMgr_ViewerSelector::Status (const occ::handle<SelectMgr_Selection>& theSelection) const
{
  return theSelection->GetSelectionState();
}

//==================================================
// Function: Status
// Purpose : gives Information about selectors
//==================================================

TCollection_AsciiString SelectMgr_ViewerSelector::Status (const occ::handle<SelectMgr_SelectableObject>& theSelectableObject) const
{
  TCollection_AsciiString aStatus ("Status Object :\n\t");
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter (theSelectableObject->Selections()); aSelIter.More(); aSelIter.Next())
  {
    if (aSelIter.Value()->GetSelectionState() != SelectMgr_SOS_Unknown)
    {
      aStatus = aStatus + "Mode " + TCollection_AsciiString (aSelIter.Value()->Mode()) + " present - "
              + (aSelIter.Value()->GetSelectionState() == SelectMgr_SOS_Activated ? " Active \n\t" : " Inactive \n\t");
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
//purpose  :
//=======================================================================
void SelectMgr_ViewerSelector::SortResult() const
{
  if (mystored.IsEmpty())
  {
    myIsSorted = true;
    return;
  }

  const int anExtent = mystored.Extent();
  if (anExtent != myIndexes.Length())
  {
    myIndexes.Resize (1, anExtent, false);
  }

  for (int anIndexIter = 1; anIndexIter <= anExtent; ++anIndexIter)
  {
    myIndexes.SetValue (anIndexIter, anIndexIter);
  }
  std::sort (myIndexes.begin(), myIndexes.end(), CompareResults (mystored, myToPreferClosest));
  myIsSorted = true;
}

//=======================================================================
// function : AddSelectableObject
// purpose  : Adds new object to the map of selectable objects
//=======================================================================
void SelectMgr_ViewerSelector::AddSelectableObject (const occ::handle<SelectMgr_SelectableObject>& theObject)
{
  if (!myMapOfObjectSensitives.IsBound (theObject))
  {
    mySelectableObjects.Append (theObject);
    occ::handle<SelectMgr_SensitiveEntitySet> anEntitySet = new SelectMgr_SensitiveEntitySet (myEntitySetBuilder);
    myMapOfObjectSensitives.Bind (theObject, anEntitySet);
  }
}

//=======================================================================
// function : AddSelectionToObject
// purpose  : Adds new selection to the object and builds its BVH tree
//=======================================================================
void SelectMgr_ViewerSelector::AddSelectionToObject (const occ::handle<SelectMgr_SelectableObject>& theObject,
                                                     const occ::handle<SelectMgr_Selection>& theSelection)
{
  if (occ::handle<SelectMgr_SensitiveEntitySet>* anEntitySet = myMapOfObjectSensitives.ChangeSeek (theObject))
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
void SelectMgr_ViewerSelector::MoveSelectableObject (const occ::handle<SelectMgr_SelectableObject>& theObject)
{
  mySelectableObjects.ChangeSubset (theObject);
}

//=======================================================================
// function : RemoveSelectableObject
// purpose  : Removes selectable object from map of selectable ones
//=======================================================================
void SelectMgr_ViewerSelector::RemoveSelectableObject (const occ::handle<SelectMgr_SelectableObject>& theObject)
{
  if (myMapOfObjectSensitives.UnBind (theObject))
  {
    RemovePicked (theObject);
    mySelectableObjects.Remove (theObject);
  }
}

//=======================================================================
// function : RemoveSelectionOfObject
// purpose  : Removes selection of the object and marks its BVH tree
//            for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RemoveSelectionOfObject (const occ::handle<SelectMgr_SelectableObject>& theObject,
                                                        const occ::handle<SelectMgr_Selection>& theSelection)
{
  if (occ::handle<SelectMgr_SensitiveEntitySet>* anEntitySet = myMapOfObjectSensitives.ChangeSeek (theObject))
  {
    (*anEntitySet)->Remove (theSelection);
  }
}

//=======================================================================
// function : RebuildObjectsTree
// purpose  : Marks BVH of selectable objects for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RebuildObjectsTree (const bool theIsForce)
{
  mySelectableObjects.MarkDirty();

  if (theIsForce)
  {
    Graphic3d_Vec2i aWinSize;
    mySelectingVolumeMgr.WindowSize (aWinSize.x(), aWinSize.y());
    mySelectableObjects.UpdateBVH (mySelectingVolumeMgr.Camera(), aWinSize);
  }
}

//=======================================================================
// function : RebuildSensitivesTree
// purpose  : Marks BVH of sensitive entities of particular selectable
//            object for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RebuildSensitivesTree (const occ::handle<SelectMgr_SelectableObject>& theObject,
                                                      const bool theIsForce)
{
  if (!Contains (theObject))
    return;

  occ::handle<SelectMgr_SensitiveEntitySet>& anEntitySet = myMapOfObjectSensitives.ChangeFind (theObject);
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
  for (NCollection_DataMap<occ::handle<SelectMgr_SelectableObject>, occ::handle<SelectMgr_SensitiveEntitySet>>::Iterator aSensitivesIter (myMapOfObjectSensitives); aSensitivesIter.More(); aSensitivesIter.Next())
  {
    occ::handle<SelectMgr_SensitiveEntitySet>& anEntitySet = aSensitivesIter.ChangeValue();
    const int anEntitiesNb = anEntitySet->Size();
    for (int anIdx = 0; anIdx < anEntitiesNb; ++anIdx)
    {
      anEntitySet->GetSensitiveById (anIdx)->ResetSelectionActiveStatus();
    }
  }
}

//=======================================================================
// function : ActiveOwners
// purpose  : Returns the list of active entity owners
//=======================================================================
void SelectMgr_ViewerSelector::ActiveOwners (NCollection_List<occ::handle<SelectMgr_EntityOwner>>& theOwners) const
{
  for (NCollection_DataMap<occ::handle<SelectMgr_SelectableObject>, occ::handle<SelectMgr_SensitiveEntitySet>>::Iterator anIter (myMapOfObjectSensitives); anIter.More(); anIter.Next())
  {
    const occ::handle<SelectMgr_SensitiveEntitySet>& anEntitySet = anIter.Value();
    const int anEntitiesNb = anEntitySet->Size();
    for (int anIdx = 0; anIdx < anEntitiesNb; ++anIdx)
    {
      const occ::handle<SelectMgr_SensitiveEntity>& aSensitive = anEntitySet->GetSensitiveById (anIdx);
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
void SelectMgr_ViewerSelector::AllowOverlapDetection (const bool theIsToAllow)
{
  mySelectingVolumeMgr.AllowOverlapDetection (theIsToAllow);
}

//=======================================================================
// Function: Pick
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector::Pick (const int theXPix,
                                     const int theYPix,
                                     const occ::handle<V3d_View>& theView)
{
  updateZLayers (theView);

  gp_Pnt2d aMousePos (static_cast<double> (theXPix),
                      static_cast<double> (theYPix));
  mySelectingVolumeMgr.InitPointSelectingVolume (aMousePos);

  mySelectingVolumeMgr.SetPixelTolerance (myTolerances.Tolerance());
  mySelectingVolumeMgr.SetCamera (theView->Camera());
  int aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);

  mySelectingVolumeMgr.BuildSelectingVolume();
  mySelectingVolumeMgr.SetViewClipping (theView->ClipPlanes(), occ::handle<Graphic3d_SequenceOfHClipPlane>(), NULL);

  TraverseSensitives (theView->View()->Identification());
}

//=======================================================================
// Function: Pick
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector::Pick (const int theXPMin,
                                     const int theYPMin,
                                     const int theXPMax,
                                     const int theYPMax,
                                     const occ::handle<V3d_View>& theView)
{
  updateZLayers (theView);

  gp_Pnt2d aMinMousePos (static_cast<double> (theXPMin),
                         static_cast<double> (theYPMin));
  gp_Pnt2d aMaxMousePos (static_cast<double> (theXPMax),
                         static_cast<double> (theYPMax));
  mySelectingVolumeMgr.InitBoxSelectingVolume (aMinMousePos,
                                               aMaxMousePos);

  mySelectingVolumeMgr.SetCamera (theView->Camera());
  int aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);

  mySelectingVolumeMgr.BuildSelectingVolume();
  mySelectingVolumeMgr.SetViewClipping (theView->ClipPlanes(), occ::handle<Graphic3d_SequenceOfHClipPlane>(), NULL);
  TraverseSensitives (theView->View()->Identification());
}

//=======================================================================
// Function: Pick
// Purpose : Selection using a polyline
//=======================================================================
void SelectMgr_ViewerSelector::Pick (const TColgp_Array1OfPnt2d& thePolyline,
                                     const occ::handle<V3d_View>& theView)
{
  updateZLayers (theView);

  mySelectingVolumeMgr.InitPolylineSelectingVolume (thePolyline);
  mySelectingVolumeMgr.SetCamera (theView->Camera());
  int aWidth = 0, aHeight = 0;
  theView->Window()->Size (aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize (aWidth, aHeight);
  mySelectingVolumeMgr.BuildSelectingVolume();
  mySelectingVolumeMgr.SetViewClipping (theView->ClipPlanes(), occ::handle<Graphic3d_SequenceOfHClipPlane>(), NULL);

  TraverseSensitives (theView->View()->Identification());
}

//=======================================================================
// Function: Pick
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector::Pick (const gp_Ax1& theAxis,
                                     const occ::handle<V3d_View>& theView)
{
  updateZLayers (theView);

  mySelectingVolumeMgr.InitAxisSelectingVolume (theAxis);
  mySelectingVolumeMgr.BuildSelectingVolume();
  mySelectingVolumeMgr.SetViewClipping (theView->ClipPlanes(), occ::handle<Graphic3d_SequenceOfHClipPlane>(), NULL);

  TraverseSensitives (theView->View()->Identification());
}

//=======================================================================
//function : ToPixMap
//purpose  :
//=======================================================================
bool SelectMgr_ViewerSelector::ToPixMap (Image_PixMap& theImage,
                                                     const occ::handle<V3d_View>& theView,
                                                     const StdSelect_TypeOfSelectionImage theType,
                                                     const int               thePickedIndex)
{
  if (theImage.IsEmpty())
  {
    throw Standard_ProgramError ("SelectMgr_ViewerSelector::ToPixMap() has been called with empty image");
  }

  occ::handle<SelectMgr_SelectionImageFiller> aFiller = SelectMgr_SelectionImageFiller::CreateFiller (theImage, this, theType);
  if (aFiller.IsNull())
  {
    return false;
  }

  const int aSizeX = static_cast<int> (theImage.SizeX());
  const int aSizeY = static_cast<int> (theImage.SizeY());
  for (int aRowIter = 0; aRowIter < aSizeY; ++aRowIter)
  {
    for (int aColIter = 0; aColIter < aSizeX; ++aColIter)
    {
      Pick (aColIter, aRowIter, theView);
      aFiller->Fill (aColIter, aRowIter, thePickedIndex);
    }
  }
  aFiller->Flush();
  return true;
}

//=======================================================================
// Function: DisplaySensitive.
// Purpose : Display active primitives.
//=======================================================================
void SelectMgr_ViewerSelector::DisplaySensitive (const occ::handle<V3d_View>& theView)
{
  for (SelectMgr_SelectableObjectSet::Iterator aSelectableIt (mySelectableObjects); aSelectableIt.More(); aSelectableIt.Next())
  {
    occ::handle<Graphic3d_Structure> aStruct = new Graphic3d_Structure (theView->Viewer()->StructureManager());
    const Handle (SelectMgr_SelectableObject)& anObj = aSelectableIt.Value();
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter (anObj->Selections()); aSelIter.More(); aSelIter.Next())
    {
      if (aSelIter.Value()->GetSelectionState() == SelectMgr_SOS_Activated)
      {
        SelectMgr::ComputeSensitivePrs (aStruct, aSelIter.Value(), anObj->Transformation(), anObj->TransformPersistence());
      }
    }

    myStructs.Append (aStruct);
  }

  for (Graphic3d_SequenceOfStructure::Iterator aStructIter (myStructs); aStructIter.More(); aStructIter.Next())
  {
    occ::handle<Graphic3d_Structure>& aStruct = aStructIter.ChangeValue();
    aStruct->SetDisplayPriority (Graphic3d_DisplayPriority_Topmost);
    aStruct->Display();
  }

  theView->Update();
}

//=======================================================================
// Function: ClearSensitive
// Purpose :
//=======================================================================
void SelectMgr_ViewerSelector::ClearSensitive (const occ::handle<V3d_View>& theView)
{
  for (Graphic3d_SequenceOfStructure::Iterator aStructIter (myStructs); aStructIter.More(); aStructIter.Next())
  {
    const occ::handle<Graphic3d_Structure>& aPrs = aStructIter.ChangeValue();
    aPrs->Erase();
    aPrs->Clear();
    aPrs->Remove();
  }
  myStructs.Clear();

  if (!theView.IsNull())
  {
    theView->Update();
  }
}

//=======================================================================
//function : DisplaySenstive
//purpose  :
//=======================================================================
void SelectMgr_ViewerSelector::DisplaySensitive (const occ::handle<SelectMgr_Selection>& theSel,
                                                 const gp_Trsf& theTrsf,
                                                 const occ::handle<V3d_View>& theView,
                                                 const bool theToClearOthers)
{
  if (theToClearOthers)
  {
    ClearSensitive (theView);
  }

  occ::handle<Graphic3d_Structure> aStruct = new Graphic3d_Structure (theView->Viewer()->StructureManager());

  SelectMgr::ComputeSensitivePrs (aStruct, theSel, theTrsf, occ::handle<Graphic3d_TransformPers>());

  myStructs.Append (aStruct);
  myStructs.Last()->SetDisplayPriority (Graphic3d_DisplayPriority_Topmost);
  myStructs.Last()->Display();

  theView->Update();
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void SelectMgr_ViewerSelector::DumpJson (Standard_OStream& theOStream, int theDepth) const 
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToPreferClosest)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, mystored.Extent())

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &mySelectingVolumeMgr)
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, &mySelectableObjects)

  int aNbOfSelectableObjects = 0;
  for (SelectMgr_SelectableObjectSet::Iterator aSelectableIt (mySelectableObjects); aSelectableIt.More(); aSelectableIt.Next())
  {
    aNbOfSelectableObjects++;
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, aNbOfSelectableObjects)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTolerances.Tolerance())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTolerances.CustomTolerance())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myZLayerOrderMap.Extent())

  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myEntitySetBuilder.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myCameraEye)
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, &myCameraDir)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myCameraScale)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIndexes.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsLeftChildQueuedFirst)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myMapOfObjectSensitives.Extent())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myStructs.Length())
  for (Graphic3d_SequenceOfStructure::Iterator aStructsIt (myStructs); aStructsIt.More(); aStructsIt.Next())
  {
    const occ::handle<Graphic3d_Structure>& aStructure = aStructsIt.Value();
    OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, aStructure)
  }
}

//=======================================================================
//function : SetToPrebuildBVH
//purpose  : 
//=======================================================================
void SelectMgr_ViewerSelector::SetToPrebuildBVH (bool theToPrebuild, int theThreadsNum)
{
  if (!theToPrebuild && !myBVHThreadPool.IsNull())
  {
    myBVHThreadPool.Nullify();
  }
  else if (theToPrebuild)
  {
    myBVHThreadPool = new SelectMgr_BVHThreadPool (theThreadsNum);
  }
  myToPrebuildBVH = theToPrebuild;
}

//=======================================================================
//function : QueueBVHBuild
//purpose  : 
//=======================================================================
void SelectMgr_ViewerSelector::QueueBVHBuild (const occ::handle<Select3D_SensitiveEntity>& theEntity)
{
  if (myToPrebuildBVH)
  {
    myBVHThreadPool->AddEntity (theEntity);
  }
}

//=======================================================================
//function : WaitForBVHBuild
//purpose  : 
//=======================================================================
void SelectMgr_ViewerSelector::WaitForBVHBuild()
{
  if (myToPrebuildBVH)
  {
    myBVHThreadPool->WaitThreads();
  }
}

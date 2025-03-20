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

#include <AIS_InteractiveObject.hxx>
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
  CompareResults(const SelectMgr_IndexedDataMapOfOwnerCriterion& theMapOfCriterion,
                 bool                                            theToPreferClosest)
      : myMapOfCriterion(&theMapOfCriterion),
        myToPreferClosest(theToPreferClosest)
  {
  }

  Standard_Boolean operator()(Standard_Integer theLeft, Standard_Integer theRight) const
  {
    const SelectMgr_SortCriterion& anElemLeft  = myMapOfCriterion->FindFromIndex(theLeft);
    const SelectMgr_SortCriterion& anElemRight = myMapOfCriterion->FindFromIndex(theRight);
    if ((anElemLeft.IsPreferPriority && anElemRight.IsPreferPriority) || !myToPreferClosest)
    {
      return anElemLeft.IsHigherPriority(anElemRight);
    }
    else
    {
      return anElemLeft.IsCloserDepth(anElemRight);
    }
  }

private:
  const SelectMgr_IndexedDataMapOfOwnerCriterion* myMapOfCriterion;
  bool                                            myToPreferClosest;
};

static const Graphic3d_Mat4d SelectMgr_ViewerSelector_THE_IDENTITY_MAT;
} // namespace

//=================================================================================================

void SelectMgr_ViewerSelector::updatePoint3d(SelectMgr_SortCriterion&                theCriterion,
                                             const SelectBasics_PickResult&          thePickResult,
                                             const Handle(Select3D_SensitiveEntity)& theEntity,
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
    theCriterion.Point    = thePickResult.PickedPoint();
    theCriterion.Normal   = thePickResult.SurfaceNormal();
    const float aNormLen2 = theCriterion.Normal.SquareModulus();
    if (aNormLen2 > ShortRealEpsilon())
    {
      hasNormal = true;
      theCriterion.Normal *= 1.0f / sqrtf(aNormLen2);
    }
  }
  else if (!thePickResult.IsValid())
  {
    theCriterion.Point = thePickResult.PickedPoint();
    return;
  }
  else
  {
    theCriterion.Point = theMgr.DetectedPoint(theCriterion.Depth);
  }

  gp_GTrsf anInvTrsf = theInversedTrsf;
  if (theCriterion.Entity->HasInitLocation())
  {
    anInvTrsf = theCriterion.Entity->InvInitLocation() * anInvTrsf;
  }
  if (anInvTrsf.Form() != gp_Identity)
  {
    const gp_GTrsf anInvInvTrsd = anInvTrsf.Inverted();
    anInvInvTrsd.Transforms(theCriterion.Point.ChangeCoord());
    if (hasNormal)
    {
      Graphic3d_Mat4d aMat4;
      anInvInvTrsd.GetMat4(aMat4);
      const Graphic3d_Vec4d aNormRes =
        aMat4 * Graphic3d_Vec4d(Graphic3d_Vec3d(theCriterion.Normal), 0.0);
      theCriterion.Normal = Graphic3d_Vec3(aNormRes.xyz());
    }
  }

  const Standard_Real aSensFactor =
    myDepthTolType == SelectMgr_TypeOfDepthTolerance_SensitivityFactor
      ? theEntity->SensitivityFactor()
      : myDepthTolerance;
  switch (myDepthTolType)
  {
    case SelectMgr_TypeOfDepthTolerance_Uniform: {
      theCriterion.Tolerance = myDepthTolerance;
      break;
    }
    case SelectMgr_TypeOfDepthTolerance_UniformPixels:
    case SelectMgr_TypeOfDepthTolerance_SensitivityFactor: {
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
        const Standard_Real aDistFromEye =
          Abs((theCriterion.Point.XYZ() - myCameraEye.XYZ()).Dot(myCameraDir.XYZ()));
        theCriterion.Tolerance = aDistFromEye * myCameraScale * aSensFactor;
      }
      break;
    }
  }
}

//=================================================================================================

SelectMgr_ViewerSelector::SelectMgr_ViewerSelector()
    : myDepthTolerance(0.0),
      myDepthTolType(SelectMgr_TypeOfDepthTolerance_SensitivityFactor),
      myToPreferClosest(Standard_True),
      myCameraScale(1.0),
      myToPrebuildBVH(Standard_False),
      myIsSorted(Standard_False),
      myIsLeftChildQueuedFirst(Standard_False)
{
  myEntitySetBuilder = new BVH_BinnedBuilder<Standard_Real, 3, 4>(BVH_Constants_LeafNodeSizeSingle,
                                                                  BVH_Constants_MaxTreeDepth,
                                                                  Standard_True);
}

//=================================================================================================

void SelectMgr_ViewerSelector::SetPixelTolerance(const Standard_Integer theTolerance)
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
    myTolerances.SetCustomTolerance(theTolerance);
  }
}

//=================================================================================================

void SelectMgr_ViewerSelector::Activate(const Handle(SelectMgr_Selection)& theSelection)
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    aSelEntIter.Value()->SetActiveForSelection();
  }

  if (theSelection->GetSelectionState() != SelectMgr_SOS_Activated)
  {
    theSelection->SetSelectionState(SelectMgr_SOS_Activated);

    myTolerances.Add(theSelection->Sensitivity());
  }
}

//=================================================================================================

void SelectMgr_ViewerSelector::Deactivate(const Handle(SelectMgr_Selection)& theSelection)
{
  for (NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator aSelEntIter(
         theSelection->Entities());
       aSelEntIter.More();
       aSelEntIter.Next())
  {
    aSelEntIter.Value()->ResetSelectionActiveStatus();
  }

  if (theSelection->GetSelectionState() == SelectMgr_SOS_Activated)
  {
    theSelection->SetSelectionState(SelectMgr_SOS_Deactivated);

    myTolerances.Decrement(theSelection->Sensitivity());
  }
}

//=======================================================================
// function: isToScaleFrustum
// purpose : Checks if the entity given requires to scale current selecting frustum
//=======================================================================
Standard_Boolean SelectMgr_ViewerSelector::isToScaleFrustum(
  const Handle(Select3D_SensitiveEntity)& theEntity)
{
  return mySelectingVolumeMgr.IsScalableActiveVolume()
         && sensitivity(theEntity) < myTolerances.Tolerance();
}

//=======================================================================
// function: sensitivity
// purpose : In case if custom tolerance is set, this method will return sum of entity
//           sensitivity and custom tolerance.
//=======================================================================
Standard_Integer SelectMgr_ViewerSelector::sensitivity(
  const Handle(Select3D_SensitiveEntity)& theEntity) const
{
  return myTolerances.IsCustomTolSet()
           ? theEntity->SensitivityFactor() + myTolerances.CustomTolerance()
           : theEntity->SensitivityFactor();
}

//=======================================================================
// function: checkOverlap
// purpose : Internal function that checks if a particular sensitive
//           entity theEntity overlaps current selecting volume precisely
//=======================================================================
void SelectMgr_ViewerSelector::checkOverlap(const Handle(Select3D_SensitiveEntity)& theEntity,
                                            const gp_GTrsf&                         theInversedTrsf,
                                            SelectMgr_SelectingVolumeManager&       theMgr)
{
  const Handle(SelectMgr_EntityOwner)& anOwner = theEntity->OwnerId();
  Handle(SelectMgr_SelectableObject)   aSelectable =
    !anOwner.IsNull() ? anOwner->Selectable() : Handle(SelectMgr_SelectableObject)();
  SelectBasics_PickResult aPickResult;
  const Standard_Boolean  isMatched = theEntity->Matches(theMgr, aPickResult);
  if (!isMatched || anOwner.IsNull())
  {
    return;
  }
  SelectMgr_SortCriterion aCriterion;
  myZLayerOrderMap.Find(!aSelectable.IsNull() ? aSelectable->ZLayer() : Graphic3d_ZLayerId_Default,
                        aCriterion.ZLayerPosition);
  aCriterion.Entity                   = theEntity;
  aCriterion.SelectionPriority        = anOwner->Priority();
  aCriterion.Depth                    = aPickResult.Depth();
  aCriterion.MinDist                  = aPickResult.DistToGeomCenter();
  Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast(aSelectable);
  if (!aSelectable.IsNull())
  {
    if (aSelectable->Presentations().Size() > 0 && !aSelectable->TransformPersistence().IsNull())
    {
      if (aSelectable->TransformPersistence()->Mode() == Graphic3d_TMF_2d)
      {
        aCriterion.IsPreferPriority = Standard_True;
        aCriterion.DisplayPriority  = Graphic3d_DisplayPriority_INVALID;
        if (!anObj.IsNull())
        {
          Handle(Prs3d_Presentation) aPrs = anObj->Presentation();
          if (!aPrs.IsNull())
          {
            aCriterion.DisplayPriority = aPrs->DisplayPriority();
          }
        }
      }
    }
  }

  if (SelectMgr_SortCriterion* aPrevCriterion = mystored.ChangeSeek(anOwner))
  {
    ++aPrevCriterion->NbOwnerMatches;
    aCriterion.NbOwnerMatches = aPrevCriterion->NbOwnerMatches;
    if (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Box)
    {
      updatePoint3d(aCriterion, aPickResult, theEntity, theInversedTrsf, theMgr);
      if (aCriterion.IsCloserDepth(*aPrevCriterion))
      {
        *aPrevCriterion = aCriterion;
      }
    }
  }
  else
  {
    aCriterion.NbOwnerMatches = 1;
    updatePoint3d(aCriterion, aPickResult, theEntity, theInversedTrsf, theMgr);
    mystored.Add(anOwner, aCriterion);
  }
}

//=================================================================================================

void SelectMgr_ViewerSelector::updateZLayers(const Handle(V3d_View)& theView)
{
  myZLayerOrderMap.Clear();
  TColStd_SequenceOfInteger aZLayers;
  theView->Viewer()->GetAllZLayers(aZLayers);
  Standard_Integer aPos             = 0;
  Standard_Boolean isPrevDepthWrite = true;
  for (TColStd_SequenceOfInteger::Iterator aLayerIter(aZLayers); aLayerIter.More();
       aLayerIter.Next())
  {
    Graphic3d_ZLayerSettings aSettings = theView->Viewer()->ZLayerSettings(aLayerIter.Value());
    if (aSettings.ToClearDepth() || isPrevDepthWrite != aSettings.ToEnableDepthWrite())
    {
      ++aPos;
    }
    isPrevDepthWrite = aSettings.ToEnableDepthWrite();
    myZLayerOrderMap.Bind(aLayerIter.Value(), aPos);
  }
}

//=================================================================================================

void SelectMgr_ViewerSelector::computeFrustum(const Handle(Select3D_SensitiveEntity)& theEnt,
                                              const SelectMgr_SelectingVolumeManager& theMgrGlobal,
                                              const SelectMgr_SelectingVolumeManager& theMgrObject,
                                              const gp_GTrsf&                         theInvTrsf,
                                              SelectMgr_FrustumCache&                 theCachedMgrs,
                                              SelectMgr_SelectingVolumeManager&       theResMgr)
{
  Standard_Integer aScale = isToScaleFrustum(theEnt) ? sensitivity(theEnt) : 1;
  const gp_GTrsf   aTrsfMtr =
    theEnt->HasInitLocation() ? theEnt->InvInitLocation() * theInvTrsf : theInvTrsf;
  const Standard_Boolean toScale     = aScale != 1;
  const Standard_Boolean toTransform = aTrsfMtr.Form() != gp_Identity;
  if (toScale && toTransform)
  {
    theResMgr = theMgrGlobal.ScaleAndTransform(aScale, aTrsfMtr, NULL);
    theResMgr.SetViewClipping(theMgrObject);
  }
  else if (toScale)
  {
    if (!theCachedMgrs.Find(aScale, theResMgr))
    {
      theResMgr = theMgrGlobal.ScaleAndTransform(aScale, gp_Trsf(), NULL);
      theCachedMgrs.Bind(aScale, theResMgr);
    }
    theResMgr.SetViewClipping(theMgrObject);
  }
  else if (toTransform)
  {
    theResMgr = theMgrGlobal.ScaleAndTransform(1, aTrsfMtr, NULL);
    theResMgr.SetViewClipping(theMgrObject);
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
void SelectMgr_ViewerSelector::traverseObject(const Handle(SelectMgr_SelectableObject)& theObject,
                                              const SelectMgr_SelectingVolumeManager&   theMgr,
                                              const Handle(Graphic3d_Camera)&           theCamera,
                                              const Graphic3d_Mat4d& theProjectionMat,
                                              const Graphic3d_Mat4d& theWorldViewMat,
                                              const Graphic3d_Vec2i& theWinSize)
{
  Handle(SelectMgr_SensitiveEntitySet)& anEntitySet = myMapOfObjectSensitives.ChangeFind(theObject);
  if (anEntitySet->Size() == 0)
  {
    return;
  }

  const bool hasEntityTrsfPers = anEntitySet->HasEntityWithPersistence() && !theCamera.IsNull();
  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aSensitivesTree = anEntitySet->BVH();
  gp_GTrsf                                               aInversedTrsf;
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

      const Graphic3d_Mat4d aMat = theObject->TransformPersistence()->Compute(theCamera,
                                                                              theProjectionMat,
                                                                              theWorldViewMat,
                                                                              theWinSize.x(),
                                                                              theWinSize.y());
      gp_GTrsf              aTPers;
      aTPers.SetMat4(aMat);
      aInversedTrsf = (aTPers * gp_GTrsf(theObject->Transformation())).Inverted();
    }
  }

  SelectMgr_SelectingVolumeManager aMgr =
    aInversedTrsf.Form() != gp_Identity ? theMgr.ScaleAndTransform(1, aInversedTrsf, NULL) : theMgr;
  if (!hasEntityTrsfPers
      && !aMgr.OverlapsBox(aSensitivesTree->MinPoint(0), aSensitivesTree->MaxPoint(0)))
  {
    return;
  }

  if (!theObject->ClipPlanes().IsNull() && theObject->ClipPlanes()->ToOverrideGlobal())
  {
    aMgr.SetViewClipping(Handle(Graphic3d_SequenceOfHClipPlane)(),
                         theObject->ClipPlanes(),
                         &theMgr);
  }
  else if (!theObject->TransformPersistence().IsNull())
  {
    if (theObject->TransformPersistence()->IsZoomOrRotate() && !theMgr.ViewClipping().IsNull())
    {
      // Zoom/rotate persistence object lives in two worlds at the same time.
      // Global clipping planes can not be trivially applied without being converted
      // into local space of transformation persistence object.
      // As more simple alternative - just clip entire object by its anchor point defined in the
      // world space.
      const gp_Pnt anAnchor = theObject->TransformPersistence()->AnchorPoint();
      for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt(*theMgr.ViewClipping());
           aPlaneIt.More();
           aPlaneIt.Next())
      {
        const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
        if (!aPlane->IsOn())
        {
          continue;
        }

        const Graphic3d_Vec4d aCheckPnt(anAnchor.X(), anAnchor.Y(), anAnchor.Z(), 1.0);
        if (aPlane->ProbePoint(aCheckPnt) == Graphic3d_ClipState_Out)
        {
          return;
        }
      }
    }

    aMgr.SetViewClipping(Handle(Graphic3d_SequenceOfHClipPlane)(),
                         theObject->ClipPlanes(),
                         &theMgr);
  }
  else if (!theObject->ClipPlanes().IsNull() && !theObject->ClipPlanes()->IsEmpty())
  {
    aMgr.SetViewClipping(theMgr.ViewClipping(), theObject->ClipPlanes(), &theMgr);
  }

  if (!theMgr.ViewClipping().IsNull()
      && theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Box)
  {
    Graphic3d_BndBox3d aBBox(aSensitivesTree->MinPoint(0), aSensitivesTree->MaxPoint(0));
    // If box selection is active, and the whole sensitive tree is out of the clip planes
    // selection is empty for this object
    const Handle(Graphic3d_SequenceOfHClipPlane)& aViewPlanes = theMgr.ViewClipping();

    for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt(*aViewPlanes); aPlaneIt.More();
         aPlaneIt.Next())
    {
      const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
      if (!aPlane->IsOn())
      {
        continue;
      }

      Graphic3d_ClipState aState = aPlane->ProbeBox(aBBox);
      if (aState == Graphic3d_ClipState_Out) // do not process only whole trees, next check on the
                                             // tree node
      {
        return;
      }
    }
  }

  const Standard_Integer aFirstStored = mystored.Extent() + 1;

  Standard_Integer                 aStack[BVH_Constants_MaxTreeDepth];
  Standard_Integer                 aHead = -1;
  Standard_Integer                 aNode = 0; // a root node
  SelectMgr_FrustumCache           aScaledTrnsfFrustums;
  SelectMgr_SelectingVolumeManager aTmpMgr;
  for (;;)
  {
    if (!aSensitivesTree->IsOuter(aNode))
    {
      const Standard_Integer aLeftChildIdx  = aSensitivesTree->Child<0>(aNode);
      const Standard_Integer aRightChildIdx = aSensitivesTree->Child<1>(aNode);
      const Standard_Boolean isLeftChildIn =
        hasEntityTrsfPers
        || aMgr.OverlapsBox(aSensitivesTree->MinPoint(aLeftChildIdx),
                            aSensitivesTree->MaxPoint(aLeftChildIdx));
      const Standard_Boolean isRightChildIn =
        hasEntityTrsfPers
        || aMgr.OverlapsBox(aSensitivesTree->MinPoint(aRightChildIdx),
                            aSensitivesTree->MaxPoint(aRightChildIdx));
      if (isLeftChildIn && isRightChildIn)
      {
        aNode = aLeftChildIdx;
        ++aHead;
        aStack[aHead] = aRightChildIdx;
      }
      else if (isLeftChildIn || isRightChildIn)
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
      if (!theMgr.ViewClipping().IsNull()
          && theMgr.GetActiveSelectionType() == SelectMgr_SelectionType_Box)
      {
        Graphic3d_BndBox3d aBBox(aSensitivesTree->MinPoint(aNode),
                                 aSensitivesTree->MaxPoint(aNode));
        // If box selection is active, and the whole sensitive tree is out of the clip planes
        // selection is empty for this object
        const Handle(Graphic3d_SequenceOfHClipPlane)& aViewPlanes = theMgr.ViewClipping();

        for (Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt(*aViewPlanes); aPlaneIt.More();
             aPlaneIt.Next())
        {
          const Handle(Graphic3d_ClipPlane)& aPlane = aPlaneIt.Value();
          if (!aPlane->IsOn())
          {
            continue;
          }
          Graphic3d_ClipState aState = aPlane->ProbeBox(aBBox);
          if (aState == Graphic3d_ClipState_Out)
          {
            aClipped = true;
            break;
          }
          if (aState == Graphic3d_ClipState_On
              && !mySelectingVolumeMgr.IsOverlapAllowed()) // partially clipped
          {
            if (aPlane->ProbeBoxTouch(aBBox))
              continue;
            aClipped = true;
            break;
          }
        }
      }
      if (!aClipped)
      {
        const Standard_Integer aStartIdx = aSensitivesTree->BegPrimitive(aNode);
        const Standard_Integer anEndIdx  = aSensitivesTree->EndPrimitive(aNode);
        for (Standard_Integer anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
        {
          const Handle(SelectMgr_SensitiveEntity)& aSensitive =
            anEntitySet->GetSensitiveById(anIdx);
          if (!aSensitive->IsActiveForSelection())
          {
            continue;
          }

          const Handle(Select3D_SensitiveEntity)& anEnt = aSensitive->BaseSensitive();

          gp_GTrsf aInvSensTrsf = aInversedTrsf;
          if (!anEnt->TransformPersistence().IsNull())
          {
            if (theCamera.IsNull())
            {
              continue;
            }
            const Graphic3d_Mat4d aMat = anEnt->TransformPersistence()->Compute(theCamera,
                                                                                theProjectionMat,
                                                                                theWorldViewMat,
                                                                                theWinSize.x(),
                                                                                theWinSize.y());
            gp_GTrsf              aTPers;
            aTPers.SetMat4(aMat);
            aInvSensTrsf = (aTPers * gp_GTrsf(theObject->Transformation())).Inverted();
          }

          computeFrustum(anEnt, theMgr, aMgr, aInvSensTrsf, aScaledTrnsfFrustums, aTmpMgr);
          checkOverlap(anEnt, aInvSensTrsf, aTmpMgr);
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

  for (Standard_Integer aStoredIter = mystored.Extent(); aStoredIter >= aFirstStored; --aStoredIter)
  {
    const SelectMgr_SortCriterion&       aCriterion       = mystored.FindFromIndex(aStoredIter);
    const Handle(SelectMgr_EntityOwner)& anOwner          = aCriterion.Entity->OwnerId();
    Standard_Integer                     aNbOwnerEntities = 0;
    anEntitySet->Owners().Find(anOwner, aNbOwnerEntities);
    if (aNbOwnerEntities > aCriterion.NbOwnerMatches)
    {
      mystored.RemoveFromIndex(aStoredIter);
    }
  }
}

//=======================================================================
// function: TraverseSensitives
// purpose : Traverses BVH containing all added selectable objects and
//           finds candidates for further search of overlap
//=======================================================================
void SelectMgr_ViewerSelector::TraverseSensitives(const Standard_Integer theViewId)
{
  SelectMgr_BVHThreadPool::Sentry aSentry(myBVHThreadPool);

  mystored.Clear();
  myIsSorted = false;

  Graphic3d_Vec2i aWinSize;
  mySelectingVolumeMgr.WindowSize(aWinSize.x(), aWinSize.y());
  const double aPixelSize =
    aWinSize.x() > 0 && aWinSize.y() > 0 ? Max(1.0 / aWinSize.x(), 1.0 / aWinSize.y()) : 1.0;

  const Handle(Graphic3d_Camera)& aCamera = mySelectingVolumeMgr.Camera();
  Graphic3d_Mat4d                 aProjectionMat, aWorldViewMat;
  Graphic3d_WorldViewProjState    aViewState;
  if (!aCamera.IsNull())
  {
    aProjectionMat = aCamera->ProjectionMatrix();
    aWorldViewMat  = aCamera->OrientationMatrix();
    aViewState     = aCamera->WorldViewProjState();

    myCameraEye = aCamera->Eye().XYZ();
    myCameraDir = aCamera->Direction().XYZ();
  }
  mySelectableObjects.UpdateBVH(aCamera, aWinSize);

  for (Standard_Integer aBVHSetIt = 0; aBVHSetIt < SelectMgr_SelectableObjectSet::BVHSubsetNb;
       ++aBVHSetIt)
  {
    const SelectMgr_SelectableObjectSet::BVHSubset aBVHSubset =
      (SelectMgr_SelectableObjectSet::BVHSubset)aBVHSetIt;
    if (mySelectableObjects.IsEmpty(aBVHSubset))
    {
      continue;
    }
    if (aCamera.IsNull() && aBVHSubset != SelectMgr_SelectableObjectSet::BVHSubset_3d)
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
      aTFrustum.SetValue(1, 1, aWorldViewMat.GetValue(0, 0));
      aTFrustum.SetValue(1, 2, aWorldViewMat.GetValue(0, 1));
      aTFrustum.SetValue(1, 3, aWorldViewMat.GetValue(0, 2));
      aTFrustum.SetValue(2, 1, aWorldViewMat.GetValue(1, 0));
      aTFrustum.SetValue(2, 2, aWorldViewMat.GetValue(1, 1));
      aTFrustum.SetValue(2, 3, aWorldViewMat.GetValue(1, 2));
      aTFrustum.SetValue(3, 1, aWorldViewMat.GetValue(2, 0));
      aTFrustum.SetValue(3, 2, aWorldViewMat.GetValue(2, 1));
      aTFrustum.SetValue(3, 3, aWorldViewMat.GetValue(2, 2));
      aTFrustum.SetTranslationPart(gp_XYZ(aWorldViewMat.GetValue(0, 3),
                                          aWorldViewMat.GetValue(1, 3),
                                          aWorldViewMat.GetValue(2, 3)));

      // define corresponding frustum builder parameters for 2d persistence.
      Handle(SelectMgr_FrustumBuilder) aBuilder   = new SelectMgr_FrustumBuilder();
      Handle(Graphic3d_Camera)         aNewCamera = new Graphic3d_Camera();
      aNewCamera->CopyMappingData(aCamera);
      aNewCamera->SetIdentityOrientation();
      if (aBVHSubset == SelectMgr_SelectableObjectSet::BVHSubset_ortho2dPersistent)
      {
        aNewCamera->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);
      }
      aWorldViewMat  = aNewCamera->OrientationMatrix(); // should be identity matrix
      aProjectionMat = aNewCamera->ProjectionMatrix();  // should be the same to aProjectionMat
      aBuilder->SetCamera(aNewCamera);
      aBuilder->SetWindowSize(aWinSize.x(), aWinSize.y());
      aMgr = mySelectingVolumeMgr.ScaleAndTransform(1, aTFrustum, aBuilder);
    }
    else if (aBVHSubset == SelectMgr_SelectableObjectSet::BVHSubset_ortho3dPersistent)
    {
      // define corresponding frustum builder parameters for 3d orthographic persistence.
      Handle(SelectMgr_FrustumBuilder) aBuilder   = new SelectMgr_FrustumBuilder();
      Handle(Graphic3d_Camera)         aNewCamera = new Graphic3d_Camera(*aCamera);
      aNewCamera->SetProjectionType(Graphic3d_Camera::Projection_Orthographic);
      aWorldViewMat  = aNewCamera->OrientationMatrix(); // should be the same to aWorldViewMat
      aProjectionMat = aNewCamera->ProjectionMatrix();  // should be orthographic projection
      aBuilder->SetCamera(aNewCamera);
      aBuilder->SetWindowSize(aWinSize.x(), aWinSize.y());
      aMgr = mySelectingVolumeMgr.CopyWithBuilder(aBuilder);
    }
    else
    {
      aMgr = mySelectingVolumeMgr;
    }

    if (!aMgr.Camera().IsNull())
    {
      myCameraScale = aMgr.Camera()->IsOrthographic()
                        ? aMgr.Camera()->Scale()
                        : 2.0 * Tan(aMgr.Camera()->FOVy() * M_PI / 360.0);
      myCameraScale *= aPixelSize;
    }

    const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVHTree =
      mySelectableObjects.BVH(aBVHSubset);

    Standard_Integer aNode = 0;
    if (!aMgr.OverlapsBox(aBVHTree->MinPoint(0), aBVHTree->MaxPoint(0)))
    {
      continue;
    }

    Standard_Integer aStack[BVH_Constants_MaxTreeDepth];
    Standard_Integer aHead = -1;
    for (;;)
    {
      if (!aBVHTree->IsOuter(aNode))
      {
        const Standard_Integer aLeftChildIdx  = aBVHTree->Child<0>(aNode);
        const Standard_Integer aRightChildIdx = aBVHTree->Child<1>(aNode);
        const Standard_Boolean isLeftChildIn =
          aMgr.OverlapsBox(aBVHTree->MinPoint(aLeftChildIdx), aBVHTree->MaxPoint(aLeftChildIdx));
        const Standard_Boolean isRightChildIn =
          aMgr.OverlapsBox(aBVHTree->MinPoint(aRightChildIdx), aBVHTree->MaxPoint(aRightChildIdx));
        if (isLeftChildIn && isRightChildIn)
        {
          aNode = aLeftChildIdx;
          ++aHead;
          aStack[aHead] = aRightChildIdx;
        }
        else if (isLeftChildIn || isRightChildIn)
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
        const Standard_Integer aStartIdx = aBVHTree->BegPrimitive(aNode);
        const Standard_Integer anEndIdx  = aBVHTree->EndPrimitive(aNode);
        for (Standard_Integer anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
        {
          const Handle(SelectMgr_SelectableObject)& aSelObj =
            mySelectableObjects.GetObjectById(aBVHSubset, anIdx);
          const Handle(Graphic3d_ViewAffinity)& aViewAffinity = aSelObj->ViewAffinity();
          if (theViewId == -1 || aViewAffinity->IsVisible(theViewId))
          {
            traverseObject(aSelObj, aMgr, aCamera, aProjectionMat, aWorldViewMat, aWinSize);
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

//=================================================================================================

void SelectMgr_ViewerSelector::ClearPicked()
{
  mystored.Clear();
  myIsSorted = true;
}

//=================================================================================================

bool SelectMgr_ViewerSelector::RemovePicked(const Handle(SelectMgr_SelectableObject)& theObject)
{
  if (mystored.IsEmpty() || !mySelectableObjects.Contains(theObject))
  {
    return false;
  }

  bool isRemoved = false;
  for (Standard_Integer aPickIter = 1; aPickIter <= mystored.Extent(); ++aPickIter)
  {
    const Handle(SelectMgr_EntityOwner)& aStoredOwner = mystored.FindKey(aPickIter);
    if (!aStoredOwner.IsNull() && aStoredOwner->IsSameSelectable(theObject))
    {
      mystored.RemoveFromIndex(aPickIter);
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

//=================================================================================================

Handle(SelectMgr_EntityOwner) SelectMgr_ViewerSelector::Picked(const Standard_Integer theRank) const
{
  if (theRank < 1 || theRank > NbPicked())
  {
    return Handle(SelectMgr_EntityOwner)();
  }

  if (!myIsSorted)
  {
    SortResult();
  }

  const Standard_Integer               anOwnerIdx   = myIndexes.Value(theRank);
  const Handle(SelectMgr_EntityOwner)& aStoredOwner = mystored.FindKey(anOwnerIdx);
  return aStoredOwner;
}

//=================================================================================================

const SelectMgr_SortCriterion& SelectMgr_ViewerSelector::PickedData(
  const Standard_Integer theRank) const
{
  Standard_OutOfRange_Raise_if(theRank < 1 || theRank > NbPicked(),
                               "SelectMgr_ViewerSelector::PickedData() out of range index");
  if (!myIsSorted)
  {
    SortResult();
  }

  const Standard_Integer anOwnerIdx = myIndexes.Value(theRank);
  return mystored.FindFromIndex(anOwnerIdx);
}

//===================================================
//
//       INTERNAL METHODS ....
//
//==================================================

//=================================================================================================

void SelectMgr_ViewerSelector::SetEntitySetBuilder(const Handle(Select3D_BVHBuilder3d)& theBuilder)
{
  myEntitySetBuilder = theBuilder;
  for (SelectMgr_MapOfObjectSensitives::Iterator aSetIter(myMapOfObjectSensitives); aSetIter.More();
       aSetIter.Next())
  {
    aSetIter.ChangeValue()->SetBuilder(myEntitySetBuilder);
  }
}

//=================================================================================================

Standard_Boolean SelectMgr_ViewerSelector::Contains(
  const Handle(SelectMgr_SelectableObject)& theObject) const
{
  return mySelectableObjects.Contains(theObject);
}

//==================================================
// Function: ActiveModes
// Purpose : return all the  modes with a given state for an object
//==================================================
Standard_Boolean SelectMgr_ViewerSelector::Modes(
  const Handle(SelectMgr_SelectableObject)& theSelectableObject,
  TColStd_ListOfInteger&                    theModeList,
  const SelectMgr_StateOfSelection          theWantedState) const
{
  Standard_Boolean hasActivatedStates = Contains(theSelectableObject);
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter(theSelectableObject->Selections());
       aSelIter.More();
       aSelIter.Next())
  {
    if (theWantedState == SelectMgr_SOS_Any)
    {
      theModeList.Append(aSelIter.Value()->Mode());
    }
    else if (theWantedState == aSelIter.Value()->GetSelectionState())
    {
      theModeList.Append(aSelIter.Value()->Mode());
    }
  }

  return hasActivatedStates;
}

//=================================================================================================

Standard_Boolean SelectMgr_ViewerSelector::IsActive(
  const Handle(SelectMgr_SelectableObject)& theSelectableObject,
  const Standard_Integer                    theMode) const
{
  if (!Contains(theSelectableObject))
    return Standard_False;

  const Handle(SelectMgr_Selection)& aSel = theSelectableObject->Selection(theMode);
  return !aSel.IsNull() && aSel->GetSelectionState() == SelectMgr_SOS_Activated;
}

//=================================================================================================

Standard_Boolean SelectMgr_ViewerSelector::IsInside(
  const Handle(SelectMgr_SelectableObject)& theSelectableObject,
  const Standard_Integer                    theMode) const
{
  if (!Contains(theSelectableObject))
    return Standard_False;

  const Handle(SelectMgr_Selection)& aSel = theSelectableObject->Selection(theMode);
  return !aSel.IsNull() && aSel->GetSelectionState() != SelectMgr_SOS_Unknown;
}

//=================================================================================================

SelectMgr_StateOfSelection SelectMgr_ViewerSelector::Status(
  const Handle(SelectMgr_Selection)& theSelection) const
{
  return theSelection->GetSelectionState();
}

//==================================================
// Function: Status
// Purpose : gives Information about selectors
//==================================================

TCollection_AsciiString SelectMgr_ViewerSelector::Status(
  const Handle(SelectMgr_SelectableObject)& theSelectableObject) const
{
  TCollection_AsciiString aStatus("Status Object :\n\t");
  for (SelectMgr_SequenceOfSelection::Iterator aSelIter(theSelectableObject->Selections());
       aSelIter.More();
       aSelIter.Next())
  {
    if (aSelIter.Value()->GetSelectionState() != SelectMgr_SOS_Unknown)
    {
      aStatus =
        aStatus + "Mode " + TCollection_AsciiString(aSelIter.Value()->Mode()) + " present - "
        + (aSelIter.Value()->GetSelectionState() == SelectMgr_SOS_Activated ? " Active \n\t"
                                                                            : " Inactive \n\t");
    }
  }

  if (!Contains(theSelectableObject))
  {
    aStatus = aStatus + "Not Present in the selector\n\n";
  }

  return aStatus;
}

//=================================================================================================

void SelectMgr_ViewerSelector::SortResult() const
{
  if (mystored.IsEmpty())
  {
    myIsSorted = true;
    return;
  }

  const Standard_Integer anExtent = mystored.Extent();
  if (anExtent != myIndexes.Length())
  {
    myIndexes.Resize(1, anExtent, false);
  }

  for (Standard_Integer anIndexIter = 1; anIndexIter <= anExtent; ++anIndexIter)
  {
    myIndexes.SetValue(anIndexIter, anIndexIter);
  }
  std::sort(myIndexes.begin(), myIndexes.end(), CompareResults(mystored, myToPreferClosest));
  myIsSorted = true;
}

//=======================================================================
// function : AddSelectableObject
// purpose  : Adds new object to the map of selectable objects
//=======================================================================
void SelectMgr_ViewerSelector::AddSelectableObject(
  const Handle(SelectMgr_SelectableObject)& theObject)
{
  if (!myMapOfObjectSensitives.IsBound(theObject))
  {
    mySelectableObjects.Append(theObject);
    Handle(SelectMgr_SensitiveEntitySet) anEntitySet =
      new SelectMgr_SensitiveEntitySet(myEntitySetBuilder);
    myMapOfObjectSensitives.Bind(theObject, anEntitySet);
  }
}

//=======================================================================
// function : AddSelectionToObject
// purpose  : Adds new selection to the object and builds its BVH tree
//=======================================================================
void SelectMgr_ViewerSelector::AddSelectionToObject(
  const Handle(SelectMgr_SelectableObject)& theObject,
  const Handle(SelectMgr_Selection)&        theSelection)
{
  if (Handle(SelectMgr_SensitiveEntitySet)* anEntitySet =
        myMapOfObjectSensitives.ChangeSeek(theObject))
  {
    (*anEntitySet)->Append(theSelection);
    (*anEntitySet)->BVH();
  }
  else
  {
    AddSelectableObject(theObject);
    AddSelectionToObject(theObject, theSelection);
  }
}

//=================================================================================================

void SelectMgr_ViewerSelector::MoveSelectableObject(
  const Handle(SelectMgr_SelectableObject)& theObject)
{
  mySelectableObjects.ChangeSubset(theObject);
}

//=======================================================================
// function : RemoveSelectableObject
// purpose  : Removes selectable object from map of selectable ones
//=======================================================================
void SelectMgr_ViewerSelector::RemoveSelectableObject(
  const Handle(SelectMgr_SelectableObject)& theObject)
{
  if (myMapOfObjectSensitives.UnBind(theObject))
  {
    RemovePicked(theObject);
    mySelectableObjects.Remove(theObject);
  }
}

//=======================================================================
// function : RemoveSelectionOfObject
// purpose  : Removes selection of the object and marks its BVH tree
//            for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RemoveSelectionOfObject(
  const Handle(SelectMgr_SelectableObject)& theObject,
  const Handle(SelectMgr_Selection)&        theSelection)
{
  if (Handle(SelectMgr_SensitiveEntitySet)* anEntitySet =
        myMapOfObjectSensitives.ChangeSeek(theObject))
  {
    (*anEntitySet)->Remove(theSelection);
  }
}

//=======================================================================
// function : RebuildObjectsTree
// purpose  : Marks BVH of selectable objects for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RebuildObjectsTree(const Standard_Boolean theIsForce)
{
  mySelectableObjects.MarkDirty();

  if (theIsForce)
  {
    Graphic3d_Vec2i aWinSize;
    mySelectingVolumeMgr.WindowSize(aWinSize.x(), aWinSize.y());
    mySelectableObjects.UpdateBVH(mySelectingVolumeMgr.Camera(), aWinSize);
  }
}

//=======================================================================
// function : RebuildSensitivesTree
// purpose  : Marks BVH of sensitive entities of particular selectable
//            object for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RebuildSensitivesTree(
  const Handle(SelectMgr_SelectableObject)& theObject,
  const Standard_Boolean                    theIsForce)
{
  if (!Contains(theObject))
    return;

  Handle(SelectMgr_SensitiveEntitySet)& anEntitySet = myMapOfObjectSensitives.ChangeFind(theObject);
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
  for (SelectMgr_MapOfObjectSensitivesIterator aSensitivesIter(myMapOfObjectSensitives);
       aSensitivesIter.More();
       aSensitivesIter.Next())
  {
    Handle(SelectMgr_SensitiveEntitySet)& anEntitySet  = aSensitivesIter.ChangeValue();
    const Standard_Integer                anEntitiesNb = anEntitySet->Size();
    for (Standard_Integer anIdx = 0; anIdx < anEntitiesNb; ++anIdx)
    {
      anEntitySet->GetSensitiveById(anIdx)->ResetSelectionActiveStatus();
    }
  }
}

//=======================================================================
// function : ActiveOwners
// purpose  : Returns the list of active entity owners
//=======================================================================
void SelectMgr_ViewerSelector::ActiveOwners(
  NCollection_List<Handle(SelectMgr_EntityOwner)>& theOwners) const
{
  for (SelectMgr_MapOfObjectSensitivesIterator anIter(myMapOfObjectSensitives); anIter.More();
       anIter.Next())
  {
    const Handle(SelectMgr_SensitiveEntitySet)& anEntitySet  = anIter.Value();
    const Standard_Integer                      anEntitiesNb = anEntitySet->Size();
    for (Standard_Integer anIdx = 0; anIdx < anEntitiesNb; ++anIdx)
    {
      const Handle(SelectMgr_SensitiveEntity)& aSensitive = anEntitySet->GetSensitiveById(anIdx);
      if (aSensitive->IsActiveForSelection())
      {
        theOwners.Append(aSensitive->BaseSensitive()->OwnerId());
      }
    }
  }
}

//=======================================================================
// function : AllowOverlapDetection
// purpose  : Sets the detection type: if theIsToAllow is false,
//           only fully included sensitives will be detected, otherwise
//           the algorithm will mark both included and overlapped entities
//           as matched
//=======================================================================
void SelectMgr_ViewerSelector::AllowOverlapDetection(const Standard_Boolean theIsToAllow)
{
  mySelectingVolumeMgr.AllowOverlapDetection(theIsToAllow);
}

//=================================================================================================

void SelectMgr_ViewerSelector::Pick(const Standard_Integer  theXPix,
                                    const Standard_Integer  theYPix,
                                    const Handle(V3d_View)& theView)
{
  updateZLayers(theView);

  gp_Pnt2d aMousePos(static_cast<Standard_Real>(theXPix), static_cast<Standard_Real>(theYPix));
  mySelectingVolumeMgr.InitPointSelectingVolume(aMousePos);

  mySelectingVolumeMgr.SetPixelTolerance(myTolerances.Tolerance());
  mySelectingVolumeMgr.SetCamera(theView->Camera());
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size(aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize(aWidth, aHeight);

  mySelectingVolumeMgr.BuildSelectingVolume();
  mySelectingVolumeMgr.SetViewClipping(theView->ClipPlanes(),
                                       Handle(Graphic3d_SequenceOfHClipPlane)(),
                                       NULL);

  TraverseSensitives(theView->View()->Identification());
}

//=================================================================================================

void SelectMgr_ViewerSelector::Pick(const Standard_Integer  theXPMin,
                                    const Standard_Integer  theYPMin,
                                    const Standard_Integer  theXPMax,
                                    const Standard_Integer  theYPMax,
                                    const Handle(V3d_View)& theView)
{
  updateZLayers(theView);

  gp_Pnt2d aMinMousePos(static_cast<Standard_Real>(theXPMin), static_cast<Standard_Real>(theYPMin));
  gp_Pnt2d aMaxMousePos(static_cast<Standard_Real>(theXPMax), static_cast<Standard_Real>(theYPMax));
  mySelectingVolumeMgr.InitBoxSelectingVolume(aMinMousePos, aMaxMousePos);

  mySelectingVolumeMgr.SetCamera(theView->Camera());
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size(aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize(aWidth, aHeight);

  mySelectingVolumeMgr.BuildSelectingVolume();
  mySelectingVolumeMgr.SetViewClipping(theView->ClipPlanes(),
                                       Handle(Graphic3d_SequenceOfHClipPlane)(),
                                       NULL);
  TraverseSensitives(theView->View()->Identification());
}

//=======================================================================
// Function: Pick
// Purpose : Selection using a polyline
//=======================================================================
void SelectMgr_ViewerSelector::Pick(const TColgp_Array1OfPnt2d& thePolyline,
                                    const Handle(V3d_View)&     theView)
{
  updateZLayers(theView);

  mySelectingVolumeMgr.InitPolylineSelectingVolume(thePolyline);
  mySelectingVolumeMgr.SetCamera(theView->Camera());
  Standard_Integer aWidth = 0, aHeight = 0;
  theView->Window()->Size(aWidth, aHeight);
  mySelectingVolumeMgr.SetWindowSize(aWidth, aHeight);
  mySelectingVolumeMgr.BuildSelectingVolume();
  mySelectingVolumeMgr.SetViewClipping(theView->ClipPlanes(),
                                       Handle(Graphic3d_SequenceOfHClipPlane)(),
                                       NULL);

  TraverseSensitives(theView->View()->Identification());
}

//=================================================================================================

void SelectMgr_ViewerSelector::Pick(const gp_Ax1& theAxis, const Handle(V3d_View)& theView)
{
  updateZLayers(theView);

  mySelectingVolumeMgr.InitAxisSelectingVolume(theAxis);
  mySelectingVolumeMgr.BuildSelectingVolume();
  mySelectingVolumeMgr.SetViewClipping(theView->ClipPlanes(),
                                       Handle(Graphic3d_SequenceOfHClipPlane)(),
                                       NULL);

  TraverseSensitives(theView->View()->Identification());
}

//=================================================================================================

Standard_Boolean SelectMgr_ViewerSelector::ToPixMap(Image_PixMap&                        theImage,
                                                    const Handle(V3d_View)&              theView,
                                                    const StdSelect_TypeOfSelectionImage theType,
                                                    const Standard_Integer thePickedIndex)
{
  if (theImage.IsEmpty())
  {
    throw Standard_ProgramError(
      "SelectMgr_ViewerSelector::ToPixMap() has been called with empty image");
  }

  Handle(SelectMgr_SelectionImageFiller) aFiller =
    SelectMgr_SelectionImageFiller::CreateFiller(theImage, this, theType);
  if (aFiller.IsNull())
  {
    return Standard_False;
  }

  const Standard_Integer aSizeX = static_cast<Standard_Integer>(theImage.SizeX());
  const Standard_Integer aSizeY = static_cast<Standard_Integer>(theImage.SizeY());
  for (Standard_Integer aRowIter = 0; aRowIter < aSizeY; ++aRowIter)
  {
    for (Standard_Integer aColIter = 0; aColIter < aSizeX; ++aColIter)
    {
      Pick(aColIter, aRowIter, theView);
      aFiller->Fill(aColIter, aRowIter, thePickedIndex);
    }
  }
  aFiller->Flush();
  return Standard_True;
}

//=======================================================================
// Function: DisplaySensitive.
// Purpose : Display active primitives.
//=======================================================================
void SelectMgr_ViewerSelector::DisplaySensitive(const Handle(V3d_View)& theView)
{
  for (SelectMgr_SelectableObjectSet::Iterator aSelectableIt(mySelectableObjects);
       aSelectableIt.More();
       aSelectableIt.Next())
  {
    Handle(Graphic3d_Structure) aStruct =
      new Graphic3d_Structure(theView->Viewer()->StructureManager());
    const Handle(SelectMgr_SelectableObject)& anObj = aSelectableIt.Value();
    for (SelectMgr_SequenceOfSelection::Iterator aSelIter(anObj->Selections()); aSelIter.More();
         aSelIter.Next())
    {
      if (aSelIter.Value()->GetSelectionState() == SelectMgr_SOS_Activated)
      {
        SelectMgr::ComputeSensitivePrs(aStruct,
                                       aSelIter.Value(),
                                       anObj->Transformation(),
                                       anObj->TransformPersistence());
      }
    }

    myStructs.Append(aStruct);
  }

  for (Graphic3d_SequenceOfStructure::Iterator aStructIter(myStructs); aStructIter.More();
       aStructIter.Next())
  {
    Handle(Graphic3d_Structure)& aStruct = aStructIter.ChangeValue();
    aStruct->SetDisplayPriority(Graphic3d_DisplayPriority_Topmost);
    aStruct->Display();
  }

  theView->Update();
}

//=================================================================================================

void SelectMgr_ViewerSelector::ClearSensitive(const Handle(V3d_View)& theView)
{
  for (Graphic3d_SequenceOfStructure::Iterator aStructIter(myStructs); aStructIter.More();
       aStructIter.Next())
  {
    const Handle(Graphic3d_Structure)& aPrs = aStructIter.ChangeValue();
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

//=================================================================================================

void SelectMgr_ViewerSelector::DisplaySensitive(const Handle(SelectMgr_Selection)& theSel,
                                                const gp_Trsf&                     theTrsf,
                                                const Handle(V3d_View)&            theView,
                                                const Standard_Boolean             theToClearOthers)
{
  if (theToClearOthers)
  {
    ClearSensitive(theView);
  }

  Handle(Graphic3d_Structure) aStruct =
    new Graphic3d_Structure(theView->Viewer()->StructureManager());

  SelectMgr::ComputeSensitivePrs(aStruct, theSel, theTrsf, Handle(Graphic3d_TransformPers)());

  myStructs.Append(aStruct);
  myStructs.Last()->SetDisplayPriority(Graphic3d_DisplayPriority_Topmost);
  myStructs.Last()->Display();

  theView->Update();
}

//=================================================================================================

void SelectMgr_ViewerSelector::DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myToPreferClosest)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mystored.Extent())

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &mySelectingVolumeMgr)
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, &mySelectableObjects)

  Standard_Integer aNbOfSelectableObjects = 0;
  for (SelectMgr_SelectableObjectSet::Iterator aSelectableIt(mySelectableObjects);
       aSelectableIt.More();
       aSelectableIt.Next())
  {
    aNbOfSelectableObjects++;
  }
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aNbOfSelectableObjects)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTolerances.Tolerance())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTolerances.CustomTolerance())
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myZLayerOrderMap.Extent())

  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myEntitySetBuilder.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCameraEye)
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCameraDir)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myCameraScale)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIndexes.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myIsLeftChildQueuedFirst)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMapOfObjectSensitives.Extent())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myStructs.Length())
  for (Graphic3d_SequenceOfStructure::Iterator aStructsIt(myStructs); aStructsIt.More();
       aStructsIt.Next())
  {
    const Handle(Graphic3d_Structure)& aStructure = aStructsIt.Value();
    OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, aStructure)
  }
}

//=================================================================================================

void SelectMgr_ViewerSelector::SetToPrebuildBVH(Standard_Boolean theToPrebuild,
                                                Standard_Integer theThreadsNum)
{
  if (!theToPrebuild && !myBVHThreadPool.IsNull())
  {
    myBVHThreadPool.Nullify();
  }
  else if (theToPrebuild)
  {
    myBVHThreadPool = new SelectMgr_BVHThreadPool(theThreadsNum);
  }
  myToPrebuildBVH = theToPrebuild;
}

//=================================================================================================

void SelectMgr_ViewerSelector::QueueBVHBuild(const Handle(Select3D_SensitiveEntity)& theEntity)
{
  if (myToPrebuildBVH)
  {
    myBVHThreadPool->AddEntity(theEntity);
  }
}

//=================================================================================================

void SelectMgr_ViewerSelector::WaitForBVHBuild()
{
  if (myToPrebuildBVH)
  {
    myBVHThreadPool->WaitThreads();
  }
}

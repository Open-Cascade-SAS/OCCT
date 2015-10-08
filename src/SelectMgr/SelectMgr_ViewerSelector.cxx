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

// Modified by  ...
//              ROB JAN/07/98 : Improve Storage of detected entities
//              AGV OCT/23/03 : Optimize the method SortResult() (OCC4201)

#include <BVH_Tree.hxx>
#include <gp_Pnt.hxx>
#include <OSD_Environment.hxx>
#include <Precision.hxx>
#include <SelectMgr_ViewerSelector.hxx>
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
}

//=======================================================================
// function: SelectMgr_ToleranceMap
// purpose : Sets tolerance values to -1.0
//=======================================================================
SelectMgr_ToleranceMap::SelectMgr_ToleranceMap()
{
  myLargestKey = -1;
  myCustomTolerance = -1;
}

//=======================================================================
// function: ~SelectMgr_ToleranceMap
// purpose :
//=======================================================================
SelectMgr_ToleranceMap::~SelectMgr_ToleranceMap()
{
  myTolerances.Clear();
}

//=======================================================================
// function: Add
// purpose : Adds the value given to map, checks if the current tolerance value
//           should be replaced by theTolerance
//=======================================================================
void SelectMgr_ToleranceMap::Add (const Standard_Integer& theTolerance)
{
  if (myTolerances.IsBound (theTolerance))
  {
    Standard_Integer& aFreq = myTolerances.ChangeFind (theTolerance);
    aFreq++;

    if (aFreq == 1 && theTolerance != myLargestKey)
      myLargestKey = Max (theTolerance, myLargestKey);
  }
  else
  {
    if (myTolerances.IsEmpty())
    {
      myTolerances.Bind (theTolerance, 1);
      myLargestKey = theTolerance;
      return;
    }

    myTolerances.Bind (theTolerance, 1);
    myLargestKey = Max (theTolerance, myLargestKey);
  }
}

//=======================================================================
// function: Decrement
// purpose : Decrements a counter of the tolerance given, checks if the current tolerance value
//           should be recalculated
//=======================================================================
void SelectMgr_ToleranceMap::Decrement (const Standard_Integer& theTolerance)
{
  if (myTolerances.IsBound (theTolerance))
  {
    Standard_Integer& aFreq = myTolerances.ChangeFind (theTolerance);
    aFreq--;

    if (Abs (theTolerance - myLargestKey) < Precision::Confusion() && aFreq == 0)
    {
      myLargestKey = 0;
      for (NCollection_DataMap<Standard_Integer, Standard_Integer>::Iterator anIter (myTolerances); anIter.More(); anIter.Next())
      {
        if (anIter.Value() != 0)
          myLargestKey = Max (myLargestKey, anIter.Key());
      }
    }
  }
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
  myMapOfDetected.Clear();
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
                                             const Standard_Integer theEntityIdx,
                                             SelectMgr_SelectingVolumeManager& theMgr)
{
  Handle(SelectMgr_EntityOwner) anOwner (Handle(SelectMgr_EntityOwner)::DownCast (theEntity->OwnerId()));

  SelectBasics_PickResult aPickResult;
  if (theEntity->Matches (theMgr, aPickResult))
  {
    if (!anOwner.IsNull())
    {
      if (HasDepthClipping (anOwner) && theMgr.GetActiveSelectionType() == SelectMgr_SelectingVolumeManager::Point)
      {
        Standard_Boolean isClipped = theMgr.IsClipped (anOwner->Selectable()->GetClipPlanes(),
                                                       aPickResult.Depth());
        if (isClipped)
          return;
      }

      Standard_Integer aPriority = anOwner->Priority();

      SelectMgr_SortCriterion aCriterion (aPriority, aPickResult.Depth(), aPickResult.DistToGeomCenter(), theEntity->SensitivityFactor() / 33.0, preferclosest);
      if (mystored.Contains (anOwner))
      {
        if (theMgr.GetActiveSelectionType() != 1)
        {
          SelectMgr_SortCriterion& aPrevCriterion = mystored.ChangeFromKey (anOwner);
          if (aCriterion > aPrevCriterion)
          {
            aPrevCriterion = aCriterion;
            myMapOfDetected.ChangeFind (anOwner) = theEntityIdx;
          }
        }
      }
      else
      {
        mystored.Add (anOwner, aCriterion);
        myMapOfDetected.Bind (anOwner, theEntityIdx);
      }
    }
  }
}

//=======================================================================
// function: computeFrustum
// purpose : Internal function that checks if a current selecting frustum
//           needs to be scaled and transformed for the entity and performs
//           necessary calculations
//=======================================================================
void SelectMgr_ViewerSelector::computeFrustum (const Handle(SelectBasics_SensitiveEntity)& theEnt,
                                               const gp_Trsf&                              theInvTrsf,
                                               SelectMgr_FrustumCache&                     theCachedMgrs,
                                               SelectMgr_SelectingVolumeManager&           theResMgr)
{
  Standard_Integer aScale = isToScaleFrustum (theEnt) ? sensitivity (theEnt) : 1;
  const gp_Trsf aTrsfMtr = theEnt->HasInitLocation() ? theEnt->InvInitLocation() * theInvTrsf : theInvTrsf;
  const Standard_Boolean toScale = aScale != 1;
  const Standard_Boolean toTransform = aTrsfMtr.Form() != gp_Identity;
  if (toScale && toTransform)
  {
    theResMgr = mySelectingVolumeMgr.ScaleAndTransform (aScale, aTrsfMtr);
  }
  else if (toScale)
  {
    if (!theCachedMgrs.IsBound (aScale))
    {
      theCachedMgrs.Bind (aScale, mySelectingVolumeMgr.ScaleAndTransform (aScale, gp_Trsf()));
    }
    theResMgr = theCachedMgrs.Find (aScale);
  }
  else if (toTransform)
  {
    theResMgr = mySelectingVolumeMgr.ScaleAndTransform (1, aTrsfMtr);
  }
}

//=======================================================================
// function: traverseObject
// purpose : Internal function that checks if there is possible overlap
//           between some entity of selectable object theObject and
//           current selecting volume
//=======================================================================
void SelectMgr_ViewerSelector::traverseObject (const Handle(SelectMgr_SelectableObject)& theObject)
{
  NCollection_Handle<SelectMgr_SensitiveEntitySet>& anEntitySet =
    myMapOfObjectSensitives.ChangeFind (theObject);

  if (anEntitySet->Size() == 0)
    return;

  const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& aSensitivesTree = anEntitySet->BVH();

  gp_Trsf aInversedTrsf;

  if (theObject->HasTransformation() || theObject->TransformPersistence().Flags)
  {
    if (!theObject->TransformPersistence().Flags)
    {
      aInversedTrsf = theObject->InversedTransformation();
    }
    else
    {
      const Graphic3d_Mat4d& aProjection = mySelectingVolumeMgr.ProjectionMatrix();
      const Graphic3d_Mat4d& aWorldView  = mySelectingVolumeMgr.WorldViewMatrix();

      gp_Trsf aTPers;
      Graphic3d_Mat4d aMat = theObject->TransformPersistence().Compute (aProjection, aWorldView, 0, 0);
      aTPers.SetValues (aMat.GetValue (0, 0), aMat.GetValue (0, 1), aMat.GetValue (0, 2), aMat.GetValue (0, 3),
                        aMat.GetValue (1, 0), aMat.GetValue (1, 1), aMat.GetValue (1, 2), aMat.GetValue (1, 3),
                        aMat.GetValue (2, 0), aMat.GetValue (2, 1), aMat.GetValue (2, 2), aMat.GetValue (2, 3));

      aInversedTrsf = (aTPers * theObject->Transformation()).Inverted();
    }
  }

  SelectMgr_SelectingVolumeManager aMgr = aInversedTrsf.Form() != gp_Identity
                                        ? mySelectingVolumeMgr.ScaleAndTransform (1, aInversedTrsf)
                                        : mySelectingVolumeMgr;

  SelectMgr_FrustumCache aScaledTrnsfFrustums;

  Standard_Integer aNode = 0; // a root node
  if (!aMgr.Overlaps (aSensitivesTree->MinPoint (0),
                      aSensitivesTree->MaxPoint (0)))
  {
    return;
  }
  Standard_Integer aStack[32];
  Standard_Integer aHead = -1;
  for (;;)
  {
    if (!aSensitivesTree->IsOuter (aNode))
    {
      const Standard_Integer aLeftChildIdx  = aSensitivesTree->LeftChild  (aNode);
      const Standard_Integer aRightChildIdx = aSensitivesTree->RightChild (aNode);
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
          computeFrustum (anEnt, aInversedTrsf, aScaledTrnsfFrustums, aTmpMgr);
          checkOverlap (anEnt, anIdx, aTmpMgr);
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
  myMapOfDetected.Clear();

  NCollection_Handle<BVH_Tree<Standard_Real, 3> > aBVHTree;
  for (Standard_Integer aBVHTreeIdx = 0; aBVHTreeIdx < 2; ++aBVHTreeIdx)
  {
    const Standard_Boolean isTrsfPers = aBVHTreeIdx == 1;
    if (isTrsfPers)
    {
      if (mySelectableObjectsTrsfPers.Size() == 0)
      {
        continue;
      }
      const Graphic3d_Mat4d& aProjection            = mySelectingVolumeMgr.ProjectionMatrix();
      const Graphic3d_Mat4d& aWorldView             = mySelectingVolumeMgr.WorldViewMatrix();
      const Graphic3d_WorldViewProjState& aWVPState = mySelectingVolumeMgr.WorldViewProjState();
      aBVHTree = mySelectableObjectsTrsfPers.BVH (aProjection, aWorldView, aWVPState);
    }
    else
    {
      if (mySelectableObjects.Size() == 0)
      {
        continue;
      }
      aBVHTree = mySelectableObjects.BVH();
    }

    Standard_Integer aNode = 0;
    if (!mySelectingVolumeMgr.Overlaps (aBVHTree->MinPoint (0),
                                        aBVHTree->MaxPoint (0)))
    {
      continue;
    }

    Standard_Integer aStack[32];
    Standard_Integer aHead = -1;
    for (;;)
    {
      if (!aBVHTree->IsOuter (aNode))
      {
        const Standard_Integer aLeftChildIdx  = aBVHTree->LeftChild  (aNode);
        const Standard_Integer aRightChildIdx = aBVHTree->RightChild (aNode);
        const Standard_Boolean isLeftChildIn  =
          mySelectingVolumeMgr.Overlaps (aBVHTree->MinPoint (aLeftChildIdx),
                                         aBVHTree->MaxPoint (aLeftChildIdx));
        const Standard_Boolean isRightChildIn =
          mySelectingVolumeMgr.Overlaps (aBVHTree->MinPoint (aRightChildIdx),
                                         aBVHTree->MaxPoint (aRightChildIdx));
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
          Handle(SelectMgr_SelectableObject) aSelectableObject =
            isTrsfPers ? mySelectableObjectsTrsfPers.GetObjectById (anIdx)
                       : mySelectableObjects.GetObjectById (anIdx);

          traverseObject (aSelectableObject);
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
//function : More
//purpose  :
//=======================================================================
Standard_Boolean SelectMgr_ViewerSelector::More()
{
  if(mystored.Extent()==0) return Standard_False;
  if(myCurRank==0) return Standard_False;
  return myCurRank <= myIndexes->Length();
}

//==================================================
// Function: OnePicked
// Purpose : only the best one is chosen
//           depend on priority and mindist...
//==================================================

Handle(SelectMgr_EntityOwner) SelectMgr_ViewerSelector
::OnePicked()
{

  Init();
  if(More()){
    Standard_Integer RankInMap = myIndexes->Value (myIndexes->Lower());
    const Handle(SelectBasics_EntityOwner)& toto = mystored.FindKey(RankInMap);
    Handle(SelectMgr_EntityOwner) Ownr = Handle(SelectMgr_EntityOwner)::DownCast (toto);
    return Ownr;
  }

  Handle (SelectMgr_EntityOwner) NullObj; //returns a null Handle if there was not successfull pick...
  return NullObj;
}


//=======================================================================
//function : NbPicked
//purpose  :
//=======================================================================

Standard_Integer SelectMgr_ViewerSelector::NbPicked() const
{
  return mystored.Extent();
}
//=======================================================================
//function : Picked
//purpose  :
//=======================================================================
Handle(SelectMgr_EntityOwner) SelectMgr_ViewerSelector::Picked(const Standard_Integer aRank) const
{

  Handle(SelectMgr_EntityOwner) anOwner;
  if (aRank < 1 || aRank > NbPicked())
    return anOwner;
  Standard_Integer anOwnerIdx = myIndexes->Value (aRank);


  const Handle(SelectBasics_EntityOwner)& aStoredOwner = mystored.FindKey (anOwnerIdx);
  anOwner = Handle(SelectMgr_EntityOwner)::DownCast (aStoredOwner);
  return anOwner;
}

//===================================================
//
//       INTERNAL METHODS ....
//
//==================================================

//==================================================
// Function: Contains
// Purpose :
//==================================================
Standard_Boolean SelectMgr_ViewerSelector::Contains (const Handle(SelectMgr_SelectableObject)& theObject) const
{
  return mySelectableObjects.Contains (theObject)
      || mySelectableObjectsTrsfPers.Contains (theObject);
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

  // to work faster...
  TColStd_Array1OfInteger& thearr = myIndexes->ChangeArray1();

  // indices from 1 to N are loaded
  Standard_Integer I ;
  for (I=1; I <= anExtent; I++)
    thearr(I)=I;

  std::sort (thearr.begin(), thearr.end(), CompareResults (mystored));

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
    if (!theObject->TransformPersistence().Flags)
    {
      mySelectableObjects.Append (theObject);
    }
    else
    {
      mySelectableObjectsTrsfPers.Append (theObject);
    }

    NCollection_Handle<SelectMgr_SensitiveEntitySet> anEntitySet = new SelectMgr_SensitiveEntitySet();
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
  if (myMapOfObjectSensitives.IsBound (theObject))
  {
    NCollection_Handle<SelectMgr_SensitiveEntitySet>& anEntitySet =
      myMapOfObjectSensitives.ChangeFind (theObject);
    anEntitySet->Append (theSelection);
    anEntitySet->BVH();
  }
  else
  {
    AddSelectableObject (theObject);
    AddSelectionToObject (theObject, theSelection);
  }
}

//=======================================================================
// function : RemoveSelectableObject
// purpose  : Removes selectable object from map of selectable ones
//=======================================================================
void SelectMgr_ViewerSelector::RemoveSelectableObject (const Handle(SelectMgr_SelectableObject)& theObject)
{
  if (myMapOfObjectSensitives.IsBound (theObject))
  {
    if (!mySelectableObjects.Remove (theObject))
    {
      mySelectableObjectsTrsfPers.Remove (theObject);
    }
    myMapOfObjectSensitives.UnBind (theObject);
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
  if (myMapOfObjectSensitives.IsBound (theObject))
  {
    NCollection_Handle<SelectMgr_SensitiveEntitySet>& anEntitySet =
      myMapOfObjectSensitives.ChangeFind (theObject);
    anEntitySet->Remove (theSelection);
  }
}

//=======================================================================
// function : RebuildObjectsTree
// purpose  : Marks BVH of selectable objects for rebuild
//=======================================================================
void SelectMgr_ViewerSelector::RebuildObjectsTree (const Standard_Boolean theIsForce)
{
  mySelectableObjects.MarkDirty();
  mySelectableObjectsTrsfPers.MarkDirty();

  if (theIsForce)
  {
    const Graphic3d_Mat4d& aProjection            = mySelectingVolumeMgr.ProjectionMatrix();
    const Graphic3d_Mat4d& aWorldView             = mySelectingVolumeMgr.WorldViewMatrix();
    const Graphic3d_WorldViewProjState& aWVPState = mySelectingVolumeMgr.WorldViewProjState();

    mySelectableObjects.BVH();
    mySelectableObjectsTrsfPers.BVH (aProjection, aWorldView, aWVPState);
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

  NCollection_Handle<SelectMgr_SensitiveEntitySet>& anEntitySet = myMapOfObjectSensitives.ChangeFind (theObject);
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
  SelectMgr_MapOfObjectSensitivesIterator aSensitivesIter (myMapOfObjectSensitives);
  for ( ; aSensitivesIter.More(); aSensitivesIter.Next())
  {
    NCollection_Handle<SelectMgr_SensitiveEntitySet>& anEntitySet =
      aSensitivesIter.ChangeValue();
    Standard_Integer anEntitiesNb = anEntitySet->Size();
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
  const Handle(SelectMgr_EntityOwner)& anOwner = myDetectedIter.Key();
  const Handle(SelectMgr_SelectableObject)& anObject = anOwner->Selectable();
  const NCollection_Handle<SelectMgr_SensitiveEntitySet>& anEntitySet =
    myMapOfObjectSensitives.Find (anObject);

  return anEntitySet->GetSensitiveById (myDetectedIter.Value())->BaseSensitive();
}

//=======================================================================
// function : ActiveOwners
// purpose  : Returns the list of active entity owners
//=======================================================================
void SelectMgr_ViewerSelector::ActiveOwners (NCollection_List<Handle(SelectBasics_EntityOwner)>& theOwners) const
{
  for (SelectMgr_MapOfObjectSensitivesIterator anIter (myMapOfObjectSensitives); anIter.More(); anIter.Next())
  {
    const NCollection_Handle<SelectMgr_SensitiveEntitySet>& anEntitySet = anIter.Value();
    Standard_Integer anEntitiesNb = anEntitySet->Size();
    for (Standard_Integer anIdx = 0; anIdx < anEntitiesNb; ++anIdx)
    {
      if (anEntitySet->GetSensitiveById (anIdx)->IsActiveForSelection())
      {
        theOwners.Append (anEntitySet->GetSensitiveById (anIdx)->BaseSensitive()->OwnerId());
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

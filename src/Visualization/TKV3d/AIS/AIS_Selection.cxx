// Copyright (c) 1998-1999 Matra Datavision
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

#include <AIS_Selection.hxx>

#include <AIS_InteractiveObject.hxx>
#include <AIS_SelectionScheme.hxx>
#include <SelectMgr_Filter.hxx>

IMPLEMENT_STANDARD_RTTIEXT(AIS_Selection, Standard_Transient)

namespace
{
constexpr int THE_MaxSizeOfResult = 100000;
}

//=================================================================================================

AIS_Selection::AIS_Selection()
{
  // for maximum performance on medium selections (< 100000 objects)
  myResultMap.ReSize(THE_MaxSizeOfResult);
}

//=================================================================================================

void AIS_Selection::Clear()
{
  for (NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator aSelIter(Objects());
       aSelIter.More();
       aSelIter.Next())
  {
    const occ::handle<SelectMgr_EntityOwner> anObject = aSelIter.Value();
    anObject->SetSelected(false);
  }
  myresult.Clear();
  myResultMap.Clear();
  myIterator = NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator();
}

//=================================================================================================

AIS_SelectStatus AIS_Selection::Select(const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                       const occ::handle<SelectMgr_Filter>&      theFilter,
                                       const AIS_SelectionScheme                 theSelScheme,
                                       const bool                                theIsDetected)
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
  {
    return AIS_SS_NotDone;
  }

  const bool isDetected = theIsDetected && (theFilter.IsNull() || theFilter->IsOk(theOwner));

  const bool wasSelected = theOwner->IsSelected();
  const bool toSelect    = theOwner->Select(theSelScheme, isDetected);

  if (!wasSelected || !myResultMap.IsBound(theOwner))
  {
    if (!toSelect)
      return AIS_SS_NotDone;

    NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator aListIter;
    myresult.Append(theOwner, aListIter);
    myResultMap.Bind(theOwner, aListIter);
    theOwner->SetSelected(true);
    return AIS_SS_Added;
  }

  NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator aListIter =
    myResultMap.Find(theOwner);
  if (myIterator == aListIter)
  {
    if (myIterator.More())
    {
      myIterator.Next();
    }
    else
    {
      myIterator = NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator();
    }
  }

  // In the mode of advanced mesh selection only one owner is created for all selection modes.
  // It is necessary to check the current detected entity
  // and remove the owner from map only if the detected entity is the same as previous selected
  // (IsForcedHilight call)
  if (theOwner->IsForcedHilight())
  {
    return AIS_SS_Added;
  }

  myresult.Remove(aListIter);
  myResultMap.UnBind(theOwner);
  theOwner->SetSelected(false);

  // update list iterator for next object in <myresult> list if any
  if (aListIter.More())
  {
    const occ::handle<SelectMgr_EntityOwner>& aNextObject = aListIter.Value();
    if (myResultMap.IsBound(aNextObject))
    {
      myResultMap(aNextObject) = aListIter;
    }
    else
    {
      myResultMap.Bind(aNextObject, aListIter);
    }
  }
  return AIS_SS_Removed;
}

//=================================================================================================

AIS_SelectStatus AIS_Selection::AddSelect(const occ::handle<SelectMgr_EntityOwner>& theObject)
{
  if (theObject.IsNull() || !theObject->HasSelectable() || myResultMap.IsBound(theObject))
  {
    return AIS_SS_NotDone;
  }

  NCollection_List<occ::handle<SelectMgr_EntityOwner>>::Iterator aListIter;
  myresult.Append(theObject, aListIter);
  myResultMap.Bind(theObject, aListIter);
  theObject->SetSelected(true);
  return AIS_SS_Added;
}

//=================================================================================================

void AIS_Selection::SelectOwners(
  const NCollection_Array1<occ::handle<SelectMgr_EntityOwner>>& thePickedOwners,
  const AIS_SelectionScheme                                     theSelScheme,
  const bool                                                    theToAllowSelOverlap,
  const occ::handle<SelectMgr_Filter>&                          theFilter)
{
  (void)theToAllowSelOverlap;

  if (theSelScheme == AIS_SelectionScheme_ReplaceExtra && thePickedOwners.Size() == myresult.Size())
  {
    // If picked owners is equivalent to the selected then just clear selected.
    bool isTheSame = true;
    for (NCollection_Array1<occ::handle<SelectMgr_EntityOwner>>::Iterator aPickedIter(
           thePickedOwners);
         aPickedIter.More();
         aPickedIter.Next())
    {
      if (!myResultMap.IsBound(aPickedIter.Value()))
      {
        isTheSame = false;
        break;
      }
    }
    if (isTheSame)
    {
      Clear();
      return;
    }
  }

  if (theSelScheme == AIS_SelectionScheme_Replace
      || theSelScheme == AIS_SelectionScheme_ReplaceExtra
      || theSelScheme == AIS_SelectionScheme_Clear)
  {
    Clear();
  }

  for (NCollection_Array1<occ::handle<SelectMgr_EntityOwner>>::Iterator aPickedIter(
         thePickedOwners);
       aPickedIter.More();
       aPickedIter.Next())
  {
    const occ::handle<SelectMgr_EntityOwner>& anOwner = aPickedIter.Value();
    Select(anOwner, theFilter, theSelScheme, true);
  }
}

//=================================================================================================

AIS_SelectStatus AIS_Selection::appendOwner(const occ::handle<SelectMgr_EntityOwner>& theOwner,
                                            const occ::handle<SelectMgr_Filter>&      theFilter)
{
  if (theOwner.IsNull() || !theOwner->HasSelectable() || !theFilter->IsOk(theOwner))
  {
    return AIS_SS_NotDone;
  }

  return AddSelect(theOwner);
}

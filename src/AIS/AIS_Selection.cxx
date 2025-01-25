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
static const Standard_Integer THE_MaxSizeOfResult = 100000;
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
  for (AIS_NListOfEntityOwner::Iterator aSelIter(Objects()); aSelIter.More(); aSelIter.Next())
  {
    const Handle(SelectMgr_EntityOwner) anObject = aSelIter.Value();
    anObject->SetSelected(Standard_False);
  }
  myresult.Clear();
  myResultMap.Clear();
  myIterator = AIS_NListOfEntityOwner::Iterator();
}

//=================================================================================================

AIS_SelectStatus AIS_Selection::Select(const Handle(SelectMgr_EntityOwner)& theOwner,
                                       const Handle(SelectMgr_Filter)&      theFilter,
                                       const AIS_SelectionScheme            theSelScheme,
                                       const Standard_Boolean               theIsDetected)
{
  if (theOwner.IsNull() || !theOwner->HasSelectable())
  {
    return AIS_SS_NotDone;
  }

  const Standard_Boolean isDetected =
    theIsDetected && (theFilter.IsNull() || theFilter->IsOk(theOwner));

  const Standard_Boolean wasSelected = theOwner->IsSelected();
  const Standard_Boolean toSelect    = theOwner->Select(theSelScheme, isDetected);

  if (toSelect && !wasSelected)
  {
    AIS_NListOfEntityOwner::Iterator aListIter;
    myresult.Append(theOwner, aListIter);
    myResultMap.Bind(theOwner, aListIter);
    theOwner->SetSelected(Standard_True);
    return AIS_SS_Added;
  }

  if (!toSelect && !wasSelected)
  {
    return AIS_SS_NotDone;
  }

  AIS_NListOfEntityOwner::Iterator aListIter = myResultMap.Find(theOwner);
  if (myIterator == aListIter)
  {
    if (myIterator.More())
    {
      myIterator.Next();
    }
    else
    {
      myIterator = AIS_NListOfEntityOwner::Iterator();
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
  theOwner->SetSelected(Standard_False);

  // update list iterator for next object in <myresult> list if any
  if (aListIter.More())
  {
    const Handle(SelectMgr_EntityOwner)& aNextObject = aListIter.Value();
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

AIS_SelectStatus AIS_Selection::AddSelect(const Handle(SelectMgr_EntityOwner)& theObject)
{
  if (theObject.IsNull() || !theObject->HasSelectable() || myResultMap.IsBound(theObject))
  {
    return AIS_SS_NotDone;
  }

  AIS_NListOfEntityOwner::Iterator aListIter;
  myresult.Append(theObject, aListIter);
  myResultMap.Bind(theObject, aListIter);
  theObject->SetSelected(Standard_True);
  return AIS_SS_Added;
}

//=================================================================================================

void AIS_Selection::SelectOwners(const AIS_NArray1OfEntityOwner& thePickedOwners,
                                 const AIS_SelectionScheme       theSelScheme,
                                 const Standard_Boolean          theToAllowSelOverlap,
                                 const Handle(SelectMgr_Filter)& theFilter)
{
  (void)theToAllowSelOverlap;

  if (theSelScheme == AIS_SelectionScheme_ReplaceExtra && thePickedOwners.Size() == myresult.Size())
  {
    // If picked owners is equivalent to the selected then just clear selected.
    Standard_Boolean isTheSame = Standard_True;
    for (AIS_NArray1OfEntityOwner::Iterator aPickedIter(thePickedOwners); aPickedIter.More();
         aPickedIter.Next())
    {
      if (!myResultMap.IsBound(aPickedIter.Value()))
      {
        isTheSame = Standard_False;
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

  for (AIS_NArray1OfEntityOwner::Iterator aPickedIter(thePickedOwners); aPickedIter.More();
       aPickedIter.Next())
  {
    const Handle(SelectMgr_EntityOwner)& anOwner = aPickedIter.Value();
    Select(anOwner, theFilter, theSelScheme, true);
  }
}

//=================================================================================================

AIS_SelectStatus AIS_Selection::appendOwner(const Handle(SelectMgr_EntityOwner)& theOwner,
                                            const Handle(SelectMgr_Filter)&      theFilter)
{
  if (theOwner.IsNull() || !theOwner->HasSelectable() || !theFilter->IsOk(theOwner))
  {
    return AIS_SS_NotDone;
  }

  return AddSelect(theOwner);
}

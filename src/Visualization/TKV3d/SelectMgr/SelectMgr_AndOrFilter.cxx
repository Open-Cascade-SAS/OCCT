// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <SelectMgr_AndOrFilter.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_Filter.hxx>
#include <NCollection_List.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_AndOrFilter, SelectMgr_CompositionFilter)

//=================================================================================================

SelectMgr_AndOrFilter::SelectMgr_AndOrFilter(const SelectMgr_FilterType theFilterType)
    : myFilterType(theFilterType)
{
}

//=================================================================================================

void SelectMgr_AndOrFilter::SetDisabledObjects(const occ::handle<NCollection_Shared<NCollection_Map<const Standard_Transient*>>>& theObjects)
{
  myDisabledObjects = theObjects;
}

//=================================================================================================

bool SelectMgr_AndOrFilter::IsOk(const occ::handle<SelectMgr_EntityOwner>& theObj) const
{
  const SelectMgr_SelectableObject* aSelectable = theObj->Selectable().operator->();
  if (!myDisabledObjects.IsNull() && myDisabledObjects->Contains(aSelectable))
  {
    return false;
  }

  for (NCollection_List<occ::handle<SelectMgr_Filter>>::Iterator anIter(myFilters); anIter.More(); anIter.Next())
  {
    bool isOK = anIter.Value()->IsOk(theObj);
    if (isOK && myFilterType == SelectMgr_FilterType_OR)
    {
      return true;
    }
    else if (!isOK && myFilterType == SelectMgr_FilterType_AND)
    {
      return false;
    }
  }

  if (myFilterType == SelectMgr_FilterType_OR && !myFilters.IsEmpty())
  {
    return false;
  }
  return true;
}

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


#include <SelectMgr_EntityOwner.hxx>
#include <SelectMgr_Filter.hxx>
#include <SelectMgr_ListIteratorOfListOfFilter.hxx>
#include <SelectMgr_OrFilter.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_OrFilter,SelectMgr_CompositionFilter)

//=============================================================================
//function : SelectMgr_OrFilter
//purpose  :
//=============================================================================
SelectMgr_OrFilter::SelectMgr_OrFilter()
{
}

//=============================================================================
//function : SetDisabledObjects
//purpose  :
//=============================================================================
void SelectMgr_OrFilter::SetDisabledObjects (const Handle(Graphic3d_NMapOfTransient)& theObjects)
{
  myDisabledObjects = theObjects;
}

//=============================================================================
//function : IsOk
//purpose  :
//=============================================================================
Standard_Boolean SelectMgr_OrFilter::IsOk (const Handle(SelectMgr_EntityOwner)& theObj) const
{
  const SelectMgr_SelectableObject* aSelectable = theObj->Selectable().operator->();
  if (!myDisabledObjects.IsNull()
    && myDisabledObjects->Contains (aSelectable))
  {
    return Standard_False;
  }
  else if (myFilters.IsEmpty())
  {
    return Standard_True;
  }

  for (SelectMgr_ListIteratorOfListOfFilter aFilterIter (myFilters); aFilterIter.More(); aFilterIter.Next())
  {
    if (aFilterIter.Value()->IsOk (theObj))
    {
      return Standard_True;
    }
  }

  return Standard_False;
}

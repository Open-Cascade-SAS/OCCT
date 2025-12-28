// Created on: 1996-01-29
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#include <SelectMgr_CompositionFilter.hxx>
#include <SelectMgr_Filter.hxx>
#include <NCollection_List.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_CompositionFilter, SelectMgr_Filter)

void SelectMgr_CompositionFilter::Add(const occ::handle<SelectMgr_Filter>& afilter)
{
  myFilters.Append(afilter);
}

void SelectMgr_CompositionFilter::Remove(const occ::handle<SelectMgr_Filter>& afilter)
{
  NCollection_List<occ::handle<SelectMgr_Filter>>::Iterator It(myFilters);
  for (; It.More(); It.Next())
  {
    if (afilter == It.Value())
    {
      myFilters.Remove(It);
      return;
    }
  }
}

bool SelectMgr_CompositionFilter::IsEmpty() const
{
  return myFilters.IsEmpty();
}

bool SelectMgr_CompositionFilter::IsIn(const occ::handle<SelectMgr_Filter>& afilter) const
{
  NCollection_List<occ::handle<SelectMgr_Filter>>::Iterator It(myFilters);
  for (; It.More(); It.Next())
    if (afilter == It.Value())
      return true;
  return false;
}

void SelectMgr_CompositionFilter::Clear()
{
  myFilters.Clear();
}

bool SelectMgr_CompositionFilter::ActsOn(const TopAbs_ShapeEnum aStandardMode) const
{
  NCollection_List<occ::handle<SelectMgr_Filter>>::Iterator It(myFilters);
  for (; It.More(); It.Next())
  {
    if (It.Value()->ActsOn(aStandardMode))
      return true;
  }

  return false;
}

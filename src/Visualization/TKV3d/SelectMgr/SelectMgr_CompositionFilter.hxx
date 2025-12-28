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

#ifndef _SelectMgr_CompositionFilter_HeaderFile
#define _SelectMgr_CompositionFilter_HeaderFile

#include <SelectMgr_Filter.hxx>
#include <NCollection_List.hxx>
#include <TopAbs_ShapeEnum.hxx>

//! A framework to define a compound filter composed of
//! two or more simple filters.
class SelectMgr_CompositionFilter : public SelectMgr_Filter
{

public:
  //! Adds the filter afilter to a filter object created by a
  //! filter class inheriting this framework.
  Standard_EXPORT void Add(const occ::handle<SelectMgr_Filter>& afilter);

  //! Removes the filter aFilter from this framework.
  Standard_EXPORT void Remove(const occ::handle<SelectMgr_Filter>& aFilter);

  //! Returns true if this framework is empty.
  Standard_EXPORT bool IsEmpty() const;

  //! Returns true if the filter aFilter is in this framework.
  Standard_EXPORT bool IsIn(const occ::handle<SelectMgr_Filter>& aFilter) const;

  //! Returns the list of stored filters from this framework.
  const NCollection_List<occ::handle<SelectMgr_Filter>>& StoredFilters() const { return myFilters; }

  //! Clears the filters used in this framework.
  Standard_EXPORT void Clear();

  Standard_EXPORT virtual bool ActsOn(const TopAbs_ShapeEnum aStandardMode) const override;

  DEFINE_STANDARD_RTTIEXT(SelectMgr_CompositionFilter, SelectMgr_Filter)

protected:
  NCollection_List<occ::handle<SelectMgr_Filter>> myFilters;
};

#endif // _SelectMgr_CompositionFilter_HeaderFile

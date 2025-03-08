// Created on: 1996-09-17
// Created by: Odile Olivier
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

#ifndef _StdSelect_ShapeTypeFilter_HeaderFile
#define _StdSelect_ShapeTypeFilter_HeaderFile

#include <TopAbs_ShapeEnum.hxx>
#include <SelectMgr_Filter.hxx>

class SelectMgr_EntityOwner;

DEFINE_STANDARD_HANDLE(StdSelect_ShapeTypeFilter, SelectMgr_Filter)

//! A filter framework which allows you to define a filter for a specific shape type.
class StdSelect_ShapeTypeFilter : public SelectMgr_Filter
{
  DEFINE_STANDARD_RTTIEXT(StdSelect_ShapeTypeFilter, SelectMgr_Filter)
public:
  //! Constructs a filter object defined by the shape type aType.
  Standard_EXPORT StdSelect_ShapeTypeFilter(const TopAbs_ShapeEnum aType);

  //! Returns the type of shape selected by the filter.
  TopAbs_ShapeEnum Type() const { return myType; }

  Standard_EXPORT virtual Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& anobj) const
    Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean ActsOn(const TopAbs_ShapeEnum aStandardMode) const
    Standard_OVERRIDE;

private:
  TopAbs_ShapeEnum myType;
};

#endif // _StdSelect_ShapeTypeFilter_HeaderFile

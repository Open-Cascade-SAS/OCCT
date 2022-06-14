// Created on: 1995-12-04
// Created by: Stephane MORTAUD
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

#ifndef _SelectMgr_OrFilter_HeaderFile
#define _SelectMgr_OrFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <SelectMgr_CompositionFilter.hxx>
class SelectMgr_EntityOwner;


class SelectMgr_OrFilter;
DEFINE_STANDARD_HANDLE(SelectMgr_OrFilter, SelectMgr_CompositionFilter)

//! A framework to define an or selection filter.
//! This selects one or another type of sensitive entity.
class SelectMgr_OrFilter : public SelectMgr_CompositionFilter
{

public:

  
  //! Constructs an empty or selection filter.
  Standard_EXPORT SelectMgr_OrFilter();
  
  Standard_EXPORT Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& anobj) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(SelectMgr_OrFilter,SelectMgr_CompositionFilter)

protected:

};







#endif // _SelectMgr_OrFilter_HeaderFile

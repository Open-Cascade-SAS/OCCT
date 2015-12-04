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

#ifndef _SelectMgr_AndFilter_HeaderFile
#define _SelectMgr_AndFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <SelectMgr_CompositionFilter.hxx>
#include <Standard_Boolean.hxx>
class SelectMgr_EntityOwner;


class SelectMgr_AndFilter;
DEFINE_STANDARD_HANDLE(SelectMgr_AndFilter, SelectMgr_CompositionFilter)

//! A framework to define a selection filter for two or
//! more types of entity.
class SelectMgr_AndFilter : public SelectMgr_CompositionFilter
{

public:

  
  //! Constructs an empty selection filter object for two or
  //! more types of entity.
  Standard_EXPORT SelectMgr_AndFilter();
  
  Standard_EXPORT Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& anobj) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(SelectMgr_AndFilter,SelectMgr_CompositionFilter)

protected:




private:




};







#endif // _SelectMgr_AndFilter_HeaderFile

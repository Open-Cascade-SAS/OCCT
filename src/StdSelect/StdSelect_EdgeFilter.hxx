// Created on: 1996-03-11
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

#ifndef _StdSelect_EdgeFilter_HeaderFile
#define _StdSelect_EdgeFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StdSelect_TypeOfEdge.hxx>
#include <SelectMgr_Filter.hxx>
#include <TopAbs_ShapeEnum.hxx>
class SelectMgr_EntityOwner;


class StdSelect_EdgeFilter;
DEFINE_STANDARD_HANDLE(StdSelect_EdgeFilter, SelectMgr_Filter)

//! A framework to define a filter to select a specific type of edge.
//! The types available include:
//! -   any edge
//! -   a linear edge
//! -   a circular edge.
class StdSelect_EdgeFilter : public SelectMgr_Filter
{

public:

  
  //! Constructs an edge filter object defined by the type of edge Edge.
  Standard_EXPORT StdSelect_EdgeFilter(const StdSelect_TypeOfEdge Edge);
  
  //! Sets the type of edge aNewType. aNewType is to be highlighted in selection.
  Standard_EXPORT void SetType (const StdSelect_TypeOfEdge aNewType);
  
  //! Returns the type of edge to be highlighted in selection.
  Standard_EXPORT StdSelect_TypeOfEdge Type() const;
  
  Standard_EXPORT virtual Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& anobj) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean ActsOn (const TopAbs_ShapeEnum aStandardMode) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StdSelect_EdgeFilter,SelectMgr_Filter)

protected:




private:


  StdSelect_TypeOfEdge mytype;


};







#endif // _StdSelect_EdgeFilter_HeaderFile

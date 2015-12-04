// Created on: 1996-03-08
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

#ifndef _StdSelect_FaceFilter_HeaderFile
#define _StdSelect_FaceFilter_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StdSelect_TypeOfFace.hxx>
#include <SelectMgr_Filter.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_ShapeEnum.hxx>
class SelectMgr_EntityOwner;


class StdSelect_FaceFilter;
DEFINE_STANDARD_HANDLE(StdSelect_FaceFilter, SelectMgr_Filter)

//! A framework to define a filter to select a specific type of face.
//! The types available include:
//! -   any face
//! -   a planar face
//! -   a cylindrical face
//! -   a spherical face
//! -   a toroidal face
//! -   a revol face.
class StdSelect_FaceFilter : public SelectMgr_Filter
{

public:

  
  //! Constructs a face filter object defined by the type of face aTypeOfFace.
  Standard_EXPORT StdSelect_FaceFilter(const StdSelect_TypeOfFace aTypeOfFace);
  
  //! Sets the type of face aNewType. aNewType is to be highlighted in selection.
  Standard_EXPORT void SetType (const StdSelect_TypeOfFace aNewType);
  
  //! Returns the type of face to be highlighted in selection.
  Standard_EXPORT StdSelect_TypeOfFace Type() const;
  
  Standard_EXPORT virtual Standard_Boolean IsOk (const Handle(SelectMgr_EntityOwner)& anobj) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Boolean ActsOn (const TopAbs_ShapeEnum aStandardMode) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(StdSelect_FaceFilter,SelectMgr_Filter)

protected:




private:


  StdSelect_TypeOfFace mytype;


};







#endif // _StdSelect_FaceFilter_HeaderFile

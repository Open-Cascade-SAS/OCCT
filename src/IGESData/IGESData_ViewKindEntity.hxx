// Created on: 1992-04-07
// Created by: Christian CAILLET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _IGESData_ViewKindEntity_HeaderFile
#define _IGESData_ViewKindEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class Standard_OutOfRange;


class IGESData_ViewKindEntity;
DEFINE_STANDARD_HANDLE(IGESData_ViewKindEntity, IGESData_IGESEntity)

//! defines required type for ViewKind in directory part
//! that is, Single view or Multiple view
//! An effective ViewKind entity must inherit it and define
//! IsSingle (True for Single, False for List of Views),
//! NbViews and ViewItem (especially for a List)
class IGESData_ViewKindEntity : public IGESData_IGESEntity
{

public:

  
  //! says if "me" is a Single View (True) or a List of Views (False)
  Standard_EXPORT virtual Standard_Boolean IsSingle() const = 0;
  
  //! Returns the count of Views for a List of Views. For a Single
  //! View, may return simply 1
  Standard_EXPORT virtual Standard_Integer NbViews() const = 0;
  
  //! Returns the View n0. <num> for a List of Views. For a Single
  //! Views, may return <me> itself
  Standard_EXPORT virtual Handle(IGESData_ViewKindEntity) ViewItem (const Standard_Integer num) const = 0;




  DEFINE_STANDARD_RTTIEXT(IGESData_ViewKindEntity,IGESData_IGESEntity)

protected:




private:




};







#endif // _IGESData_ViewKindEntity_HeaderFile

// Created on: 1992-10-21
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

#ifndef _IGESData_SingleParentEntity_HeaderFile
#define _IGESData_SingleParentEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>


class IGESData_SingleParentEntity;
DEFINE_STANDARD_HANDLE(IGESData_SingleParentEntity, IGESData_IGESEntity)

//! a SingleParentEntity is a kind of IGESEntity which can refer
//! to a (Single) Parent, from Associativities list of an Entity
//! a effective SingleParent definition entity must inherit it
class IGESData_SingleParentEntity : public IGESData_IGESEntity
{

public:

  
  //! Returns the parent designated by the Entity, if only one !
  Standard_EXPORT virtual Handle(IGESData_IGESEntity) SingleParent() const = 0;
  
  //! Returns the count of Entities designated as children
  Standard_EXPORT virtual Standard_Integer NbChildren() const = 0;
  
  //! Returns a Child given its rank
  Standard_EXPORT virtual Handle(IGESData_IGESEntity) Child (const Standard_Integer num) const = 0;




  DEFINE_STANDARD_RTTIEXT(IGESData_SingleParentEntity,IGESData_IGESEntity)

protected:




private:




};







#endif // _IGESData_SingleParentEntity_HeaderFile

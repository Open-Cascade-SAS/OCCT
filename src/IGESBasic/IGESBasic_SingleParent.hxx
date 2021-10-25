// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen ( Arun MENON )
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESBasic_SingleParent_HeaderFile
#define _IGESBasic_SingleParent_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_SingleParentEntity.hxx>
class IGESData_IGESEntity;


class IGESBasic_SingleParent;
DEFINE_STANDARD_HANDLE(IGESBasic_SingleParent, IGESData_SingleParentEntity)

//! defines SingleParent, Type <402> Form <9>
//! in package IGESBasic
//! It defines a logical structure of one independent
//! (parent) entity and one or more subordinate (children)
//! entities
class IGESBasic_SingleParent : public IGESData_SingleParentEntity
{

public:

  
  Standard_EXPORT IGESBasic_SingleParent();
  
  //! This method is used to set the fields of the class
  //! SingleParent
  //! - nbParentEntities : Indicates number of Parents, always = 1
  //! - aParentEntity    : Used to hold the Parent Entity
  //! - allChildren      : Used to hold the children
  Standard_EXPORT void Init (const Standard_Integer nbParentEntities, const Handle(IGESData_IGESEntity)& aParentEntity, const Handle(IGESData_HArray1OfIGESEntity)& allChildren);
  
  //! returns the number of Parent Entities, which should be 1
  Standard_EXPORT Standard_Integer NbParentEntities() const;
  
  //! Returns the Parent Entity (inherited method)
  Standard_EXPORT Handle(IGESData_IGESEntity) SingleParent() const Standard_OVERRIDE;
  
  //! returns the number of children of the Parent
  Standard_EXPORT Standard_Integer NbChildren() const Standard_OVERRIDE;
  
  //! returns the specific child as indicated by Index
  //! raises exception if Index <= 0 or Index > NbChildren()
  Standard_EXPORT Handle(IGESData_IGESEntity) Child (const Standard_Integer Index) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESBasic_SingleParent,IGESData_SingleParentEntity)

protected:




private:


  Standard_Integer theNbParentEntities;
  Handle(IGESData_IGESEntity) theParentEntity;
  Handle(IGESData_HArray1OfIGESEntity) theChildren;


};







#endif // _IGESBasic_SingleParent_HeaderFile

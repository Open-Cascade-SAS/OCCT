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

#ifndef _IGESBasic_Group_HeaderFile
#define _IGESBasic_Group_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IGESData_HArray1OfIGESEntity.hxx>
#include <IGESData_IGESEntity.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Standard_Transient;


class IGESBasic_Group;
DEFINE_STANDARD_HANDLE(IGESBasic_Group, IGESData_IGESEntity)

//! defines Group, Type <402> Form <1>
//! in package IGESBasic
//! The Group Associativity allows a collection of a set
//! of entities to be maintained as a single, logical
//! entity
//!
//! Group, OrderedGroup, GroupWithoutBackP, OrderedGroupWithoutBackP
//! share the same definition (class Group), form number changes
//!
//! non Ordered, non WithoutBackP : form  1
//! non Ordered,     WithoutBackP : form  7
//! Ordered, non WithoutBackP : form 14
//! Ordered,     WithoutBackP : form 15
class IGESBasic_Group : public IGESData_IGESEntity
{

public:

  
  Standard_EXPORT IGESBasic_Group();
  
  //! Creates a Group with a predefined count of items
  //! (which all start as null)
  Standard_EXPORT IGESBasic_Group(const Standard_Integer nb);
  
  //! This method is used to set the fields of the class Group
  //! - allEntities : Used to store pointers to members of
  //! the Group.
  Standard_EXPORT void Init (const Handle(IGESData_HArray1OfIGESEntity)& allEntities);
  
  //! Sets a Group to be, or not to be  Ordered (according mode)
  Standard_EXPORT void SetOrdered (const Standard_Boolean mode);
  
  //! Sets a Group to be, or not to be  WithoutBackP
  Standard_EXPORT void SetWithoutBackP (const Standard_Boolean mode);
  
  //! Returns True if <me> is Ordered
  Standard_EXPORT Standard_Boolean IsOrdered() const;
  
  //! Returns True if <me> is WithoutBackP
  Standard_EXPORT Standard_Boolean IsWithoutBackP() const;
  
  //! Enforce a new value for the type and form
  Standard_EXPORT void SetUser (const Standard_Integer type, const Standard_Integer form);
  
  //! Changes the count of item
  //! If greater, new items are null
  //! If lower, old items are lost
  Standard_EXPORT void SetNb (const Standard_Integer nb);
  
  //! returns the number of IGESEntities in the Group
  Standard_EXPORT Standard_Integer NbEntities() const;
  
  //! returns the specific entity from the Group
  Standard_EXPORT Handle(IGESData_IGESEntity) Entity (const Standard_Integer Index) const;
  
  //! returns the specific entity from the Group
  Standard_EXPORT Handle(Standard_Transient) Value (const Standard_Integer Index) const;
  
  //! Sets a new value for item <Index>
  Standard_EXPORT void SetValue (const Standard_Integer Index, const Handle(IGESData_IGESEntity)& ent);




  DEFINE_STANDARD_RTTIEXT(IGESBasic_Group,IGESData_IGESEntity)

protected:




private:


  Handle(IGESData_HArray1OfIGESEntity) theEntities;


};







#endif // _IGESBasic_Group_HeaderFile

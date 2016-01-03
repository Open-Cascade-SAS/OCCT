// Created on: 1993-03-26
// Created by: Christian CAILLET
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

#ifndef _StepData_SelectType_HeaderFile
#define _StepData_SelectType_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Type.hxx>
#include <Standard_CString.hxx>
#include <StepData_Logical.hxx>
#include <Standard_Real.hxx>
class Standard_Transient;
class Standard_TypeMismatch;
class StepData_PDescr;
class StepData_SelectMember;


//! SelectType is the basis used for SELECT_TYPE definitions from
//! the EXPRESS form. A SELECT_TYPE in EXPRESS is an enumeration
//! of Types, it corresponds in a way to a Super-Type, but with
//! no specific Methods, and no exclusivity (a given Type can be
//! member of several SELECT_TYPES, plus be itself a SUB_TYPE).
//!
//! A SelectType can be field of a Transient Entity or only used
//! to control an input Argument
//!
//! This class implies to designate each member Type by a Case
//! Number which is a positive Integer value (this allows a faster
//! treatement).
//!
//! With this class, a specific SelectType can :
//! - recognize an Entity as complying or not with its definition,
//! - storing it, with the garanty that the stored Entity complies
//! with the definition of the SelectType
//! - and (if judged useful) give the stored Entity under the good
//! Type rather than simply "Transient".
class StepData_SelectType 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Recognizes the Type of an Entity. Returns a positive Number
  //! which identifies the Type in the definition List of the
  //! SelectType. Returns Zero if its Type in not in this List.
  Standard_EXPORT virtual Standard_Integer CaseNum (const Handle(Standard_Transient)& ent) const = 0;
  
  //! Returns True if the Type of an Entity complies with the
  //! definition list of the SelectType.
  //! Also checks for a SelectMember
  //! Default Implementation looks for CaseNum  or CaseMem positive
  Standard_EXPORT Standard_Boolean Matches (const Handle(Standard_Transient)& ent) const;
  
  //! Stores an Entity. This allows to define a specific SelectType
  //! class with one read method per member Type, which returns the
  //! Value casted with the good Type.
  Standard_EXPORT void SetValue (const Handle(Standard_Transient)& ent);
  
  //! Nullifies the Stored Entity
  Standard_EXPORT void Nullify();
  
  //! Returns the Stored Entity. Can be used to define specific
  //! read methods (see above)
  Standard_EXPORT const Handle(Standard_Transient)& Value() const;
  
  //! Returns True if there is no Stored Entity (i.e. it is Null)
  Standard_EXPORT Standard_Boolean IsNull() const;
  
  //! Returns the Effective (Dynamic) Type of the Stored Entity
  //! If it is Null, returns TYPE(Transient)
  Standard_EXPORT Handle(Standard_Type) Type() const;
  
  //! Recognizes the Type of the stored Entity, or zero if it is
  //! Null or SelectMember. Calls the first method CaseNum on Value
  Standard_EXPORT Standard_Integer CaseNumber() const;
  
  //! Returns the Description which corresponds to <me>
  //! Null if no specific description to give. This description is
  //! used to control reading an check validity.
  //! Default returns a Null Handle, i.e. undefined description
  //! It can suffice if CaseNum and CaseMem give enough control
  Standard_EXPORT virtual Handle(StepData_PDescr) Description() const;
  
  //! Returns a preferred SelectMember. Default returns a Null
  //! By default, a SelectMember can be set according to data type
  //! and Name : it is a SelectNamed if Name is defined
  //!
  //! This method allows to define, for a specific SelectType, a
  //! specific SelectMember than SelectNamed. For instance for a
  //! Real plus a Name, a SelectReal plus a case number is a good
  //! solution, lighter than SelectNamed which is very multipurpose
  Standard_EXPORT virtual Handle(StepData_SelectMember) NewMember() const;
  
  //! Recognize a SelectMember (kind, name). Returns a positive
  //! value which identifies the case in the List of immediate cases
  //! (distinct from the List of Entity Types). Zero if not
  //! recognizes
  //! Default returns 0, saying that no immediate value is allowed
  Standard_EXPORT virtual Standard_Integer CaseMem (const Handle(StepData_SelectMember)& ent) const;
  
  //! Returns the Type of the stored SelectMember, or zero if it is
  //! Null or Entity. Calls the method CaseMem on Value
  Standard_EXPORT Standard_Integer CaseMember() const;
  
  //! Returns Value as a SelectMember. Null if not a SelectMember
  Standard_EXPORT Handle(StepData_SelectMember) Member() const;
  
  //! Returns the type name of SelectMember. If no SelectMember or
  //! with no type name, returns an empty string
  //! To change it, pass through the SelectMember itself
  Standard_EXPORT Standard_CString SelectName() const;
  
  //! This internal method gives access to a value implemented by an
  //! Integer (to read it)
  Standard_EXPORT Standard_Integer Int() const;
  
  //! This internal method gives access to a value implemented by an
  //! Integer (to set it) : a SelectMember MUST ALREADY BE THERE !
  Standard_EXPORT void SetInt (const Standard_Integer val);
  
  //! Gets the value as an Integer
  Standard_EXPORT Standard_Integer Integer() const;
  
  //! Sets a new Integer value, with an optional type name
  //! Warning : If a SelectMember is already set, works on it : value and
  //! name must then be accepted by this SelectMember
  Standard_EXPORT void SetInteger (const Standard_Integer val, const Standard_CString name = "");
  
  Standard_EXPORT Standard_Boolean Boolean() const;
  
  Standard_EXPORT void SetBoolean (const Standard_Boolean val, const Standard_CString name = "");
  
  Standard_EXPORT StepData_Logical Logical() const;
  
  Standard_EXPORT void SetLogical (const StepData_Logical val, const Standard_CString name = "");
  
  Standard_EXPORT Standard_Real Real() const;
  
  Standard_EXPORT void SetReal (const Standard_Real val, const Standard_CString name = "");
  
  Standard_EXPORT virtual ~StepData_SelectType();

private:



  Handle(Standard_Transient) thevalue;


};







#endif // _StepData_SelectType_HeaderFile

// Created on: 1996-12-16
// Created by: Christian CAILLET
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

#ifndef _StepData_Field_HeaderFile
#define _StepData_Field_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <StepData_Logical.hxx>
#include <Standard_CString.hxx>
class StepData_SelectMember;

//! Defines a generally defined Field for STEP data : can be used
//! either in any kind of entity to implement it or in free format
//! entities in a "late-binding" mode
//! A field can have : no value (or derived), a single value of
//! any kind, a list of value : single or double list
//!
//! When a field is set, this defines its new kind (Integer etc..)
//! A single value is immediately set. A list of value is, firstly
//! declared as for a kind (Integer String etc), then declared as
//! a list with its initial size, after this its items are set
//! Also it can be set in once if the HArray is ready
class StepData_Field
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a Field, empty ("no value defined")
  Standard_EXPORT StepData_Field();

  //! Creates a Field from another one. If <copy> is True, Handled
  //! data (Select,String,List, not entities) are copied
  Standard_EXPORT StepData_Field(const StepData_Field&  other,
                                 const Standard_Boolean copy = Standard_False);

  //! Gets the copy of the values of another field
  Standard_EXPORT void CopyFrom(const StepData_Field& other);

  //! Assignment
  StepData_Field& operator=(const StepData_Field& theOther)
  {
    CopyFrom(theOther);
    return *this;
  }

  //! Clears the field, to set it as "no value defined"
  //! Just before SetList, predeclares it as "any"
  //! A Kind can be directly set here to declare a type
  Standard_EXPORT void Clear(const Standard_Integer kind = 0);

  //! Codes a Field as derived (no proper value)
  Standard_EXPORT void SetDerived();

  //! Directly sets the Integer value, if its Kind matches
  //! Integer, Boolean, Logical, or Enum (does not change Kind)
  Standard_EXPORT void SetInt(const Standard_Integer val);

  //! Sets an Integer value (before SetList* declares it as Integer)
  Standard_EXPORT void SetInteger(const Standard_Integer val = 0);

  //! Sets a Boolean value (or predeclares a list as boolean)
  Standard_EXPORT void SetBoolean(const Standard_Boolean val = Standard_False);

  //! Sets a Logical Value (or predeclares a list as logical)
  Standard_EXPORT void SetLogical(const StepData_Logical val = StepData_LFalse);

  //! Sets a Real    Value (or predeclares a list as Real);
  Standard_EXPORT void SetReal(const Standard_Real val = 0.0);

  //! Sets a String Value (or predeclares a list as String)
  //! Does not redefine the Kind if it is already String or Enum
  Standard_EXPORT void SetString(const Standard_CString val = "");

  //! Sets an Enum Value (as its integer counterpart)
  //! (or predeclares a list as Enum)
  //! If <text> is given , also sets its textual expression
  //! <val> negative means unknown (known values begin at 0)
  Standard_EXPORT void SetEnum(const Standard_Integer val = -1, const Standard_CString text = "");

  //! Sets a SelectMember (for Integer,Boolean,Enum,Real,Logical)
  //! Hence, the value of the field is accessed through this member
  Standard_EXPORT void SetSelectMember(const Handle(StepData_SelectMember)& val);

  //! Sets an Entity Value
  Standard_EXPORT void SetEntity(const Handle(Standard_Transient)& val);

  //! Predeclares a list as of entity
  Standard_EXPORT void SetEntity();

  //! Declares a field as a list, with an initial size
  //! Initial lower is defaulted as 1, can be defined
  //! The list starts empty, typed by the last Set*
  //! If no Set* before, sets it as "any" (transient/select)
  Standard_EXPORT void SetList(const Standard_Integer size, const Standard_Integer first = 1);

  //! Declares a field as an homogeneous square list, with initial
  //! sizes, and initial lowers
  Standard_EXPORT void SetList2(const Standard_Integer siz1,
                                const Standard_Integer siz2,
                                const Standard_Integer f1 = 1,
                                const Standard_Integer f2 = 1);

  //! Sets an undetermined value : can be String, SelectMember,
  //! HArray(1-2) ... else, an Entity
  //! In case of an HArray, determines and records its size(s)
  Standard_EXPORT void Set(const Handle(Standard_Transient)& val);

  //! Declares an item of the list as undefined
  //! (ignored if list not defined as String,Entity or Any)
  Standard_EXPORT void ClearItem(const Standard_Integer num);

  //! Internal access to an Integer Value for a list, plus its kind
  Standard_EXPORT void SetInt(const Standard_Integer num,
                              const Standard_Integer val,
                              const Standard_Integer kind);

  //! Sets an Integer Value for a list (rank num)
  //! (recognizes a SelectMember)
  Standard_EXPORT void SetInteger(const Standard_Integer num, const Standard_Integer val);

  Standard_EXPORT void SetBoolean(const Standard_Integer num, const Standard_Boolean val);

  Standard_EXPORT void SetLogical(const Standard_Integer num, const StepData_Logical val);

  //! Sets an Enum Value (Integer counterpart), also its text
  //! expression if known (if list has been set as "any")
  Standard_EXPORT void SetEnum(const Standard_Integer num,
                               const Standard_Integer val,
                               const Standard_CString text = "");

  Standard_EXPORT void SetReal(const Standard_Integer num, const Standard_Real val);

  Standard_EXPORT void SetString(const Standard_Integer num, const Standard_CString val);

  Standard_EXPORT void SetEntity(const Standard_Integer num, const Handle(Standard_Transient)& val);

  Standard_EXPORT Standard_Boolean IsSet(const Standard_Integer n1 = 1,
                                         const Standard_Integer n2 = 1) const;

  //! Returns the kind of an item in a list or double list
  //! It is the kind of the list, except if it is "Any", in such a
  //! case the true kind is determined and returned
  Standard_EXPORT Standard_Integer ItemKind(const Standard_Integer n1 = 1,
                                            const Standard_Integer n2 = 1) const;

  //! Returns the kind of the field
  //! <type> True (D) : returns only the type itself
  //! else, returns the complete kind
  Standard_EXPORT Standard_Integer Kind(const Standard_Boolean type = Standard_True) const;

  Standard_EXPORT Standard_Integer Arity() const;

  Standard_EXPORT Standard_Integer Length(const Standard_Integer index = 1) const;

  Standard_EXPORT Standard_Integer Lower(const Standard_Integer index = 1) const;

  Standard_EXPORT Standard_Integer Int() const;

  Standard_EXPORT Standard_Integer Integer(const Standard_Integer n1 = 1,
                                           const Standard_Integer n2 = 1) const;

  Standard_EXPORT Standard_Boolean Boolean(const Standard_Integer n1 = 1,
                                           const Standard_Integer n2 = 1) const;

  Standard_EXPORT StepData_Logical Logical(const Standard_Integer n1 = 1,
                                           const Standard_Integer n2 = 1) const;

  Standard_EXPORT Standard_Real Real(const Standard_Integer n1 = 1,
                                     const Standard_Integer n2 = 1) const;

  Standard_EXPORT Standard_CString String(const Standard_Integer n1 = 1,
                                          const Standard_Integer n2 = 1) const;

  Standard_EXPORT Standard_Integer Enum(const Standard_Integer n1 = 1,
                                        const Standard_Integer n2 = 1) const;

  Standard_EXPORT Standard_CString EnumText(const Standard_Integer n1 = 1,
                                            const Standard_Integer n2 = 1) const;

  Standard_EXPORT Handle(Standard_Transient) Entity(const Standard_Integer n1 = 1,
                                                    const Standard_Integer n2 = 1) const;

  Standard_EXPORT Handle(Standard_Transient) Transient() const;

protected:
private:
  Standard_Integer           thekind;
  Standard_Integer           theint;
  Standard_Real              thereal;
  Handle(Standard_Transient) theany;
};

#endif // _StepData_Field_HeaderFile

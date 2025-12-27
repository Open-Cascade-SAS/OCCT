// Created on: 1997-01-03
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StepData_PDescr_HeaderFile
#define _StepData_PDescr_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <StepData_EnumTool.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
class StepData_EDescr;
class StepData_Field;
class Interface_Check;

//! This class is intended to describe the authorized form for a
//! parameter, as a type or a value for a field
//!
//! A PDescr firstly describes a type, which can be SELECT, i.e.
//! have several members
class StepData_PDescr : public Standard_Transient
{

public:
  Standard_EXPORT StepData_PDescr();

  Standard_EXPORT void SetName(const char* name);

  Standard_EXPORT const char* Name() const;

  //! Declares this PDescr to be a Select, hence to have members
  //! <me> itself can be the first member
  Standard_EXPORT void SetSelect();

  //! Adds a member to a SELECT description
  Standard_EXPORT void AddMember(const occ::handle<StepData_PDescr>& member);

  //! Sets a name for SELECT member. To be used if a member is for
  //! an immediate type
  Standard_EXPORT void SetMemberName(const char* memname);

  //! Sets <me> for an Integer value
  Standard_EXPORT void SetInteger();

  //! Sets <me> for a Real value
  Standard_EXPORT void SetReal();

  //! Sets <me> for a String value
  Standard_EXPORT void SetString();

  //! Sets <me> for a Boolean value (false,true)
  Standard_EXPORT void SetBoolean();

  //! Sets <me> for a Logical value (false,true,unknown)
  Standard_EXPORT void SetLogical();

  //! Sets <me> for an Enum value
  //! Then, call AddEnumDef ordered from the first one (value 0)
  Standard_EXPORT void SetEnum();

  //! Adds an enum value as a string
  Standard_EXPORT void AddEnumDef(const char* enumdef);

  //! Sets <me> for an Entity which must match a Type (early-bound)
  Standard_EXPORT void SetType(const occ::handle<Standard_Type>& atype);

  //! Sets <me> for a Described Entity, whose Description must match
  //! the type name <dscnam>
  Standard_EXPORT void SetDescr(const char* dscnam);

  //! Adds an arity count to <me>, by default 1
  //! 1 : a simple field passes to a LIST/ARRAY etc
  //! or a LIST to a LIST OF LIST
  //! 2 : a simple field passes to a LIST OF LIST
  Standard_EXPORT void AddArity(const int arity = 1);

  //! Directly sets the arity count
  //! 0 : simple field
  //! 1 : LIST or ARRAY etc
  //! 2 : LIST OF LIST
  Standard_EXPORT void SetArity(const int arity = 1);

  //! Sets <me> as <other> but duplicated
  //! Hence, some definition may be changed
  Standard_EXPORT void SetFrom(const occ::handle<StepData_PDescr>& other);

  //! Sets/Unsets <me> to accept undefined values
  Standard_EXPORT void SetOptional(const bool opt = true);

  //! Sets/Unsets <me> to be for a derived field
  Standard_EXPORT void SetDerived(const bool der = true);

  //! Sets <me> to describe a field of an entity
  //! With a name and a rank
  Standard_EXPORT void SetField(const char* name, const int rank);

  //! Tells if <me> is for a SELECT
  Standard_EXPORT bool IsSelect() const;

  //! For a SELECT, returns the member whose name matches <name>
  //! To this member, the following question can then be asked
  //! Null Handle if <name> not matched or <me> not a SELECT
  //!
  //! Remark : not to be asked for an entity type
  //! Hence, following IsInteger .. Enum* only apply on <me> and
  //! require Member
  //! While IsType applies on <me> and all Select Members
  Standard_EXPORT occ::handle<StepData_PDescr> Member(const char* name) const;

  //! Tells if <me> is for an Integer
  Standard_EXPORT bool IsInteger() const;

  //! Tells if <me> is for a Real value
  Standard_EXPORT bool IsReal() const;

  //! Tells if <me> is for a String value
  Standard_EXPORT bool IsString() const;

  //! Tells if <me> is for a Boolean value (false,true)
  Standard_EXPORT bool IsBoolean() const;

  //! Tells if <me> is for a Logical value (false,true,unknown)
  Standard_EXPORT bool IsLogical() const;

  //! Tells if <me> is for an Enum value
  //! Then, call AddEnumDef ordered from the first one (value 0)
  //! Managed by an EnumTool
  Standard_EXPORT bool IsEnum() const;

  //! Returns the maximum integer for a suitable value (count - 1)
  Standard_EXPORT int EnumMax() const;

  //! Returns the numeric value found for an enum text
  //! The text must be in capitals and limited by dots
  //! A non-suitable text gives a negative value to be returned
  Standard_EXPORT int EnumValue(const char* name) const;

  //! Returns the text which corresponds to a numeric value,
  //! between 0 and EnumMax. It is limited by dots
  Standard_EXPORT const char* EnumText(const int val) const;

  //! Tells if <me> is for an Entity, either Described or CDL Type
  Standard_EXPORT bool IsEntity() const;

  //! Tells if <me> is for an entity of a given CDL type (early-bnd)
  //! (works for <me> + nexts if <me> is a Select)
  Standard_EXPORT bool IsType(const occ::handle<Standard_Type>& atype) const;

  //! Returns the type to match (IsKind), for a CDL Entity
  //! (else, null handle)
  Standard_EXPORT occ::handle<Standard_Type> Type() const;

  //! Tells if <me> is for a Described entity of a given EDescr
  //! (does this EDescr match description name ?). For late-bnd
  //! (works for <me> + nexts if <me> is a Select)
  Standard_EXPORT bool IsDescr(const occ::handle<StepData_EDescr>& descr) const;

  //! Returns the description (type name) to match, for a Described
  //! (else, empty string)
  Standard_EXPORT const char* DescrName() const;

  //! Returns the arity of <me>
  Standard_EXPORT int Arity() const;

  //! For a LIST or LIST OF LIST, Returns the PDescr for the simpler
  //! PDescr. Else, returns <me>
  //! This allows to have different attributes for Optional for
  //! instance, on a field, and on the parameter of a LIST :
  //! [OPTIONAL] LIST OF [OPTIONAL] ...
  Standard_EXPORT occ::handle<StepData_PDescr> Simple() const;

  //! Tells if <me> is Optional
  Standard_EXPORT bool IsOptional() const;

  //! Tells if <me> is Derived
  Standard_EXPORT bool IsDerived() const;

  //! Tells if <me> is a Field. Else it is a Type
  Standard_EXPORT bool IsField() const;

  Standard_EXPORT const char* FieldName() const;

  Standard_EXPORT int FieldRank() const;

  //! Semantic Check of a Field : does it complies with the given
  //! description ?
  Standard_EXPORT virtual void Check(const StepData_Field&    afild,
                                     occ::handle<Interface_Check>& ach) const;

  DEFINE_STANDARD_RTTIEXT(StepData_PDescr, Standard_Transient)

private:
  Standard_EXPORT int Kind() const;

  TCollection_AsciiString thename;
  int        thesel;
  TCollection_AsciiString thesnam;
  occ::handle<StepData_PDescr> thenext;
  int        thekind;
  StepData_EnumTool       theenum;
  occ::handle<Standard_Type>   thetype;
  TCollection_AsciiString thednam;
  int        thearit;
  occ::handle<StepData_PDescr> thefrom;
  bool        theopt;
  bool        theder;
  TCollection_AsciiString thefnam;
  int        thefnum;
};

#endif // _StepData_PDescr_HeaderFile

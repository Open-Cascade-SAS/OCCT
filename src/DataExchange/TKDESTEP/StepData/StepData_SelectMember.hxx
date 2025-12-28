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

#ifndef _StepData_SelectMember_HeaderFile
#define _StepData_SelectMember_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Interface_ParamType.hxx>
#include <StepData_Logical.hxx>

//! The general form for a Select Member. A Select Member can,
//! either define a value of a basic type (such as an integer)
//! with an additional information : a name or list of names
//! which precise the meaning of this value
//! or be an alternate value in a select, which also accepts an
//! entity (in this case, the name is not mandatory)
//!
//! Several sub-types of SelectMember are defined for integer and
//! real value, plus an "universal" one for any, and one more to
//! describe a select with several names
//!
//! It is also possible to define a specific subtype by redefining
//! virtual method, then give a better control
//!
//! Remark : this class itself could be deferred, because at least
//! one of its virtual methods must be redefined to be usable
class StepData_SelectMember : public Standard_Transient
{

public:
  Standard_EXPORT StepData_SelectMember();

  //! Tells if a SelectMember has a name. Default is False
  Standard_EXPORT virtual bool HasName() const;

  //! Returns the name of a SelectMember. Default is empty
  Standard_EXPORT virtual const char* Name() const;

  //! Sets the name of a SelectMember, returns True if done, False
  //! if no name is allowed
  //! Default does nothing and returns False
  Standard_EXPORT virtual bool SetName(const char* name);

  //! Tells if the name of a SelectMember matches a given one
  //! By default, compares the strings, can be redefined (optimised)
  Standard_EXPORT virtual bool Matches(const char* name) const;

  Standard_EXPORT virtual int Kind() const;

  Standard_EXPORT virtual void SetKind(const int kind);

  //! Returns the Kind of the SelectMember, under the form of an
  //! enum ParamType
  Standard_EXPORT Interface_ParamType ParamType() const;

  //! This internal method gives access to a value implemented by an
  //! Integer (to read it)
  Standard_EXPORT virtual int Int() const;

  //! This internal method gives access to a value implemented by an
  //! Integer (to set it)
  Standard_EXPORT virtual void SetInt(const int val);

  //! Gets the value as an Integer
  Standard_EXPORT int Integer() const;

  Standard_EXPORT void SetInteger(const int val);

  Standard_EXPORT bool Boolean() const;

  Standard_EXPORT void SetBoolean(const bool val);

  Standard_EXPORT StepData_Logical Logical() const;

  Standard_EXPORT void SetLogical(const StepData_Logical val);

  Standard_EXPORT virtual double Real() const;

  Standard_EXPORT virtual void SetReal(const double val);

  Standard_EXPORT virtual const char* String() const;

  Standard_EXPORT virtual void SetString(const char* val);

  Standard_EXPORT int Enum() const;

  Standard_EXPORT virtual const char* EnumText() const;

  Standard_EXPORT void SetEnum(const int val, const char* text = "");

  Standard_EXPORT virtual void SetEnumText(const int val, const char* text);

  DEFINE_STANDARD_RTTIEXT(StepData_SelectMember, Standard_Transient)
};

#endif // _StepData_SelectMember_HeaderFile

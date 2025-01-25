// Created on: 1997-05-09
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

#ifndef _StepData_Described_HeaderFile
#define _StepData_Described_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepData_EDescr;
class StepData_Simple;
class StepData_Field;
class Interface_Check;
class Interface_EntityIterator;

class StepData_Described;
DEFINE_STANDARD_HANDLE(StepData_Described, Standard_Transient)

//! General frame to describe entities with Description (Simple or
//! Complex)
class StepData_Described : public Standard_Transient
{

public:
  //! Returns the Description used to define this entity
  Standard_EXPORT Handle(StepData_EDescr) Description() const;

  //! Tells if a described entity is complex
  Standard_EXPORT virtual Standard_Boolean IsComplex() const = 0;

  //! Tells if a step type is matched by <me>
  //! For a Simple Entity : own type or super type
  //! For a Complex Entity : one of the members
  Standard_EXPORT virtual Standard_Boolean Matches(const Standard_CString steptype) const = 0;

  //! Returns a Simple Entity which matches with a Type in <me> :
  //! For a Simple Entity : me if it matches, else a null handle
  //! For a Complex Entity : the member which matches, else null
  Standard_EXPORT virtual Handle(StepData_Simple) As(const Standard_CString steptype) const = 0;

  //! Tells if a Field brings a given name
  Standard_EXPORT virtual Standard_Boolean HasField(const Standard_CString name) const = 0;

  //! Returns a Field from its name; read-only
  Standard_EXPORT virtual const StepData_Field& Field(const Standard_CString name) const = 0;

  //! Returns a Field from its name; read or write
  Standard_EXPORT virtual StepData_Field& CField(const Standard_CString name) = 0;

  //! Fills a Check by using its Description
  Standard_EXPORT virtual void Check(Handle(Interface_Check)& ach) const = 0;

  //! Fills an EntityIterator with entities shared by <me>
  Standard_EXPORT virtual void Shared(Interface_EntityIterator& list) const = 0;

  DEFINE_STANDARD_RTTIEXT(StepData_Described, Standard_Transient)

protected:
  //! Initializes a Described Entity from a Description
  //! (i.e. puts it in a field ...)
  Standard_EXPORT StepData_Described(const Handle(StepData_EDescr)& descr);

private:
  Handle(StepData_EDescr) thedescr;
};

#endif // _StepData_Described_HeaderFile

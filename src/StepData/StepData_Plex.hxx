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

#ifndef _StepData_Plex_HeaderFile
#define _StepData_Plex_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColStd_SequenceOfTransient.hxx>
#include <StepData_Described.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
class StepData_ECDescr;
class StepData_Simple;
class StepData_Field;
class Interface_Check;
class Interface_EntityIterator;

class StepData_Plex;
DEFINE_STANDARD_HANDLE(StepData_Plex, StepData_Described)

//! A Plex (for Complex) Entity is defined as a list of Simple
//! Members ("external mapping")
//! The types of these members must be in alphabetic order
class StepData_Plex : public StepData_Described
{

public:
  //! Creates a Plex (empty). The complete creation is made by the
  //! ECDescr itself, by calling Add
  Standard_EXPORT StepData_Plex(const Handle(StepData_ECDescr)& descr);

  //! Adds a member to <me>
  Standard_EXPORT void Add(const Handle(StepData_Simple)& member);

  //! Returns the Description as for a Plex
  Standard_EXPORT Handle(StepData_ECDescr) ECDescr() const;

  //! Returns False
  Standard_EXPORT Standard_Boolean IsComplex() const Standard_OVERRIDE;

  //! Tells if a step type is matched by <me>
  //! For a Simple Entity : own type or super type
  //! For a Complex Entity : one of the members
  Standard_EXPORT Standard_Boolean Matches(const Standard_CString steptype) const Standard_OVERRIDE;

  //! Returns a Simple Entity which matches with a Type in <me> :
  //! For a Simple Entity : me if it matches, else a null handle
  //! For a Complex Entity : the member which matches, else null
  Standard_EXPORT Handle(StepData_Simple) As(const Standard_CString steptype) const
    Standard_OVERRIDE;

  //! Tells if a Field brings a given name
  Standard_EXPORT Standard_Boolean HasField(const Standard_CString name) const Standard_OVERRIDE;

  //! Returns a Field from its name; read-only
  Standard_EXPORT const StepData_Field& Field(const Standard_CString name) const Standard_OVERRIDE;

  //! Returns a Field from its name; read or write
  Standard_EXPORT StepData_Field& CField(const Standard_CString name) Standard_OVERRIDE;

  //! Returns the count of simple members
  Standard_EXPORT Standard_Integer NbMembers() const;

  //! Returns a simple member from its rank
  Standard_EXPORT Handle(StepData_Simple) Member(const Standard_Integer num) const;

  //! Returns the actual list of members types
  Standard_EXPORT Handle(TColStd_HSequenceOfAsciiString) TypeList() const;

  //! Fills a Check by using its Description
  Standard_EXPORT void Check(Handle(Interface_Check)& ach) const Standard_OVERRIDE;

  //! Fills an EntityIterator with entities shared by <me>
  Standard_EXPORT void Shared(Interface_EntityIterator& list) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(StepData_Plex, StepData_Described)

protected:
private:
  TColStd_SequenceOfTransient themembers;
};

#endif // _StepData_Plex_HeaderFile

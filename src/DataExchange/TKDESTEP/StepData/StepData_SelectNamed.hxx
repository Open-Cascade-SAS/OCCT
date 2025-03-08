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

#ifndef _StepData_SelectNamed_HeaderFile
#define _StepData_SelectNamed_HeaderFile

#include <Standard.hxx>

#include <TCollection_AsciiString.hxx>
#include <StepData_Field.hxx>
#include <StepData_SelectMember.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>

class StepData_SelectNamed;
DEFINE_STANDARD_HANDLE(StepData_SelectNamed, StepData_SelectMember)

//! This select member can be of any kind, and be named
//! But its takes more memory than some specialised ones
//! This class allows one name for the instance
class StepData_SelectNamed : public StepData_SelectMember
{

public:
  Standard_EXPORT StepData_SelectNamed();

  Standard_EXPORT virtual Standard_Boolean HasName() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_CString Name() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean SetName(const Standard_CString name) Standard_OVERRIDE;

  Standard_EXPORT const StepData_Field& Field() const;

  Standard_EXPORT StepData_Field& CField();

  Standard_EXPORT virtual Standard_Integer Kind() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetKind(const Standard_Integer kind) Standard_OVERRIDE;

  //! This internal method gives access to a value implemented by an
  //! Integer (to read it)
  Standard_EXPORT virtual Standard_Integer Int() const Standard_OVERRIDE;

  //! This internal method gives access to a value implemented by an
  //! Integer (to set it)
  Standard_EXPORT virtual void SetInt(const Standard_Integer val) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Real Real() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetReal(const Standard_Real val) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_CString String() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetString(const Standard_CString val) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(StepData_SelectNamed, StepData_SelectMember)

protected:
private:
  TCollection_AsciiString thename;
  StepData_Field          theval;
};

#endif // _StepData_SelectNamed_HeaderFile

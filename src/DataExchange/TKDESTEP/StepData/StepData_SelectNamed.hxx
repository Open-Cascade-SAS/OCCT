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

//! This select member can be of any kind, and be named
//! But its takes more memory than some specialised ones
//! This class allows one name for the instance
class StepData_SelectNamed : public StepData_SelectMember
{

public:
  Standard_EXPORT StepData_SelectNamed();

  Standard_EXPORT virtual bool HasName() const override;

  Standard_EXPORT virtual const char* Name() const override;

  Standard_EXPORT virtual bool SetName(const char* const name) override;

  Standard_EXPORT const StepData_Field& Field() const;

  Standard_EXPORT StepData_Field& CField();

  Standard_EXPORT virtual int Kind() const override;

  Standard_EXPORT virtual void SetKind(const int kind) override;

  //! This internal method gives access to a value implemented by an
  //! Integer (to read it)
  Standard_EXPORT virtual int Int() const override;

  //! This internal method gives access to a value implemented by an
  //! Integer (to set it)
  Standard_EXPORT virtual void SetInt(const int val) override;

  Standard_EXPORT virtual double Real() const override;

  Standard_EXPORT virtual void SetReal(const double val) override;

  Standard_EXPORT virtual const char* String() const override;

  Standard_EXPORT virtual void SetString(const char* const val) override;

  DEFINE_STANDARD_RTTIEXT(StepData_SelectNamed, StepData_SelectMember)

private:
  TCollection_AsciiString thename;
  StepData_Field          theval;
};

#endif // _StepData_SelectNamed_HeaderFile

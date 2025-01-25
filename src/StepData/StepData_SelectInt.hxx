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

#ifndef _StepData_SelectInt_HeaderFile
#define _StepData_SelectInt_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <StepData_SelectMember.hxx>

class StepData_SelectInt;
DEFINE_STANDARD_HANDLE(StepData_SelectInt, StepData_SelectMember)

//! A SelectInt is a SelectMember specialised for a basic integer
//! type in a select which also accepts entities : this one has
//! NO NAME.
//! For a named select, see SelectNamed
class StepData_SelectInt : public StepData_SelectMember
{

public:
  Standard_EXPORT StepData_SelectInt();

  Standard_EXPORT virtual Standard_Integer Kind() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetKind(const Standard_Integer kind) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Integer Int() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetInt(const Standard_Integer val) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(StepData_SelectInt, StepData_SelectMember)

protected:
private:
  Standard_Integer thekind;
  Standard_Integer theval;
};

#endif // _StepData_SelectInt_HeaderFile

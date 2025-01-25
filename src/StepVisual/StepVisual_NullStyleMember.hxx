// Created on: 2016-03-09
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _StepVisual_NullStyleMember_HeaderFile
#define _StepVisual_NullStyleMember_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_SelectInt.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <StepVisual_NullStyle.hxx>

class StepVisual_NullStyleMember;
DEFINE_STANDARD_HANDLE(StepVisual_NullStyleMember, StepData_SelectInt)

//! Defines NullStyle as unique member of PresentationStyleSelect
//! Works with an EnumTool
class StepVisual_NullStyleMember : public StepData_SelectInt
{

public:
  Standard_EXPORT StepVisual_NullStyleMember();

  virtual Standard_Boolean HasName() const Standard_OVERRIDE { return Standard_True; }

  virtual Standard_CString Name() const Standard_OVERRIDE { return "NULL_STYLE"; }

  virtual Standard_Boolean SetName(const Standard_CString /*theName*/) Standard_OVERRIDE
  {
    return Standard_True;
  }

  Standard_Integer Kind() const Standard_OVERRIDE { return 4; }

  Standard_EXPORT virtual Standard_CString EnumText() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetEnumText(const Standard_Integer theValue,
                                           const Standard_CString theText) Standard_OVERRIDE;

  Standard_EXPORT void SetValue(const StepVisual_NullStyle theValue);

  Standard_EXPORT StepVisual_NullStyle Value() const;

  DEFINE_STANDARD_RTTIEXT(StepVisual_NullStyleMember, StepData_SelectInt)
};
#endif // _StepVisual_NullStyleMember_HeaderFile

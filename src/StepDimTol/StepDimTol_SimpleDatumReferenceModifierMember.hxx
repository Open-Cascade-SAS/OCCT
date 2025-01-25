// Created on: 2015-07-16
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#ifndef _StepDimTol_SimpleDatumReferenceModifierMember_HeaderFile
#define _StepDimTol_SimpleDatumReferenceModifierMember_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepData_SelectInt.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <StepDimTol_SimpleDatumReferenceModifier.hxx>

class StepDimTol_SimpleDatumReferenceModifierMember;
DEFINE_STANDARD_HANDLE(StepDimTol_SimpleDatumReferenceModifierMember, StepData_SelectInt)

//! Defines SimpleDatumReferenceModifier as unique member of DatumReferenceModifier
//! Works with an EnumTool
class StepDimTol_SimpleDatumReferenceModifierMember : public StepData_SelectInt
{

public:
  Standard_EXPORT StepDimTol_SimpleDatumReferenceModifierMember();

  virtual Standard_Boolean HasName() const Standard_OVERRIDE { return Standard_True; }

  virtual Standard_CString Name() const Standard_OVERRIDE
  {
    return "SIMPLE_DATUM_REFERENCE_MODIFIER";
  }

  virtual Standard_Boolean SetName(const Standard_CString /*theName*/) Standard_OVERRIDE
  {
    return Standard_True;
  }

  Standard_Integer Kind() const Standard_OVERRIDE { return 4; }

  Standard_EXPORT virtual Standard_CString EnumText() const Standard_OVERRIDE;

  Standard_EXPORT virtual void SetEnumText(const Standard_Integer theValue,
                                           const Standard_CString theText) Standard_OVERRIDE;

  Standard_EXPORT void SetValue(const StepDimTol_SimpleDatumReferenceModifier theValue);

  Standard_EXPORT StepDimTol_SimpleDatumReferenceModifier Value() const;

  DEFINE_STANDARD_RTTIEXT(StepDimTol_SimpleDatumReferenceModifierMember, StepData_SelectInt)
};
#endif // _StepDimTol_SimpleDatumReferenceModifierMember_HeaderFile

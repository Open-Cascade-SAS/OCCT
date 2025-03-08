// Created on: 2015-07-14
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

#ifndef _StepShape_ValueFormatTypeQualifier_HeaderFile
#define _StepShape_ValueFormatTypeQualifier_HeaderFile

#include <StepShape_ValueFormatTypeQualifier.hxx>

#include <Standard_Transient.hxx>
#include <Standard.hxx>
#include <TCollection_HAsciiString.hxx>

class TCollection_HAsciiString;

class StepShape_ValueFormatTypeQualifier;
DEFINE_STANDARD_HANDLE(StepShape_ValueFormatTypeQualifier, Standard_Transient)

//! Added for Dimensional Tolerances
class StepShape_ValueFormatTypeQualifier : public Standard_Transient
{

public:
  Standard_EXPORT StepShape_ValueFormatTypeQualifier();

  //! Init all field own and inherited
  Standard_EXPORT void Init(const Handle(TCollection_HAsciiString)& theFormatType);

  //! Returns field FormatType
  inline Handle(TCollection_HAsciiString) FormatType() { return formatType; }

  //! Set field FormatType
  inline void SetFormatType(const Handle(TCollection_HAsciiString)& theFormatType)
  {
    formatType = theFormatType;
  }

  DEFINE_STANDARD_RTTIEXT(StepShape_ValueFormatTypeQualifier, Standard_Transient)

private:
  Handle(TCollection_HAsciiString) formatType;
};
#endif // _StepShape_ValueFormatTypeQualifier_HeaderFile

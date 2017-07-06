// Created on: 2015-07-13
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

#ifndef _StepDimTol_ToleranceZoneForm_HeaderFile
#define _StepDimTol_ToleranceZoneForm_HeaderFile

#include <StepDimTol_ToleranceZoneForm.hxx>

#include <Standard_Transient.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>

class TCollection_HAsciiString;

class StepDimTol_ToleranceZoneForm;
DEFINE_STANDARD_HANDLE(StepDimTol_ToleranceZoneForm, Standard_Transient)
//! Added for Dimensional Tolerances
class StepDimTol_ToleranceZoneForm : public Standard_Transient
{

public:
  
  Standard_EXPORT StepDimTol_ToleranceZoneForm();
  
  //! Init all field own and inherited
  Standard_EXPORT void Init (const Handle(TCollection_HAsciiString)& theName);

  //! Returns field Name
  inline Handle(TCollection_HAsciiString) Name()
  {
    return myName;
  }
  
  //! Set field Name
  inline void SetName(const Handle(TCollection_HAsciiString) &theName)
  {
    myName = theName;
  }

  DEFINE_STANDARD_RTTIEXT(StepDimTol_ToleranceZoneForm,Standard_Transient)
  
private:
  Handle(TCollection_HAsciiString) myName;

};
#endif // _StepDimTol_ToleranceZoneForm_HeaderFile

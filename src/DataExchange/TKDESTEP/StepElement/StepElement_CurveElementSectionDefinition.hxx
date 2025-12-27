// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _StepElement_CurveElementSectionDefinition_HeaderFile
#define _StepElement_CurveElementSectionDefinition_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
class TCollection_HAsciiString;

//! Representation of STEP entity CurveElementSectionDefinition
class StepElement_CurveElementSectionDefinition : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepElement_CurveElementSectionDefinition();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const occ::handle<TCollection_HAsciiString>& aDescription,
                            const double                     aSectionAngle);

  //! Returns field Description
  Standard_EXPORT occ::handle<TCollection_HAsciiString> Description() const;

  //! Set field Description
  Standard_EXPORT void SetDescription(const occ::handle<TCollection_HAsciiString>& Description);

  //! Returns field SectionAngle
  Standard_EXPORT double SectionAngle() const;

  //! Set field SectionAngle
  Standard_EXPORT void SetSectionAngle(const double SectionAngle);

  DEFINE_STANDARD_RTTIEXT(StepElement_CurveElementSectionDefinition, Standard_Transient)

private:
  occ::handle<TCollection_HAsciiString> theDescription;
  double                    theSectionAngle;
};

#endif // _StepElement_CurveElementSectionDefinition_HeaderFile

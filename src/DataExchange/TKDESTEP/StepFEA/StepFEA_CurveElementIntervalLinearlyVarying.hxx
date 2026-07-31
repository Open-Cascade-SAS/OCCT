// Created on: 2003-01-22
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _StepFEA_CurveElementIntervalLinearlyVarying_HeaderFile
#define _StepFEA_CurveElementIntervalLinearlyVarying_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepElement_CurveElementSectionDefinition.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepFEA_CurveElementInterval.hxx>
class StepFEA_CurveElementLocation;
class StepBasic_EulerAngles;

//! Representation of STEP entity CurveElementIntervalLinearlyVarying
class StepFEA_CurveElementIntervalLinearlyVarying : public StepFEA_CurveElementInterval
{

public:
  //! Empty constructor
  Standard_EXPORT StepFEA_CurveElementIntervalLinearlyVarying();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const occ::handle<StepFEA_CurveElementLocation>& aCurveElementInterval_FinishPosition,
    const occ::handle<StepBasic_EulerAngles>&        aCurveElementInterval_EuAngles,
    const occ::handle<NCollection_HArray1<occ::handle<StepElement_CurveElementSectionDefinition>>>&
      aSections);

  //! Returns field Sections
  Standard_EXPORT occ::handle<
    NCollection_HArray1<occ::handle<StepElement_CurveElementSectionDefinition>>>
    Sections() const;

  //! Set field Sections
  Standard_EXPORT void SetSections(
    const occ::handle<NCollection_HArray1<occ::handle<StepElement_CurveElementSectionDefinition>>>&
      Sections);

  DEFINE_STANDARD_RTTIEXT(StepFEA_CurveElementIntervalLinearlyVarying, StepFEA_CurveElementInterval)

private:
  occ::handle<NCollection_HArray1<occ::handle<StepElement_CurveElementSectionDefinition>>>
    theSections;
};

#endif // _StepFEA_CurveElementIntervalLinearlyVarying_HeaderFile

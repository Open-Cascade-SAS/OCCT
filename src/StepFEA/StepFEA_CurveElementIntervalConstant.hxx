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

#ifndef _StepFEA_CurveElementIntervalConstant_HeaderFile
#define _StepFEA_CurveElementIntervalConstant_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <StepFEA_CurveElementInterval.hxx>
class StepElement_CurveElementSectionDefinition;
class StepFEA_CurveElementLocation;
class StepBasic_EulerAngles;


class StepFEA_CurveElementIntervalConstant;
DEFINE_STANDARD_HANDLE(StepFEA_CurveElementIntervalConstant, StepFEA_CurveElementInterval)

//! Representation of STEP entity CurveElementIntervalConstant
class StepFEA_CurveElementIntervalConstant : public StepFEA_CurveElementInterval
{

public:

  
  //! Empty constructor
  Standard_EXPORT StepFEA_CurveElementIntervalConstant();
  
  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init (const Handle(StepFEA_CurveElementLocation)& aCurveElementInterval_FinishPosition, const Handle(StepBasic_EulerAngles)& aCurveElementInterval_EuAngles, const Handle(StepElement_CurveElementSectionDefinition)& aSection);
  
  //! Returns field Section
  Standard_EXPORT Handle(StepElement_CurveElementSectionDefinition) Section() const;
  
  //! Set field Section
  Standard_EXPORT void SetSection (const Handle(StepElement_CurveElementSectionDefinition)& Section);




  DEFINE_STANDARD_RTTIEXT(StepFEA_CurveElementIntervalConstant,StepFEA_CurveElementInterval)

protected:




private:


  Handle(StepElement_CurveElementSectionDefinition) theSection;


};







#endif // _StepFEA_CurveElementIntervalConstant_HeaderFile

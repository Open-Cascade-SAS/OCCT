// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#ifndef _StepBasic_EffectivityAssignment_HeaderFile
#define _StepBasic_EffectivityAssignment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class StepBasic_Effectivity;

class StepBasic_EffectivityAssignment;
DEFINE_STANDARD_HANDLE(StepBasic_EffectivityAssignment, Standard_Transient)

//! Representation of STEP entity EffectivityAssignment
class StepBasic_EffectivityAssignment : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT StepBasic_EffectivityAssignment();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(const Handle(StepBasic_Effectivity)& aAssignedEffectivity);

  //! Returns field AssignedEffectivity
  Standard_EXPORT Handle(StepBasic_Effectivity) AssignedEffectivity() const;

  //! Set field AssignedEffectivity
  Standard_EXPORT void SetAssignedEffectivity(
    const Handle(StepBasic_Effectivity)& AssignedEffectivity);

  DEFINE_STANDARD_RTTIEXT(StepBasic_EffectivityAssignment, Standard_Transient)

protected:
private:
  Handle(StepBasic_Effectivity) theAssignedEffectivity;
};

#endif // _StepBasic_EffectivityAssignment_HeaderFile

// Created on : Sat May 02 12:41:15 2020
// Created by: Irina KRYLOVA
// Generator:	Express (EXPRESS -> CASCADE/XSTEP Translator) V3.0
// Copyright (c) Open CASCADE 2020
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

#ifndef _StepKinematics_KinematicPropertyMechanismRepresentation_HeaderFile_
#define _StepKinematics_KinematicPropertyMechanismRepresentation_HeaderFile_

#include <Standard.hxx>
#include <StepKinematics_KinematicPropertyDefinitionRepresentation.hxx>

#include <StepRepr_RepresentedDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepKinematics_KinematicLinkRepresentation.hxx>

DEFINE_STANDARD_HANDLE(StepKinematics_KinematicPropertyMechanismRepresentation,
                       StepKinematics_KinematicPropertyDefinitionRepresentation)

//! Representation of STEP entity KinematicPropertyMechanismRepresentation
class StepKinematics_KinematicPropertyMechanismRepresentation
    : public StepKinematics_KinematicPropertyDefinitionRepresentation
{
public:
  //! default constructor
  Standard_EXPORT StepKinematics_KinematicPropertyMechanismRepresentation();

  //! Initialize all fields (own and inherited)
  Standard_EXPORT void Init(
    const StepRepr_RepresentedDefinition&  thePropertyDefinitionRepresentation_Definition,
    const Handle(StepRepr_Representation)& thePropertyDefinitionRepresentation_UsedRepresentation,
    const Handle(StepKinematics_KinematicLinkRepresentation)& theBase);

  //! Returns field Base
  Standard_EXPORT Handle(StepKinematics_KinematicLinkRepresentation) Base() const;
  //! Sets field Base
  Standard_EXPORT void SetBase(const Handle(StepKinematics_KinematicLinkRepresentation)& theBase);

  DEFINE_STANDARD_RTTIEXT(StepKinematics_KinematicPropertyMechanismRepresentation,
                          StepKinematics_KinematicPropertyDefinitionRepresentation)

private:
  Handle(StepKinematics_KinematicLinkRepresentation) myBase;
};
#endif // _StepKinematics_KinematicPropertyMechanismRepresentation_HeaderFile_

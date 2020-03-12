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

#include <StepKinematics_KinematicPropertyMechanismRepresentation.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepKinematics_KinematicPropertyMechanismRepresentation, StepKinematics_KinematicPropertyDefinitionRepresentation)

//=======================================================================
//function : StepKinematics_KinematicPropertyMechanismRepresentation
//purpose  :
//=======================================================================
StepKinematics_KinematicPropertyMechanismRepresentation::StepKinematics_KinematicPropertyMechanismRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void StepKinematics_KinematicPropertyMechanismRepresentation::Init (const StepRepr_RepresentedDefinition& thePropertyDefinitionRepresentation_Definition,
                                                                    const Handle(StepRepr_Representation)& thePropertyDefinitionRepresentation_UsedRepresentation,
                                                                    const Handle(StepKinematics_KinematicLinkRepresentation)& theBase)
{
  StepKinematics_KinematicPropertyDefinitionRepresentation::Init(thePropertyDefinitionRepresentation_Definition,
                                                                 thePropertyDefinitionRepresentation_UsedRepresentation);

  myBase = theBase;
}

//=======================================================================
//function : Base
//purpose  :
//=======================================================================
Handle(StepKinematics_KinematicLinkRepresentation) StepKinematics_KinematicPropertyMechanismRepresentation::Base () const
{
  return myBase;
}

//=======================================================================
//function : SetBase
//purpose  :
//=======================================================================
void StepKinematics_KinematicPropertyMechanismRepresentation::SetBase (const Handle(StepKinematics_KinematicLinkRepresentation)& theBase)
{
  myBase = theBase;
}

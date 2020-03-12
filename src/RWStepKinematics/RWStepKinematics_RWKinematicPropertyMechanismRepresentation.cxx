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

#include <RWStepKinematics_RWKinematicPropertyMechanismRepresentation.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_KinematicPropertyMechanismRepresentation.hxx>
#include <StepRepr_RepresentedDefinition.hxx>
#include <StepRepr_Representation.hxx>
#include <StepKinematics_KinematicLinkRepresentation.hxx>

//=======================================================================
//function : RWStepKinematics_RWKinematicPropertyMechanismRepresentation
//purpose  :
//=======================================================================
RWStepKinematics_RWKinematicPropertyMechanismRepresentation::RWStepKinematics_RWKinematicPropertyMechanismRepresentation() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWKinematicPropertyMechanismRepresentation::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                                            const Standard_Integer theNum,
                                                                            Handle(Interface_Check)& theArch,
                                                                            const Handle(StepKinematics_KinematicPropertyMechanismRepresentation)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,3,theArch,"kinematic_property_mechanism_representation") ) return;

  // Inherited fields of PropertyDefinitionRepresentation

  StepRepr_RepresentedDefinition aPropertyDefinitionRepresentation_Definition;
  theData->ReadEntity (theNum, 1, "property_definition_representation.definition", theArch, aPropertyDefinitionRepresentation_Definition);

  Handle(StepRepr_Representation) aPropertyDefinitionRepresentation_UsedRepresentation;
  theData->ReadEntity (theNum, 2, "property_definition_representation.used_representation", theArch, STANDARD_TYPE(StepRepr_Representation), aPropertyDefinitionRepresentation_UsedRepresentation);

  // Own fields of KinematicPropertyMechanismRepresentation

  Handle(StepKinematics_KinematicLinkRepresentation) aBase;
  theData->ReadEntity (theNum, 3, "base", theArch, STANDARD_TYPE(StepKinematics_KinematicLinkRepresentation), aBase);

  // Initialize entity
  theEnt->Init(aPropertyDefinitionRepresentation_Definition,
            aPropertyDefinitionRepresentation_UsedRepresentation,
            aBase);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWKinematicPropertyMechanismRepresentation::WriteStep (StepData_StepWriter& theSW,
                                                                             const Handle(StepKinematics_KinematicPropertyMechanismRepresentation)& theEnt) const
{

  // Own fields of PropertyDefinitionRepresentation

  theSW.Send (theEnt->Definition().Value());

  theSW.Send (theEnt->UsedRepresentation());

  // Own fields of KinematicPropertyMechanismRepresentation

  theSW.Send (theEnt->Base());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWKinematicPropertyMechanismRepresentation::Share (const Handle(StepKinematics_KinematicPropertyMechanismRepresentation)& theEnt,
                                                                         Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinitionRepresentation

  iter.AddItem (theEnt->StepRepr_PropertyDefinitionRepresentation::Definition().Value());

  iter.AddItem (theEnt->StepRepr_PropertyDefinitionRepresentation::UsedRepresentation());

  // Own fields of KinematicPropertyMechanismRepresentation

  iter.AddItem (theEnt->Base());
}

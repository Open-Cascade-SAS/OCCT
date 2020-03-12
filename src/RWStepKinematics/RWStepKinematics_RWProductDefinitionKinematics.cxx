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

#include <RWStepKinematics_RWProductDefinitionKinematics.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_ProductDefinitionKinematics.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_CharacterizedDefinition.hxx>

//=======================================================================
//function : RWStepKinematics_RWProductDefinitionKinematics
//purpose  :
//=======================================================================
RWStepKinematics_RWProductDefinitionKinematics::RWStepKinematics_RWProductDefinitionKinematics() {}


//=======================================================================
//function : ReadStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWProductDefinitionKinematics::ReadStep (const Handle(StepData_StepReaderData)& theData,
                                                               const Standard_Integer theNum,
                                                               Handle(Interface_Check)& theArch,
                                                               const Handle(StepKinematics_ProductDefinitionKinematics)& theEnt) const
{
  // Check number of parameters
  if ( ! theData->CheckNbParams(theNum,3,theArch,"product_definition_kinematics") ) return;

  // Inherited fields of PropertyDefinition

  Handle(TCollection_HAsciiString) aPropertyDefinition_Name;
  theData->ReadString (theNum, 1, "property_definition.name", theArch, aPropertyDefinition_Name);

  Handle(TCollection_HAsciiString) aPropertyDefinition_Description;
  Standard_Boolean hasPropertyDefinition_Description = Standard_True;
  if ( theData->IsParamDefined (theNum,2) ) {
    theData->ReadString (theNum, 2, "property_definition.description", theArch, aPropertyDefinition_Description);
  }
  else {
    hasPropertyDefinition_Description = Standard_False;
    aPropertyDefinition_Description.Nullify();
  }

  StepRepr_CharacterizedDefinition aPropertyDefinition_Definition;
  theData->ReadEntity (theNum, 3, "property_definition.definition", theArch, aPropertyDefinition_Definition);

  // Initialize entity
  theEnt->Init(aPropertyDefinition_Name,
            hasPropertyDefinition_Description,
            aPropertyDefinition_Description,
            aPropertyDefinition_Definition);
}

//=======================================================================
//function : WriteStep
//purpose  :
//=======================================================================
void RWStepKinematics_RWProductDefinitionKinematics::WriteStep (StepData_StepWriter& theSW,
                                                                const Handle(StepKinematics_ProductDefinitionKinematics)& theEnt) const
{

  // Own fields of PropertyDefinition

  theSW.Send (theEnt->Name());

  if ( theEnt->HasDescription() ) {
    theSW.Send (theEnt->Description());
  }
  else theSW.SendUndef();

  theSW.Send (theEnt->Definition().Value());
}

//=======================================================================
//function : Share
//purpose  :
//=======================================================================
void RWStepKinematics_RWProductDefinitionKinematics::Share (const Handle(StepKinematics_ProductDefinitionKinematics)& theEnt,
                                                            Interface_EntityIterator& iter) const
{

  // Inherited fields of PropertyDefinition

  iter.AddItem (theEnt->StepRepr_PropertyDefinition::Definition().Value());
}

// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <Interface_EntityIterator.hxx>
#include "RWStepBasic_RWGeneralPropertyAssociation.pxx"
#include <StepBasic_GeneralProperty.hxx>
#include <StepBasic_GeneralPropertyAssociation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_PropertyDefinition.hxx>

//=================================================================================================

RWStepBasic_RWGeneralPropertyAssociation::RWStepBasic_RWGeneralPropertyAssociation() {}

//=================================================================================================

void RWStepBasic_RWGeneralPropertyAssociation::ReadStep(
  const Handle(StepData_StepReaderData)&              theData,
  const Standard_Integer                              theNum,
  Handle(Interface_Check)&                            theAch,
  const Handle(StepBasic_GeneralPropertyAssociation)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theAch, "general_property_association"))
    return;

  Handle(TCollection_HAsciiString) aName;
  theData->ReadString(theNum, 1, "name", theAch, aName);

  Handle(TCollection_HAsciiString) aDescription;
  theData->ReadString(theNum, 2, "description", theAch, aDescription);

  Handle(StepBasic_GeneralProperty) aGeneralProperty;
  theData->ReadEntity(theNum,
                      3,
                      "base_definition",
                      theAch,
                      STANDARD_TYPE(StepBasic_GeneralProperty),
                      aGeneralProperty);

  Handle(StepRepr_PropertyDefinition) aPropertyDefinition;
  theData->ReadEntity(theNum,
                      4,
                      "derived_definition",
                      theAch,
                      STANDARD_TYPE(StepRepr_PropertyDefinition),
                      aPropertyDefinition);

  // Initialize entity
  theEnt->Init(aName, aDescription, aGeneralProperty, aPropertyDefinition);
}

//=================================================================================================

void RWStepBasic_RWGeneralPropertyAssociation::WriteStep(
  StepData_StepWriter&                                theSW,
  const Handle(StepBasic_GeneralPropertyAssociation)& theEnt) const
{
  theSW.Send(theEnt->Name());

  theSW.Send(theEnt->Description());

  theSW.Send(theEnt->GeneralProperty());

  theSW.Send(theEnt->PropertyDefinition());
}

//=================================================================================================

void RWStepBasic_RWGeneralPropertyAssociation::Share(
  const Handle(StepBasic_GeneralPropertyAssociation)& theEnt,
  Interface_EntityIterator&                           theIter) const
{
  theIter.AddItem(theEnt->GeneralProperty());

  theIter.AddItem(theEnt->PropertyDefinition());
}

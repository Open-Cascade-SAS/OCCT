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
#include "RWStepBasic_RWGeneralPropertyRelationship.pxx"
#include <StepBasic_GeneralProperty.hxx>
#include <StepBasic_GeneralPropertyRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_PropertyDefinition.hxx>

//=================================================================================================

RWStepBasic_RWGeneralPropertyRelationship::RWStepBasic_RWGeneralPropertyRelationship() {}

//=================================================================================================

void RWStepBasic_RWGeneralPropertyRelationship::ReadStep(
  const Handle(StepData_StepReaderData)&               theData,
  const Standard_Integer                               theNum,
  Handle(Interface_Check)&                             theAch,
  const Handle(StepBasic_GeneralPropertyRelationship)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theAch, "general_property_association"))
    return;

  Handle(TCollection_HAsciiString) aName;
  theData->ReadString(theNum, 1, "name", theAch, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean                 hasDescription = Standard_True;
  if (theData->IsParamDefined(theNum, 2))
  {
    theData->ReadString(theNum, 2, "description", theAch, aDescription);
  }
  else
  {
    hasDescription = Standard_False;
  }

  Handle(StepBasic_GeneralProperty) aRelatingGeneralProperty;
  theData->ReadEntity(theNum,
                      3,
                      "relating_property",
                      theAch,
                      STANDARD_TYPE(StepBasic_GeneralProperty),
                      aRelatingGeneralProperty);

  Handle(StepBasic_GeneralProperty) aRelatedGeneralProperty;
  theData->ReadEntity(theNum,
                      4,
                      "related_property",
                      theAch,
                      STANDARD_TYPE(StepBasic_GeneralProperty),
                      aRelatedGeneralProperty);

  // Initialize entity
  theEnt->Init(aName,
               hasDescription,
               aDescription,
               aRelatingGeneralProperty,
               aRelatedGeneralProperty);
}

//=================================================================================================

void RWStepBasic_RWGeneralPropertyRelationship::WriteStep(
  StepData_StepWriter&                                 theSW,
  const Handle(StepBasic_GeneralPropertyRelationship)& theEnt) const
{
  theSW.Send(theEnt->Name());

  if (theEnt->HasDescription())
  {
    theSW.Send(theEnt->Description());
  }
  else
  {
    theSW.SendUndef();
  }

  theSW.Send(theEnt->RelatingGeneralProperty());

  theSW.Send(theEnt->RelatedGeneralProperty());
}

//=================================================================================================

void RWStepBasic_RWGeneralPropertyRelationship::Share(
  const Handle(StepBasic_GeneralPropertyRelationship)& theEnt,
  Interface_EntityIterator&                            theIter) const
{
  theIter.AddItem(theEnt->RelatingGeneralProperty());

  theIter.AddItem(theEnt->RelatedGeneralProperty());
}

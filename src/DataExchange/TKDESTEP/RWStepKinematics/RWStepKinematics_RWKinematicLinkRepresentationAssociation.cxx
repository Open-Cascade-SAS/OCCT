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

#include "RWStepKinematics_RWKinematicLinkRepresentationAssociation.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_KinematicLinkRepresentationAssociation.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_Representation.hxx>
#include <StepRepr_RepresentationOrRepresentationReference.hxx>

//=================================================================================================

RWStepKinematics_RWKinematicLinkRepresentationAssociation::
  RWStepKinematics_RWKinematicLinkRepresentationAssociation()
{
}

//=================================================================================================

void RWStepKinematics_RWKinematicLinkRepresentationAssociation::ReadStep(
  const Handle(StepData_StepReaderData)&                               theData,
  const Standard_Integer                                               theNum,
  Handle(Interface_Check)&                                             theArch,
  const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theArch, "kinematic_link_representation_association"))
    return;

  // Inherited fields of RepresentationRelationship

  Handle(TCollection_HAsciiString) aRepresentationRelationship_Name;
  theData->ReadString(theNum,
                      1,
                      "representation_relationship.name",
                      theArch,
                      aRepresentationRelationship_Name);

  Handle(TCollection_HAsciiString) aRepresentationRelationship_Description;
  if (theData->IsParamDefined(theNum, 2))
  {
    theData->ReadString(theNum,
                        2,
                        "representation_relationship.description",
                        theArch,
                        aRepresentationRelationship_Description);
  }
  else
  {
    aRepresentationRelationship_Description.Nullify();
  }

  StepRepr_RepresentationOrRepresentationReference aRepresentationRelationship_Rep1;
  theData->ReadEntity(theNum,
                      3,
                      "representation_relationship.rep1",
                      theArch,
                      aRepresentationRelationship_Rep1);

  StepRepr_RepresentationOrRepresentationReference aRepresentationRelationship_Rep2;
  theData->ReadEntity(theNum,
                      4,
                      "representation_relationship.rep2",
                      theArch,
                      aRepresentationRelationship_Rep2);

  // Process only one type (Representation)
  if (aRepresentationRelationship_Rep1.CaseNumber() != 1
      || aRepresentationRelationship_Rep1.CaseNumber() != 1)
    return;

  // Initialize entity
  theEnt->Init(aRepresentationRelationship_Name,
               aRepresentationRelationship_Description,
               aRepresentationRelationship_Rep1.Representation(),
               aRepresentationRelationship_Rep2.Representation());
}

//=================================================================================================

void RWStepKinematics_RWKinematicLinkRepresentationAssociation::WriteStep(
  StepData_StepWriter&                                                 theSW,
  const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& theEnt) const
{

  // Own fields of RepresentationRelationship

  theSW.Send(theEnt->Name());

  if (theEnt->HasDescription())
  {
    theSW.Send(theEnt->Description());
  }
  else
    theSW.SendUndef();

  theSW.Send(theEnt->Rep1());

  theSW.Send(theEnt->Rep2());
}

//=================================================================================================

void RWStepKinematics_RWKinematicLinkRepresentationAssociation::Share(
  const Handle(StepKinematics_KinematicLinkRepresentationAssociation)& theEnt,
  Interface_EntityIterator&                                            iter) const
{

  // Inherited fields of RepresentationRelationship

  iter.AddItem(theEnt->StepRepr_RepresentationRelationship::Rep1());

  iter.AddItem(theEnt->StepRepr_RepresentationRelationship::Rep2());
}

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

#include "RWStepKinematics_RWContextDependentKinematicLinkRepresentation.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_ContextDependentKinematicLinkRepresentation.hxx>
#include <StepKinematics_KinematicLinkRepresentationAssociation.hxx>
#include <StepKinematics_ProductDefinitionRelationshipKinematics.hxx>

//=================================================================================================

RWStepKinematics_RWContextDependentKinematicLinkRepresentation::
  RWStepKinematics_RWContextDependentKinematicLinkRepresentation()
{
}

//=================================================================================================

void RWStepKinematics_RWContextDependentKinematicLinkRepresentation::ReadStep(
  const occ::handle<StepData_StepReaderData>&                                    theData,
  const int                                                                      theNum,
  occ::handle<Interface_Check>&                                                  theArch,
  const occ::handle<StepKinematics_ContextDependentKinematicLinkRepresentation>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum,
                              2,
                              theArch,
                              "context_dependent_kinematic_link_representation"))
    return;

  // Own fields of ContextDependentKinematicLinkRepresentation

  occ::handle<StepKinematics_KinematicLinkRepresentationAssociation> aRepresentationRelation;
  theData->ReadEntity(theNum,
                      1,
                      "representation_relation",
                      theArch,
                      STANDARD_TYPE(StepKinematics_KinematicLinkRepresentationAssociation),
                      aRepresentationRelation);

  occ::handle<StepKinematics_ProductDefinitionRelationshipKinematics> aRepresentedProductRelation;
  theData->ReadEntity(theNum,
                      2,
                      "represented_product_relation",
                      theArch,
                      STANDARD_TYPE(StepKinematics_ProductDefinitionRelationshipKinematics),
                      aRepresentedProductRelation);

  // Initialize entity
  theEnt->Init(aRepresentationRelation, aRepresentedProductRelation);
}

//=================================================================================================

void RWStepKinematics_RWContextDependentKinematicLinkRepresentation::WriteStep(
  StepData_StepWriter&                                                           theSW,
  const occ::handle<StepKinematics_ContextDependentKinematicLinkRepresentation>& theEnt) const
{

  // Own fields of ContextDependentKinematicLinkRepresentation

  theSW.Send(theEnt->RepresentationRelation());

  theSW.Send(theEnt->RepresentedProductRelation());
}

//=================================================================================================

void RWStepKinematics_RWContextDependentKinematicLinkRepresentation::Share(
  const occ::handle<StepKinematics_ContextDependentKinematicLinkRepresentation>& theEnt,
  Interface_EntityIterator&                                                      iter) const
{

  // Own fields of ContextDependentKinematicLinkRepresentation

  iter.AddItem(theEnt->RepresentationRelation());

  iter.AddItem(theEnt->RepresentedProductRelation());
}

// Created on: 2020-06-18
// Created by: PASUKHIN DMITRY
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include "RWStepKinematics_RWMechanismStateRepresentation.pxx"

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_MechanismStateRepresentation.hxx>
#include <StepKinematics_MechanismRepresentation.hxx>

//=================================================================================================

RWStepKinematics_RWMechanismStateRepresentation::RWStepKinematics_RWMechanismStateRepresentation() =
  default;

//=================================================================================================

void RWStepKinematics_RWMechanismStateRepresentation::ReadStep(
  const occ::handle<StepData_StepReaderData>&                     theData,
  const int                                                       theNum,
  occ::handle<Interface_Check>&                                   theArch,
  const occ::handle<StepKinematics_MechanismStateRepresentation>& theEnt) const
{
  // Number of Parameter Control
  if (!theData->CheckNbParams(theNum, 4, theArch, "mechanism_state_representation"))
    return;

  // Inherited field : name
  occ::handle<TCollection_HAsciiString> aName;
  theData->ReadString(theNum, 1, "name", theArch, aName);

  // Inherited field : items
  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aItems;
  occ::handle<StepRepr_RepresentationItem>                                   anEnt;
  int                                                                        nsub;
  if (theData->ReadSubList(theNum, 2, "items", theArch, nsub))
  {
    int nb = theData->NbParams(nsub);
    aItems = new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nb);
    for (int i = 1; i <= nb; i++)
    {
      if (theData->ReadEntity(nsub,
                              i,
                              "representation_item",
                              theArch,
                              STANDARD_TYPE(StepRepr_RepresentationItem),
                              anEnt))
        aItems->SetValue(i, anEnt);
    }
  }

  // Inherited field : context_of_items
  occ::handle<StepRepr_RepresentationContext> aContextOfItems;
  theData->ReadEntity(theNum,
                      3,
                      "context_of_items",
                      theArch,
                      STANDARD_TYPE(StepRepr_RepresentationContext),
                      aContextOfItems);

  // Inherited field : represented_mechanism
  occ::handle<StepKinematics_MechanismRepresentation> aRepresentedMechanism;
  theData->ReadEntity(theNum,
                      4,
                      "represented_mechanism",
                      theArch,
                      STANDARD_TYPE(StepKinematics_MechanismRepresentation),
                      aRepresentedMechanism);

  // Initialisation of the read entity
  theEnt->Init(aName, aItems, aContextOfItems, aRepresentedMechanism);
}

//=================================================================================================

void RWStepKinematics_RWMechanismStateRepresentation::WriteStep(
  StepData_StepWriter&                                            theSW,
  const occ::handle<StepKinematics_MechanismStateRepresentation>& theEnt) const
{
  // Inherited field : name
  theSW.Send(theEnt->Name());

  // Inherited field : items
  theSW.OpenSub();
  for (int i = 1; i <= theEnt->NbItems(); i++)
  {
    theSW.Send(theEnt->ItemsValue(i));
  }
  theSW.CloseSub();

  // Inherited field : context_of_items
  theSW.Send(theEnt->ContextOfItems());

  // Inherited field : represented_mechanism
  theSW.Send(theEnt->Mechanism());
}

//=================================================================================================

void RWStepKinematics_RWMechanismStateRepresentation::Share(
  const occ::handle<StepKinematics_MechanismStateRepresentation>& theEnt,
  Interface_EntityIterator&                                       iter) const
{
  int nbElem = theEnt->NbItems();
  for (int i = 1; i <= nbElem; i++)
  {
    iter.GetOneItem(theEnt->ItemsValue(i));
  }
  iter.GetOneItem(theEnt->ContextOfItems());
  iter.GetOneItem(theEnt->Mechanism());
}

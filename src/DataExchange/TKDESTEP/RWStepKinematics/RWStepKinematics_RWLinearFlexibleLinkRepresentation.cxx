// Created on : Fri May 08 19:02:07 2020
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

#include "RWStepKinematics_RWLinearFlexibleLinkRepresentation.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_LinearFlexibleLinkRepresentation.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <StepKinematics_KinematicLink.hxx>

//=================================================================================================

RWStepKinematics_RWLinearFlexibleLinkRepresentation::
  RWStepKinematics_RWLinearFlexibleLinkRepresentation() = default;

//=================================================================================================

void RWStepKinematics_RWLinearFlexibleLinkRepresentation::ReadStep(
  const occ::handle<StepData_StepReaderData>&                         theData,
  const int                                                           theNum,
  occ::handle<Interface_Check>&                                       theArch,
  const occ::handle<StepKinematics_LinearFlexibleLinkRepresentation>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 4, theArch, "linear_flexible_link_representation"))
    return;

  // Inherited fields of Representation

  occ::handle<TCollection_HAsciiString> aRepresentation_Name;
  theData->ReadString(theNum, 1, "representation.name", theArch, aRepresentation_Name);

  occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>> aRepresentation_Items;
  int                                                                        sub2 = 0;
  if (theData->ReadSubList(theNum, 2, "representation.items", theArch, sub2))
  {
    int nb0 = theData->NbParams(sub2);
    aRepresentation_Items =
      new NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>(1, nb0);
    int num2 = sub2;
    for (int i0 = 1; i0 <= nb0; i0++)
    {
      occ::handle<StepRepr_RepresentationItem> anIt0;
      theData->ReadEntity(num2,
                          i0,
                          "representation_item",
                          theArch,
                          STANDARD_TYPE(StepRepr_RepresentationItem),
                          anIt0);
      aRepresentation_Items->SetValue(i0, anIt0);
    }
  }

  occ::handle<StepRepr_RepresentationContext> aRepresentation_ContextOfItems;
  theData->ReadEntity(theNum,
                      3,
                      "representation.context_of_items",
                      theArch,
                      STANDARD_TYPE(StepRepr_RepresentationContext),
                      aRepresentation_ContextOfItems);

  // Inherited fields of KinematicLinkRepresentation

  occ::handle<StepKinematics_KinematicLink> aKinematicLinkRepresentation_RepresentedLink;
  theData->ReadEntity(theNum,
                      4,
                      "kinematic_link_representation.represented_link",
                      theArch,
                      STANDARD_TYPE(StepKinematics_KinematicLink),
                      aKinematicLinkRepresentation_RepresentedLink);

  // Initialize entity
  theEnt->Init(aRepresentation_Name,
               aRepresentation_Items,
               aRepresentation_ContextOfItems,
               aKinematicLinkRepresentation_RepresentedLink);
}

//=================================================================================================

void RWStepKinematics_RWLinearFlexibleLinkRepresentation::WriteStep(
  StepData_StepWriter&                                                theSW,
  const occ::handle<StepKinematics_LinearFlexibleLinkRepresentation>& theEnt) const
{

  // Own fields of Representation

  theSW.Send(theEnt->Name());

  theSW.OpenSub();
  for (int i1 = 1; i1 <= theEnt->Items()->Length(); i1++)
  {
    occ::handle<StepRepr_RepresentationItem> Var0 = theEnt->Items()->Value(i1);
    theSW.Send(Var0);
  }
  theSW.CloseSub();

  theSW.Send(theEnt->ContextOfItems());

  // Own fields of KinematicLinkRepresentation

  theSW.Send(theEnt->RepresentedLink());
}

//=================================================================================================

void RWStepKinematics_RWLinearFlexibleLinkRepresentation::Share(
  const occ::handle<StepKinematics_LinearFlexibleLinkRepresentation>& theEnt,
  Interface_EntityIterator&                                           iter) const
{

  // Inherited fields of Representation

  for (int i1 = 1; i1 <= theEnt->StepRepr_Representation::Items()->Length(); i1++)
  {
    occ::handle<StepRepr_RepresentationItem> Var0 =
      theEnt->StepRepr_Representation::Items()->Value(i1);
    iter.AddItem(Var0);
  }

  iter.AddItem(theEnt->StepRepr_Representation::ContextOfItems());

  // Inherited fields of KinematicLinkRepresentation

  iter.AddItem(theEnt->StepKinematics_KinematicLinkRepresentation::RepresentedLink());
}

// Created on : Sat May 02 12:41:16 2020
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

#include "RWStepKinematics_RWSphericalPairValue.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepKinematics_SphericalPairValue.hxx>
#include <TCollection_HAsciiString.hxx>
#include <StepKinematics_KinematicPair.hxx>
#include <StepKinematics_SpatialRotation.hxx>

//=================================================================================================

RWStepKinematics_RWSphericalPairValue::RWStepKinematics_RWSphericalPairValue() {}

//=================================================================================================

void RWStepKinematics_RWSphericalPairValue::ReadStep(
  const occ::handle<StepData_StepReaderData>&           theData,
  const int                           theNum,
  occ::handle<Interface_Check>&                         theArch,
  const occ::handle<StepKinematics_SphericalPairValue>& theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 3, theArch, "spherical_pair_value"))
    return;

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theArch, aRepresentationItem_Name);

  // Inherited fields of PairValue

  occ::handle<StepKinematics_KinematicPair> aPairValue_AppliesToPair;
  theData->ReadEntity(theNum,
                      2,
                      "pair_value.applies_to_pair",
                      theArch,
                      STANDARD_TYPE(StepKinematics_KinematicPair),
                      aPairValue_AppliesToPair);

  // Own fields of SphericalPairValue
  StepKinematics_SpatialRotation aInputOrientation;
  if (theData->SubListNumber(theNum, 3, true))
  {
    occ::handle<NCollection_HArray1<double>> aItems;
    int              nsub = 0;
    if (theData->ReadSubList(theNum, 3, "items", theArch, nsub))
    {
      int nb   = theData->NbParams(nsub);
      aItems                = new NCollection_HArray1<double>(1, nb);
      int num2 = nsub;
      for (int i0 = 1; i0 <= nb; i0++)
      {
        double anIt0;
        theData->ReadReal(num2, i0, "real", theArch, anIt0);
        aItems->SetValue(i0, anIt0);
      }
    }
    aInputOrientation.SetValue(aItems);
  }
  else
    theData->ReadEntity(theNum, 3, "input_orientation", theArch, aInputOrientation);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aPairValue_AppliesToPair, aInputOrientation);
}

//=================================================================================================

void RWStepKinematics_RWSphericalPairValue::WriteStep(
  StepData_StepWriter&                             theSW,
  const occ::handle<StepKinematics_SphericalPairValue>& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of PairValue

  theSW.Send(theEnt->AppliesToPair());

  // Own fields of SphericalPairValue

  if (!theEnt->InputOrientation().YprRotation().IsNull())
  {
    // Inherited field : YPR
    theSW.OpenSub();
    for (int i = 1; i <= theEnt->InputOrientation().YprRotation()->Length(); i++)
    {
      theSW.Send(theEnt->InputOrientation().YprRotation()->Value(i));
    }
    theSW.CloseSub();
  }
  else
    theSW.Send(theEnt->InputOrientation().Value());
}

//=================================================================================================

void RWStepKinematics_RWSphericalPairValue::Share(
  const occ::handle<StepKinematics_SphericalPairValue>& theEnt,
  Interface_EntityIterator&                        iter) const
{

  // Inherited fields of RepresentationItem

  // Inherited fields of PairValue

  iter.AddItem(theEnt->StepKinematics_PairValue::AppliesToPair());

  // Own fields of SphericalPairValue

  if (!theEnt->InputOrientation().RotationAboutDirection().IsNull())
    iter.AddItem(theEnt->InputOrientation().Value());
}

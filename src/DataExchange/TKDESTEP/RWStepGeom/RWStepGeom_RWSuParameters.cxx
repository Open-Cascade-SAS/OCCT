// Created on : Sat May 02 12:41:14 2020
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

#include "RWStepGeom_RWSuParameters.pxx"

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_SuParameters.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Standard_Real.hxx>

//=================================================================================================

RWStepGeom_RWSuParameters::RWStepGeom_RWSuParameters() = default;

//=================================================================================================

void RWStepGeom_RWSuParameters::ReadStep(const occ::handle<StepData_StepReaderData>& theData,
                                         const int                                   theNum,
                                         occ::handle<Interface_Check>&               theAch,
                                         const occ::handle<StepGeom_SuParameters>&   theEnt) const
{
  // Check number of parameters
  if (!theData->CheckNbParams(theNum, 7, theAch, "su_parameters"))
    return;

  // Inherited fields of RepresentationItem

  occ::handle<TCollection_HAsciiString> aRepresentationItem_Name;
  theData->ReadString(theNum, 1, "representation_item.name", theAch, aRepresentationItem_Name);

  // Own fields of SuParameters

  double aA;
  theData->ReadReal(theNum, 2, "a", theAch, aA);

  double aAlpha;
  theData->ReadReal(theNum, 3, "alpha", theAch, aAlpha);

  double aB;
  theData->ReadReal(theNum, 4, "b", theAch, aB);

  double aBeta;
  theData->ReadReal(theNum, 5, "beta", theAch, aBeta);

  double aC;
  theData->ReadReal(theNum, 6, "c", theAch, aC);

  double aGamma;
  theData->ReadReal(theNum, 7, "gamma", theAch, aGamma);

  // Initialize entity
  theEnt->Init(aRepresentationItem_Name, aA, aAlpha, aB, aBeta, aC, aGamma);
}

//=================================================================================================

void RWStepGeom_RWSuParameters::WriteStep(StepData_StepWriter&                      theSW,
                                          const occ::handle<StepGeom_SuParameters>& theEnt) const
{

  // Own fields of RepresentationItem

  theSW.Send(theEnt->Name());

  // Own fields of SuParameters

  theSW.Send(theEnt->A());

  theSW.Send(theEnt->Alpha());

  theSW.Send(theEnt->B());

  theSW.Send(theEnt->Beta());

  theSW.Send(theEnt->C());

  theSW.Send(theEnt->Gamma());
}

//=================================================================================================

void RWStepGeom_RWSuParameters::Share(const occ::handle<StepGeom_SuParameters>& /*theEnt*/,
                                      Interface_EntityIterator& /*iter*/) const
{

  // Inherited fields of RepresentationItem

  // Own fields of SuParameters
}

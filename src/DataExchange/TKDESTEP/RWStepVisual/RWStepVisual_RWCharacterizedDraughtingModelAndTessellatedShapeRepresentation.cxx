// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <Interface_Check.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CharacterizedDraughtingModelAndTessellatedShapeRepresentation.hxx>

#include "RWStepVisual_RWCharacterizedDraughtingModelAndTessellatedShapeRepresentation.pxx"
#include "../RWStepRepr/RWStepRepr_RWRepresentation.pxx"

//==================================================================================================

void RWStepVisual_RWCharacterizedDraughtingModelAndTessellatedShapeRepresentation::ReadStep(
  const occ::handle<StepData_StepReaderData>& theData,
  const int                                   theNumber,
  occ::handle<Interface_Check>&               theCheck,
  const occ::handle<StepVisual_CharacterizedDraughtingModelAndTessellatedShapeRepresentation>&
    theEntity) const
{
  int aRepresentation = 0;
  theData->NamedForComplex("REPRESENTATION", "RPRSNT", theNumber, aRepresentation, theCheck);
  const RWStepRepr_RWRepresentation aTool;
  aTool.ReadStep(theData, aRepresentation, theCheck, theEntity);
}

//==================================================================================================

void RWStepVisual_RWCharacterizedDraughtingModelAndTessellatedShapeRepresentation::WriteStep(
  StepData_StepWriter& theWriter,
  const occ::handle<StepVisual_CharacterizedDraughtingModelAndTessellatedShapeRepresentation>&
    theEntity) const
{
  theWriter.StartEntity("CHARACTERIZED_OBJECT");
  theWriter.SendDerived();
  theWriter.SendDerived();
  theWriter.StartEntity("CHARACTERIZED_REPRESENTATION");
  theWriter.StartEntity("DRAUGHTING_MODEL");
  theWriter.StartEntity("REPRESENTATION");
  const RWStepRepr_RWRepresentation aTool;
  aTool.WriteStep(theWriter, theEntity);
  theWriter.StartEntity("SHAPE_REPRESENTATION");
  theWriter.StartEntity("TESSELLATED_SHAPE_REPRESENTATION");
}

//==================================================================================================

void RWStepVisual_RWCharacterizedDraughtingModelAndTessellatedShapeRepresentation::Share(
  const occ::handle<StepVisual_CharacterizedDraughtingModelAndTessellatedShapeRepresentation>&
                            theEntity,
  Interface_EntityIterator& theIterator) const
{
  const RWStepRepr_RWRepresentation aTool;
  aTool.Share(theEntity, theIterator);
}

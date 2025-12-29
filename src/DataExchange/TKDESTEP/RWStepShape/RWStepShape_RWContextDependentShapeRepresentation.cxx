// Copyright (c) 1999-2014 OPEN CASCADE SAS
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
#include "RWStepShape_RWContextDependentShapeRepresentation.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>

// pour Share
//=================================================================================================

RWStepShape_RWContextDependentShapeRepresentation::
  RWStepShape_RWContextDependentShapeRepresentation() = default;

//=================================================================================================

void RWStepShape_RWContextDependentShapeRepresentation::ReadStep(
  const occ::handle<StepData_StepReaderData>&                       data,
  const int                                                         num,
  occ::handle<Interface_Check>&                                     ach,
  const occ::handle<StepShape_ContextDependentShapeRepresentation>& ent) const
{
  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 2, ach, "context_dependent_shape_representation"))
    return;

  // --- own field : representation_relation ---

  occ::handle<StepRepr_ShapeRepresentationRelationship> aRepRel;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadEntity(num,
                   1,
                   "representation_relation",
                   ach,
                   STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship),
                   aRepRel);

  // --- own field : represented_product_relation ---

  occ::handle<StepRepr_ProductDefinitionShape> aProRel;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num,
                   2,
                   "represented_product_relation",
                   ach,
                   STANDARD_TYPE(StepRepr_ProductDefinitionShape),
                   aProRel);

  ent->Init(aRepRel, aProRel);
}

//=================================================================================================

void RWStepShape_RWContextDependentShapeRepresentation::WriteStep(
  StepData_StepWriter&                                              SW,
  const occ::handle<StepShape_ContextDependentShapeRepresentation>& ent) const
{
  SW.Send(ent->RepresentationRelation());
  SW.Send(ent->RepresentedProductRelation());
}

//=================================================================================================

void RWStepShape_RWContextDependentShapeRepresentation::Share(
  const occ::handle<StepShape_ContextDependentShapeRepresentation>& ent,
  Interface_EntityIterator&                                         iter) const
{
  iter.AddItem(ent->RepresentationRelation());
  iter.AddItem(ent->RepresentedProductRelation());
}

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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepShape_RWContextDependentShapeRepresentation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_ShapeRepresentationRelationship.hxx>
#include <StepShape_ContextDependentShapeRepresentation.hxx>
#include <StepShape_ShapeDefinitionRepresentation.hxx>

// pour Share
//=======================================================================
//function : RWStepShape_RWContextDependentShapeRepresentation
//purpose  : 
//=======================================================================
RWStepShape_RWContextDependentShapeRepresentation::RWStepShape_RWContextDependentShapeRepresentation()
{
}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWContextDependentShapeRepresentation::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepShape_ContextDependentShapeRepresentation)& ent) const
{
  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num,2,ach,"context_dependent_shape_representation")) return;

  // --- own field : representation_relation ---
  
  Handle(StepRepr_ShapeRepresentationRelationship) aRepRel;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadEntity(num, 1,"representation_relation", ach, STANDARD_TYPE(StepRepr_ShapeRepresentationRelationship), aRepRel);

  // --- own field : represented_product_relation ---

  Handle(StepRepr_ProductDefinitionShape) aProRel;
  //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadEntity(num, 2,"represented_product_relation", ach, STANDARD_TYPE(StepRepr_ProductDefinitionShape), aProRel);

  ent->Init(aRepRel, aProRel);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWContextDependentShapeRepresentation::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepShape_ContextDependentShapeRepresentation)& ent) const
{
  SW.Send(ent->RepresentationRelation());
  SW.Send(ent->RepresentedProductRelation());
}


//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWContextDependentShapeRepresentation::Share
  (const Handle(StepShape_ContextDependentShapeRepresentation)& ent,
   Interface_EntityIterator& iter) const
{
  iter.AddItem(ent->RepresentationRelation());
  iter.AddItem(ent->RepresentedProductRelation());
}


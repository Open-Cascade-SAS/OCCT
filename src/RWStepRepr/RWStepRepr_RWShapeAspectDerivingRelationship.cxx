// Created on: 2003-06-04
// Created by: Galina KULIKOVA
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWShapeAspectDerivingRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectDerivingRelationship.hxx>

//=======================================================================
//function : RWStepRepr_RWShapeAspectDerivingRelationship
//purpose  : 
//=======================================================================
RWStepRepr_RWShapeAspectDerivingRelationship::RWStepRepr_RWShapeAspectDerivingRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWShapeAspectDerivingRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                             const Standard_Integer num,
                                                             Handle(Interface_Check)& ach,
                                                             const Handle(StepRepr_ShapeAspectDerivingRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"shape_aspect_deriving_relationship") ) return;

  // Inherited fields of ShapeAspectRelationship

  Handle(TCollection_HAsciiString) aShapeAspectRelationship_Name;
  data->ReadString (num, 1, "shape_aspect_relationship.name", ach, aShapeAspectRelationship_Name);

  Handle(TCollection_HAsciiString) aShapeAspectRelationship_Description;
  Standard_Boolean hasShapeAspectRelationship_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "shape_aspect_relationship.description", ach, aShapeAspectRelationship_Description);
  }
  else {
    hasShapeAspectRelationship_Description = Standard_False;
  }

  Handle(StepRepr_ShapeAspect) aShapeAspectRelationship_RelatingShapeAspect;
  data->ReadEntity (num, 3, "shape_aspect_relationship.relating_shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aShapeAspectRelationship_RelatingShapeAspect);

  Handle(StepRepr_ShapeAspect) aShapeAspectRelationship_RelatedShapeAspect;
  data->ReadEntity (num, 4, "shape_aspect_relationship.related_shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aShapeAspectRelationship_RelatedShapeAspect);

  // Initialize entity
  ent->Init(aShapeAspectRelationship_Name,
            hasShapeAspectRelationship_Description,
            aShapeAspectRelationship_Description,
            aShapeAspectRelationship_RelatingShapeAspect,
            aShapeAspectRelationship_RelatedShapeAspect);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWShapeAspectDerivingRelationship::WriteStep (StepData_StepWriter& SW,
                                                              const Handle(StepRepr_ShapeAspectDerivingRelationship) &ent) const
{

  // Inherited fields of ShapeAspectRelationship

  SW.Send (ent->StepRepr_ShapeAspectRelationship::Name());

  if ( ent->StepRepr_ShapeAspectRelationship::HasDescription() ) {
    SW.Send (ent->StepRepr_ShapeAspectRelationship::Description());
  }
  else SW.SendUndef();

  SW.Send (ent->StepRepr_ShapeAspectRelationship::RelatingShapeAspect());

  SW.Send (ent->StepRepr_ShapeAspectRelationship::RelatedShapeAspect());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWShapeAspectDerivingRelationship::Share (const Handle(StepRepr_ShapeAspectDerivingRelationship) &ent,
                                                          Interface_EntityIterator& iter) const
{

  // Inherited fields of ShapeAspectRelationship

  iter.AddItem (ent->StepRepr_ShapeAspectRelationship::RelatingShapeAspect());

  iter.AddItem (ent->StepRepr_ShapeAspectRelationship::RelatedShapeAspect());
}

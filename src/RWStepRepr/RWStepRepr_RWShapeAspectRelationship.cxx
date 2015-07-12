// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWShapeAspectRelationship.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>

//=======================================================================
//function : RWStepRepr_RWShapeAspectRelationship
//purpose  : 
//=======================================================================
RWStepRepr_RWShapeAspectRelationship::RWStepRepr_RWShapeAspectRelationship ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWShapeAspectRelationship::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                     const Standard_Integer num,
                                                     Handle(Interface_Check)& ach,
                                                     const Handle(StepRepr_ShapeAspectRelationship) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"shape_aspect_relationship") ) return;

  // Own fields of ShapeAspectRelationship

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  Handle(StepRepr_ShapeAspect) aRelatingShapeAspect;
  data->ReadEntity (num, 3, "relating_shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aRelatingShapeAspect);

  Handle(StepRepr_ShapeAspect) aRelatedShapeAspect;
  data->ReadEntity (num, 4, "related_shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aRelatedShapeAspect);

  // Initialize entity
  ent->Init(aName,
            hasDescription,
            aDescription,
            aRelatingShapeAspect,
            aRelatedShapeAspect);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWShapeAspectRelationship::WriteStep (StepData_StepWriter& SW,
                                                      const Handle(StepRepr_ShapeAspectRelationship) &ent) const
{

  // Own fields of ShapeAspectRelationship

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->RelatingShapeAspect());

  SW.Send (ent->RelatedShapeAspect());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWShapeAspectRelationship::Share (const Handle(StepRepr_ShapeAspectRelationship) &ent,
                                                  Interface_EntityIterator& iter) const
{

  // Own fields of ShapeAspectRelationship

  iter.AddItem (ent->RelatingShapeAspect());

  iter.AddItem (ent->RelatedShapeAspect());
}

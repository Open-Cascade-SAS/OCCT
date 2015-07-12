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
#include <RWStepShape_RWAngularLocation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_AngularLocation.hxx>

//=======================================================================
//function : RWStepShape_RWAngularLocation
//purpose  : 
//=======================================================================
RWStepShape_RWAngularLocation::RWStepShape_RWAngularLocation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWAngularLocation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepShape_AngularLocation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"angular_location") ) return;

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

  // Own fields of AngularLocation

// PTV 16.09.2000
// defaul value set as StepShape_Small, cause there wasn`t default value, but may be situation when
// value will not be initialized and returnd in ent->Init.
  StepShape_AngleRelator aAngleSelection = StepShape_Small;
  if (data->ParamType (num, 5) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 5);
    if      (strcmp(text, ".EQUAL.") == 0) aAngleSelection = StepShape_Equal;
    else if (strcmp(text, ".LARGE.") == 0) aAngleSelection = StepShape_Large;
    else if (strcmp(text, ".SMALL.") == 0) aAngleSelection = StepShape_Small;
    else ach->AddFail("Parameter #5 (angle_selection) has not allowed value");
  }
  else ach->AddFail("Parameter #5 (angle_selection) is not enumeration");

  // Initialize entity
  ent->Init(aShapeAspectRelationship_Name,
            hasShapeAspectRelationship_Description,
            aShapeAspectRelationship_Description,
            aShapeAspectRelationship_RelatingShapeAspect,
            aShapeAspectRelationship_RelatedShapeAspect,
            aAngleSelection);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWAngularLocation::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepShape_AngularLocation) &ent) const
{

  // Inherited fields of ShapeAspectRelationship

  SW.Send (ent->StepRepr_ShapeAspectRelationship::Name());

  if ( ent->StepRepr_ShapeAspectRelationship::HasDescription() ) {
    SW.Send (ent->StepRepr_ShapeAspectRelationship::Description());
  }
  else SW.SendUndef();

  SW.Send (ent->StepRepr_ShapeAspectRelationship::RelatingShapeAspect());

  SW.Send (ent->StepRepr_ShapeAspectRelationship::RelatedShapeAspect());

  // Own fields of AngularLocation

  switch (ent->AngleSelection()) {
    case StepShape_Equal: SW.SendEnum (".EQUAL."); break;
    case StepShape_Large: SW.SendEnum (".LARGE."); break;
    case StepShape_Small: SW.SendEnum (".SMALL."); break;
  }
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWAngularLocation::Share (const Handle(StepShape_AngularLocation) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Inherited fields of ShapeAspectRelationship

  iter.AddItem (ent->StepRepr_ShapeAspectRelationship::RelatingShapeAspect());

  iter.AddItem (ent->StepRepr_ShapeAspectRelationship::RelatedShapeAspect());

  // Own fields of AngularLocation
}

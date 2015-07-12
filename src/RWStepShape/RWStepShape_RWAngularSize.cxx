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
#include <RWStepShape_RWAngularSize.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_AngularSize.hxx>

//=======================================================================
//function : RWStepShape_RWAngularSize
//purpose  : 
//=======================================================================
RWStepShape_RWAngularSize::RWStepShape_RWAngularSize ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWAngularSize::ReadStep (const Handle(StepData_StepReaderData)& data,
                                          const Standard_Integer num,
                                          Handle(Interface_Check)& ach,
                                          const Handle(StepShape_AngularSize) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"angular_size") ) return;

  // Inherited fields of DimensionalSize

  Handle(StepRepr_ShapeAspect) aDimensionalSize_AppliesTo;
  data->ReadEntity (num, 1, "dimensional_size.applies_to", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aDimensionalSize_AppliesTo);

  Handle(TCollection_HAsciiString) aDimensionalSize_Name;
  data->ReadString (num, 2, "dimensional_size.name", ach, aDimensionalSize_Name);

  // Own fields of AngularSize

// PTV 16.09.2000
// defaul value set as StepShape_Small, cause there wasn`t default value, but may be situation when
// value will not be initialized and returnd in ent->Init.

  StepShape_AngleRelator aAngleSelection = StepShape_Small;
  if (data->ParamType (num, 3) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 3);
    if      (strcmp(text, ".EQUAL.") == 0) aAngleSelection = StepShape_Equal;
    else if (strcmp(text, ".LARGE.") == 0) aAngleSelection = StepShape_Large;
    else if (strcmp(text, ".SMALL.") == 0) aAngleSelection = StepShape_Small;
    else ach->AddFail("Parameter #3 (angle_selection) has not allowed value");
  }
  else ach->AddFail("Parameter #3 (angle_selection) is not enumeration");

  // Initialize entity
  ent->Init(aDimensionalSize_AppliesTo,
            aDimensionalSize_Name,
            aAngleSelection);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWAngularSize::WriteStep (StepData_StepWriter& SW,
                                           const Handle(StepShape_AngularSize) &ent) const
{

  // Inherited fields of DimensionalSize

  SW.Send (ent->StepShape_DimensionalSize::AppliesTo());

  SW.Send (ent->StepShape_DimensionalSize::Name());

  // Own fields of AngularSize

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

void RWStepShape_RWAngularSize::Share (const Handle(StepShape_AngularSize) &ent,
                                       Interface_EntityIterator& iter) const
{

  // Inherited fields of DimensionalSize

  iter.AddItem (ent->StepShape_DimensionalSize::AppliesTo());

  // Own fields of AngularSize
}

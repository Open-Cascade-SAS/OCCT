// Created on: 2000-04-18
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <RWStepShape_RWAngularSize.ixx>

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
    if      (strcmp(text, ".EQUAL.")) aAngleSelection = StepShape_Equal;
    else if (strcmp(text, ".LARGE.")) aAngleSelection = StepShape_Large;
    else if (strcmp(text, ".SMALL.")) aAngleSelection = StepShape_Small;
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

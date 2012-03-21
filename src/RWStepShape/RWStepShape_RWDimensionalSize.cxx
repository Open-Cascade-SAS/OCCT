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

#include <RWStepShape_RWDimensionalSize.ixx>

//=======================================================================
//function : RWStepShape_RWDimensionalSize
//purpose  : 
//=======================================================================

RWStepShape_RWDimensionalSize::RWStepShape_RWDimensionalSize ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSize::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepShape_DimensionalSize) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"dimensional_size") ) return;

  // Own fields of DimensionalSize

  Handle(StepRepr_ShapeAspect) aAppliesTo;
  data->ReadEntity (num, 1, "applies_to", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aAppliesTo);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  // Initialize entity
  ent->Init(aAppliesTo,
            aName);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSize::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepShape_DimensionalSize) &ent) const
{

  // Own fields of DimensionalSize

  SW.Send (ent->AppliesTo());

  SW.Send (ent->Name());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSize::Share (const Handle(StepShape_DimensionalSize) &ent,
                                           Interface_EntityIterator& iter) const
{

  // Own fields of DimensionalSize

  iter.AddItem (ent->AppliesTo());
}

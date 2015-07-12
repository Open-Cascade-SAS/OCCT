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
#include <RWStepShape_RWDimensionalSize.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_DimensionalSize.hxx>

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

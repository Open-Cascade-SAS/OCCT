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
#include <RWStepShape_RWDimensionalSizeWithPath.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ShapeAspect.hxx>
#include <StepShape_DimensionalSizeWithPath.hxx>

//=======================================================================
//function : RWStepShape_RWDimensionalSizeWithPath
//purpose  : 
//=======================================================================
RWStepShape_RWDimensionalSizeWithPath::RWStepShape_RWDimensionalSizeWithPath ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSizeWithPath::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                      const Standard_Integer num,
                                                      Handle(Interface_Check)& ach,
                                                      const Handle(StepShape_DimensionalSizeWithPath) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"dimensional_size_with_path") ) return;

  // Inherited fields of DimensionalSize

  Handle(StepRepr_ShapeAspect) aDimensionalSize_AppliesTo;
  data->ReadEntity (num, 1, "dimensional_size.applies_to", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aDimensionalSize_AppliesTo);

  Handle(TCollection_HAsciiString) aDimensionalSize_Name;
  data->ReadString (num, 2, "dimensional_size.name", ach, aDimensionalSize_Name);

  // Own fields of DimensionalSizeWithPath

  Handle(StepRepr_ShapeAspect) aPath;
  data->ReadEntity (num, 3, "path", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aPath);

  // Initialize entity
  ent->Init(aDimensionalSize_AppliesTo,
            aDimensionalSize_Name,
            aPath);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSizeWithPath::WriteStep (StepData_StepWriter& SW,
                                                       const Handle(StepShape_DimensionalSizeWithPath) &ent) const
{

  // Inherited fields of DimensionalSize

  SW.Send (ent->StepShape_DimensionalSize::AppliesTo());

  SW.Send (ent->StepShape_DimensionalSize::Name());

  // Own fields of DimensionalSizeWithPath

  SW.Send (ent->Path());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepShape_RWDimensionalSizeWithPath::Share (const Handle(StepShape_DimensionalSizeWithPath) &ent,
                                                   Interface_EntityIterator& iter) const
{

  // Inherited fields of DimensionalSize

  iter.AddItem (ent->StepShape_DimensionalSize::AppliesTo());

  // Own fields of DimensionalSizeWithPath

  iter.AddItem (ent->Path());
}

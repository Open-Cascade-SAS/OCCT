// Created on: 2015-06-22
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <RWStepRepr_RWParallelOffset.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_ParallelOffset.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

//=======================================================================
//function : RWStepRepr_RWParallelOffset
//purpose  : 
//=======================================================================

RWStepRepr_RWParallelOffset::RWStepRepr_RWParallelOffset ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWParallelOffset::ReadStep (const Handle(StepData_StepReaderData)& data,
                                            const Standard_Integer num,
                                            Handle(Interface_Check)& ach,
                                            const Handle(StepRepr_ParallelOffset) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"parallel_offset") ) return;

  // Inherited fields of ShapeAspect

  Handle(TCollection_HAsciiString) aShapeAspect_Name;
  data->ReadString (num, 1, "shape_aspect.name", ach, aShapeAspect_Name);

  Handle(TCollection_HAsciiString) aShapeAspect_Description;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "shape_aspect.description", ach, aShapeAspect_Description);
  }

  Handle(StepRepr_ProductDefinitionShape) aShapeAspect_OfShape;
  data->ReadEntity (num, 3, "shape_aspect.of_shape", ach, STANDARD_TYPE(StepRepr_ProductDefinitionShape), aShapeAspect_OfShape);

  StepData_Logical aShapeAspect_ProductDefinitional;
  data->ReadLogical (num, 4, "shape_aspect.product_definitional", ach, aShapeAspect_ProductDefinitional);
  
  // Own fields of ParallelOffset
  
  Handle(StepBasic_MeasureWithUnit) anOffset;
  data->ReadEntity (num, 5, "offset", ach, STANDARD_TYPE(StepBasic_MeasureWithUnit), anOffset);

  // Initialize entity
  ent->Init(aShapeAspect_Name,
            aShapeAspect_Description,
            aShapeAspect_OfShape,
            aShapeAspect_ProductDefinitional,
            anOffset);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWParallelOffset::WriteStep (StepData_StepWriter& SW,
                                             const Handle(StepRepr_ParallelOffset) &ent) const
{

  // Inherited fields of ShapeAspect

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->OfShape());

  SW.SendLogical (ent->ProductDefinitional());
  
  SW.Send(ent->Offset());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWParallelOffset::Share (const Handle(StepRepr_ParallelOffset) &ent,
                                         Interface_EntityIterator& iter) const
{

  // Inherited fields of ShapeAspect

  iter.AddItem (ent->OfShape());
}

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
#include <RWStepDimTol_RWDatum.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_Datum.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

//=======================================================================
//function : RWStepDimTol_RWDatum
//purpose  : 
//=======================================================================
RWStepDimTol_RWDatum::RWStepDimTol_RWDatum ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatum::ReadStep (const Handle(StepData_StepReaderData)& data,
                                     const Standard_Integer num,
                                     Handle(Interface_Check)& ach,
                                     const Handle(StepDimTol_Datum) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,5,ach,"datum") ) return;

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

  // Own fields of Datum

  Handle(TCollection_HAsciiString) aIdentification;
  data->ReadString (num, 5, "identification", ach, aIdentification);

  // Initialize entity
  ent->Init(aShapeAspect_Name,
            aShapeAspect_Description,
            aShapeAspect_OfShape,
            aShapeAspect_ProductDefinitional,
            aIdentification);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatum::WriteStep (StepData_StepWriter& SW,
                                      const Handle(StepDimTol_Datum) &ent) const
{

  // Inherited fields of ShapeAspect

  SW.Send (ent->StepRepr_ShapeAspect::Name());

  SW.Send (ent->StepRepr_ShapeAspect::Description());

  SW.Send (ent->StepRepr_ShapeAspect::OfShape());

  SW.SendLogical (ent->StepRepr_ShapeAspect::ProductDefinitional());

  // Own fields of Datum

  SW.Send (ent->Identification());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWDatum::Share (const Handle(StepDimTol_Datum) &ent,
                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of ShapeAspect

  iter.AddItem (ent->StepRepr_ShapeAspect::OfShape());

  // Own fields of Datum
}

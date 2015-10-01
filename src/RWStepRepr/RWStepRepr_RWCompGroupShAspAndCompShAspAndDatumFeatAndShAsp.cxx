// Created on: 2015-08-11
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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

//=======================================================================
//function : RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp
//purpose  : 
//=======================================================================
RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp::RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp()
{
}


//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp::ReadStep
  (const Handle(StepData_StepReaderData)& data,
   const Standard_Integer num0, Handle(Interface_Check)& ach,
   const Handle(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp)& ent) const
{
  Standard_Integer num = 0;
  data->NamedForComplex("SHAPE_ASPECT","SHPASP", num0, num, ach);
  if (!data->CheckNbParams(num, 4, ach, "shape_aspect")) return;

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  if (data->IsParamDefined (num, 2)) {
    data->ReadString (num, 2, "description", ach, aDescription);
  }
  Handle(StepRepr_ProductDefinitionShape) aOfShape;
  data->ReadEntity(num, 3,"of_shape", ach, STANDARD_TYPE(StepRepr_ProductDefinitionShape), aOfShape);

  StepData_Logical aProductDefinitional;
  data->ReadLogical (num,4,"product_definitional",ach,aProductDefinitional);

  // Initialize the entity
  ent->Init(aName, aDescription, aOfShape, aProductDefinitional);
}


//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp::WriteStep
  (StepData_StepWriter& SW,
   const Handle(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp)& ent) const
{
  SW.StartEntity("COMPOSITE_GROUP_SHAPE_ASPECT");
  SW.StartEntity("COMPOSITE_SHAPE_ASPECT");
  SW.StartEntity("DATUM_FEATURE");
  SW.StartEntity("SHAPE_ASPECT");
  SW.Send(ent->Name());
  SW.Send(ent->Description());
  SW.Send(ent->OfShape());
  SW.SendLogical(ent->ProductDefinitional());
}


//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWCompGroupShAspAndCompShAspAndDatumFeatAndShAsp::Share
  (const Handle(StepRepr_CompGroupShAspAndCompShAspAndDatumFeatAndShAsp)& ent,
   Interface_EntityIterator& iter) const
{
  iter.GetOneItem(ent->OfShape());
}

// Created on: 2015-07-13
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

#include <RWStepDimTol_RWToleranceZone.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepDimTol_HArray1OfToleranceZoneTarget.hxx>
#include <StepDimTol_ToleranceZone.hxx>
#include <StepDimTol_ToleranceZoneForm.hxx>
#include <StepDimTol_ToleranceZoneTarget.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>

//=======================================================================
//function : RWStepDimTol_RWToleranceZone
//purpose  : 
//=======================================================================

RWStepDimTol_RWToleranceZone::RWStepDimTol_RWToleranceZone ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWToleranceZone::ReadStep (const Handle(StepData_StepReaderData)& data,
                                             const Standard_Integer num,
                                             Handle(Interface_Check)& ach,
                                             const Handle(StepDimTol_ToleranceZone) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"tolerance_zone") ) return;

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
  
  // Own fields of ToleranceZone

  Handle(StepDimTol_HArray1OfToleranceZoneTarget) anItems;
  StepDimTol_ToleranceZoneTarget anEnt;
  Standard_Integer nbSub;
  if (data->ReadSubList (num,5,"defining_tolerance",ach,nbSub)) {
    Standard_Integer nbElements = data->NbParams(nbSub);
    anItems = new StepDimTol_HArray1OfToleranceZoneTarget (1, nbElements);
    for (Standard_Integer i = 1; i <= nbElements; i++) {
      if (data->ReadEntity(nbSub, i,"tolerance_zone_target", ach, anEnt))
        anItems->SetValue(i, anEnt);
    }
  }
  
  Handle (StepDimTol_ToleranceZoneForm) aForm;
  data->ReadEntity (num, 6, "form", ach, STANDARD_TYPE(StepDimTol_ToleranceZoneForm), aForm);

  // Initialize entity
  ent->Init(aShapeAspect_Name,
            aShapeAspect_Description,
            aShapeAspect_OfShape,
            aShapeAspect_ProductDefinitional,
            anItems,
            aForm);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWToleranceZone::WriteStep (StepData_StepWriter& SW,
                                              const Handle(StepDimTol_ToleranceZone) &ent) const
{

  // Inherited fields of ShapeAspect

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.Send (ent->OfShape());

  SW.SendLogical (ent->ProductDefinitional());
  
  // Own fields of ToleranceZone
  
  SW.OpenSub();
  for (Standard_Integer i = 1;  i <= ent->NbDefiningTolerances();  i++) {
    SW.Send(ent->DefiningToleranceValue(i).Value());
  }
  SW.CloseSub();
  
  SW.Send (ent->Form());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWToleranceZone::Share (const Handle(StepDimTol_ToleranceZone) &ent,
                                          Interface_EntityIterator& iter) const
{

  // Inherited fields of ShapeAspect

  iter.AddItem (ent->OfShape());
  
  // Own fields of ToleranceZone
  Standard_Integer i, nb = ent->NbDefiningTolerances();
  for (i = 1; i <= nb; i++)  
    iter.AddItem (ent->DefiningToleranceValue(i).Value());
}

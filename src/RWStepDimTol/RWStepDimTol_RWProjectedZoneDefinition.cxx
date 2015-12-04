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

#include <RWStepDimTol_RWProjectedZoneDefinition.hxx>

#include <Interface_EntityIterator.hxx>
#include <StepBasic_LengthMeasureWithUnit.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_HArray1OfShapeAspect.hxx>
#include <StepDimTol_ProjectedZoneDefinition.hxx>
#include <StepDimTol_ToleranceZone.hxx>
#include <StepRepr_ShapeAspect.hxx>

//=======================================================================
//function : RWStepDimTol_RWProjectedZoneDefinition
//purpose  : 
//=======================================================================

RWStepDimTol_RWProjectedZoneDefinition::RWStepDimTol_RWProjectedZoneDefinition ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWProjectedZoneDefinition::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                       const Standard_Integer num,
                                                       Handle(Interface_Check)& ach,
                                                       const Handle(StepDimTol_ProjectedZoneDefinition) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"projected_zone_definition") ) return;

  // Inherited fields from ToleranceZoneDefinition

  Handle(StepDimTol_ToleranceZone) aToleranceZone;
  data->ReadEntity (num, 1, "tolerance_zone_definition.zone", ach, STANDARD_TYPE(StepDimTol_ToleranceZone), aToleranceZone);
  
  Handle(StepRepr_HArray1OfShapeAspect) anItems;
  Handle(StepRepr_ShapeAspect) anEnt;
  Standard_Integer nbSub;
  if (data->ReadSubList (num,2,"tolerance_zone_definition.boundaries",ach,nbSub)) {
    Standard_Integer nbElements = data->NbParams(nbSub);
    anItems = new StepRepr_HArray1OfShapeAspect (1, nbElements);
    for (Standard_Integer i = 1; i <= nbElements; i++) {
      if (data->ReadEntity(nbSub, i,"shape_aspect", ach, STANDARD_TYPE(StepRepr_ShapeAspect), anEnt))
        anItems->SetValue(i, anEnt);
    }
  }
  
  Handle(StepRepr_ShapeAspect) aProjectionEnd;
  data->ReadEntity(num, 3, "projection_end", ach, STANDARD_TYPE(StepRepr_ShapeAspect), aProjectionEnd);
  
  Handle(StepBasic_LengthMeasureWithUnit) aProjectionLength;
  data->ReadEntity(num, 4, "projection_length", ach, STANDARD_TYPE(StepBasic_LengthMeasureWithUnit), aProjectionLength);

  // Initialize entity
  ent->Init(aToleranceZone,
            anItems,
            aProjectionEnd,
            aProjectionLength);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepDimTol_RWProjectedZoneDefinition::WriteStep (StepData_StepWriter& SW,
                                                        const Handle(StepDimTol_ProjectedZoneDefinition) &ent) const
{
  // Inherited fields of ToleranceZoneDefinition
  
  SW.Send (ent->Zone());
  
  SW.OpenSub();
  for (Standard_Integer i = 1;  i <= ent->NbBoundaries();  i++) {
    SW.Send(ent->BoundariesValue(i));
  }
  SW.CloseSub();
  
  // Own fields of ProjectedZoneDefinition
  
  SW.Send (ent->ProjectionEnd());
  
  SW.Send (ent->ProjectionLength());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepDimTol_RWProjectedZoneDefinition::Share (const Handle(StepDimTol_ProjectedZoneDefinition) &ent,
                                                    Interface_EntityIterator& iter) const
{

  // Inherited fields from ToleranceZoneDefinition

  iter.AddItem (ent->Zone());
  
  Standard_Integer i, nb = ent->NbBoundaries();
  for (i = 1; i <= nb; i++)  
    iter.AddItem (ent->BoundariesValue(i));
    
  //Own field of ProjectionZoneDefinition
  iter.AddItem (ent->ProjectionEnd());
}

// Created on: 2002-12-26
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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
#include <RWStepFEA_RWConstantSurface3dElementCoordinateSystem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_ConstantSurface3dElementCoordinateSystem.hxx>

//=======================================================================
//function : RWStepFEA_RWConstantSurface3dElementCoordinateSystem
//purpose  : 
//=======================================================================
RWStepFEA_RWConstantSurface3dElementCoordinateSystem::RWStepFEA_RWConstantSurface3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWConstantSurface3dElementCoordinateSystem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                     const Standard_Integer num,
                                                                     Handle(Interface_Check)& ach,
                                                                     const Handle(StepFEA_ConstantSurface3dElementCoordinateSystem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"constant_surface3d_element_coordinate_system") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ConstantSurface3dElementCoordinateSystem

  Standard_Integer aAxis;
  data->ReadInteger (num, 2, "axis", ach, aAxis);

  Standard_Real aAngle;
  data->ReadReal (num, 3, "angle", ach, aAngle);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aAxis,
            aAngle);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWConstantSurface3dElementCoordinateSystem::WriteStep (StepData_StepWriter& SW,
                                                                      const Handle(StepFEA_ConstantSurface3dElementCoordinateSystem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ConstantSurface3dElementCoordinateSystem

  SW.Send (ent->Axis());

  SW.Send (ent->Angle());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWConstantSurface3dElementCoordinateSystem::Share (const Handle(StepFEA_ConstantSurface3dElementCoordinateSystem)&,
                                                                  Interface_EntityIterator&) const
{
  // Inherited fields of RepresentationItem
  // Own fields of ConstantSurface3dElementCoordinateSystem
}

// Created on: 2002-12-12
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
#include <RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepFEA_ArbitraryVolume3dElementCoordinateSystem.hxx>
#include <StepFEA_FeaAxis2Placement3d.hxx>

//=======================================================================
//function : RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem
//purpose  : 
//=======================================================================
RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem::RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                     const Standard_Integer num,
                                                                     Handle(Interface_Check)& ach,
                                                                     const Handle(StepFEA_ArbitraryVolume3dElementCoordinateSystem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"arbitrary_volume3d_element_coordinate_system") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ArbitraryVolume3dElementCoordinateSystem

  Handle(StepFEA_FeaAxis2Placement3d) aCoordinateSystem;
  data->ReadEntity (num, 2, "coordinate_system", ach, STANDARD_TYPE(StepFEA_FeaAxis2Placement3d), aCoordinateSystem);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aCoordinateSystem);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem::WriteStep (StepData_StepWriter& SW,
                                                                      const Handle(StepFEA_ArbitraryVolume3dElementCoordinateSystem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ArbitraryVolume3dElementCoordinateSystem

  SW.Send (ent->CoordinateSystem());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWArbitraryVolume3dElementCoordinateSystem::Share (const Handle(StepFEA_ArbitraryVolume3dElementCoordinateSystem) &ent,
                                                                  Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of ArbitraryVolume3dElementCoordinateSystem

  iter.AddItem (ent->CoordinateSystem());
}

// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <RWStepFEA_RWParametricSurface3dElementCoordinateSystem.ixx>

//=======================================================================
//function : RWStepFEA_RWParametricSurface3dElementCoordinateSystem
//purpose  : 
//=======================================================================

RWStepFEA_RWParametricSurface3dElementCoordinateSystem::RWStepFEA_RWParametricSurface3dElementCoordinateSystem ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricSurface3dElementCoordinateSystem::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                       const Standard_Integer num,
                                                                       Handle(Interface_Check)& ach,
                                                                       const Handle(StepFEA_ParametricSurface3dElementCoordinateSystem) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"parametric_surface3d_element_coordinate_system") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ParametricSurface3dElementCoordinateSystem

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

void RWStepFEA_RWParametricSurface3dElementCoordinateSystem::WriteStep (StepData_StepWriter& SW,
                                                                        const Handle(StepFEA_ParametricSurface3dElementCoordinateSystem) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ParametricSurface3dElementCoordinateSystem

  SW.Send (ent->Axis());

  SW.Send (ent->Angle());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricSurface3dElementCoordinateSystem::Share (const Handle(StepFEA_ParametricSurface3dElementCoordinateSystem) &ent,
                                                                    Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of ParametricSurface3dElementCoordinateSystem
}

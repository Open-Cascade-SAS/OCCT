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

#include <RWStepFEA_RWParametricCurve3dElementCoordinateDirection.ixx>

//=======================================================================
//function : RWStepFEA_RWParametricCurve3dElementCoordinateDirection
//purpose  : 
//=======================================================================

RWStepFEA_RWParametricCurve3dElementCoordinateDirection::RWStepFEA_RWParametricCurve3dElementCoordinateDirection ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateDirection::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                        const Standard_Integer num,
                                                                        Handle(Interface_Check)& ach,
                                                                        const Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"parametric_curve3d_element_coordinate_direction") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of ParametricCurve3dElementCoordinateDirection

  Handle(StepGeom_Direction) aOrientation;
  data->ReadEntity (num, 2, "orientation", ach, STANDARD_TYPE(StepGeom_Direction), aOrientation);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aOrientation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateDirection::WriteStep (StepData_StepWriter& SW,
                                                                         const Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of ParametricCurve3dElementCoordinateDirection

  SW.Send (ent->Orientation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWParametricCurve3dElementCoordinateDirection::Share (const Handle(StepFEA_ParametricCurve3dElementCoordinateDirection) &ent,
                                                                     Interface_EntityIterator& iter) const
{

  // Inherited fields of RepresentationItem

  // Own fields of ParametricCurve3dElementCoordinateDirection

  iter.AddItem (ent->Orientation());
}

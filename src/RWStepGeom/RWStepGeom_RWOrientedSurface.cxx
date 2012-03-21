// Created on: 2002-01-04
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <RWStepGeom_RWOrientedSurface.ixx>

//=======================================================================
//function : RWStepGeom_RWOrientedSurface
//purpose  : 
//=======================================================================

RWStepGeom_RWOrientedSurface::RWStepGeom_RWOrientedSurface ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepGeom_RWOrientedSurface::ReadStep (const Handle(StepData_StepReaderData)& data,
                                             const Standard_Integer num,
                                             Handle(Interface_Check)& ach,
                                             const Handle(StepGeom_OrientedSurface) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"oriented_surface") ) return;

  // Inherited fields of RepresentationItem

  Handle(TCollection_HAsciiString) aRepresentationItem_Name;
  data->ReadString (num, 1, "representation_item.name", ach, aRepresentationItem_Name);

  // Own fields of OrientedSurface

  Standard_Boolean aOrientation;
  data->ReadBoolean (num, 2, "orientation", ach, aOrientation);

  // Initialize entity
  ent->Init(aRepresentationItem_Name,
            aOrientation);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepGeom_RWOrientedSurface::WriteStep (StepData_StepWriter& SW,
                                              const Handle(StepGeom_OrientedSurface) &ent) const
{

  // Inherited fields of RepresentationItem

  SW.Send (ent->StepRepr_RepresentationItem::Name());

  // Own fields of OrientedSurface

  SW.SendBoolean (ent->Orientation());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepGeom_RWOrientedSurface::Share (const Handle(StepGeom_OrientedSurface) &/*ent*/,
                                          Interface_EntityIterator& /*iter*/) const
{

  // Inherited fields of RepresentationItem

  // Own fields of OrientedSurface
}

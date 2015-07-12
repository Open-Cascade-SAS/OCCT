// Created on: 2002-01-04
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepGeom_RWOrientedSurface.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_OrientedSurface.hxx>

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

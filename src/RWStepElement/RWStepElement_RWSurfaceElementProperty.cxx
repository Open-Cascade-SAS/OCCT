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
#include <RWStepElement_RWSurfaceElementProperty.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_SurfaceElementProperty.hxx>
#include <StepElement_SurfaceSectionField.hxx>

//=======================================================================
//function : RWStepElement_RWSurfaceElementProperty
//purpose  : 
//=======================================================================
RWStepElement_RWSurfaceElementProperty::RWStepElement_RWSurfaceElementProperty ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceElementProperty::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                       const Standard_Integer num,
                                                       Handle(Interface_Check)& ach,
                                                       const Handle(StepElement_SurfaceElementProperty) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"surface_element_property") ) return;

  // Own fields of SurfaceElementProperty

  Handle(TCollection_HAsciiString) aPropertyId;
  data->ReadString (num, 1, "property_id", ach, aPropertyId);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepElement_SurfaceSectionField) aSection;
  data->ReadEntity (num, 3, "section", ach, STANDARD_TYPE(StepElement_SurfaceSectionField), aSection);

  // Initialize entity
  ent->Init(aPropertyId,
            aDescription,
            aSection);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceElementProperty::WriteStep (StepData_StepWriter& SW,
                                                        const Handle(StepElement_SurfaceElementProperty) &ent) const
{

  // Own fields of SurfaceElementProperty

  SW.Send (ent->PropertyId());

  SW.Send (ent->Description());

  SW.Send (ent->Section());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceElementProperty::Share (const Handle(StepElement_SurfaceElementProperty) &ent,
                                                    Interface_EntityIterator& iter) const
{

  // Own fields of SurfaceElementProperty

  iter.AddItem (ent->Section());
}

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
#include <RWStepElement_RWSurfaceSectionField.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_SurfaceSectionField.hxx>

//=======================================================================
//function : RWStepElement_RWSurfaceSectionField
//purpose  : 
//=======================================================================
RWStepElement_RWSurfaceSectionField::RWStepElement_RWSurfaceSectionField ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionField::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepElement_SurfaceSectionField) &/*ent*/) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,0,ach,"surface_section_field") ) return;

  // Initialize entity
//  ent->Init();
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionField::WriteStep (StepData_StepWriter&,
                                                     const Handle(StepElement_SurfaceSectionField)&) const
{
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWSurfaceSectionField::Share (const Handle(StepElement_SurfaceSectionField)&,
                                                 Interface_EntityIterator&) const
{
}

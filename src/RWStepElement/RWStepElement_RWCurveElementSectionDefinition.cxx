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
#include <RWStepElement_RWCurveElementSectionDefinition.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_CurveElementSectionDefinition.hxx>

//=======================================================================
//function : RWStepElement_RWCurveElementSectionDefinition
//purpose  : 
//=======================================================================
RWStepElement_RWCurveElementSectionDefinition::RWStepElement_RWCurveElementSectionDefinition ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDefinition::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                              const Standard_Integer num,
                                                              Handle(Interface_Check)& ach,
                                                              const Handle(StepElement_CurveElementSectionDefinition) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"curve_element_section_definition") ) return;

  // Own fields of CurveElementSectionDefinition

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 1, "description", ach, aDescription);

  Standard_Real aSectionAngle;
  data->ReadReal (num, 2, "section_angle", ach, aSectionAngle);

  // Initialize entity
  ent->Init(aDescription,
            aSectionAngle);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDefinition::WriteStep (StepData_StepWriter& SW,
                                                               const Handle(StepElement_CurveElementSectionDefinition) &ent) const
{

  // Own fields of CurveElementSectionDefinition

  SW.Send (ent->Description());

  SW.Send (ent->SectionAngle());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWCurveElementSectionDefinition::Share (const Handle(StepElement_CurveElementSectionDefinition)&,
                                                           Interface_EntityIterator&) const
{
  // Own fields of CurveElementSectionDefinition
}

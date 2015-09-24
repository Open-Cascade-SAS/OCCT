// Created on: 2015-07-14
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

#include <RWStepShape_RWValueFormatTypeQualifier.hxx>

#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_ValueFormatTypeQualifier.hxx>

//=======================================================================
//function : RWStepShape_RWValueFormatTypeQualifier
//purpose  : 
//=======================================================================

RWStepShape_RWValueFormatTypeQualifier::RWStepShape_RWValueFormatTypeQualifier ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepShape_RWValueFormatTypeQualifier::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                       const Standard_Integer num,
                                                       Handle(Interface_Check)& ach,
                                                       const Handle(StepShape_ValueFormatTypeQualifier) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,1,ach,"value_format_type_qualifier") ) return;

  // Own fields of ValueFormatTypeQualifier

  Handle(TCollection_HAsciiString) aFormatType;
  data->ReadString (num, 1, "format_type", ach, aFormatType);

  // Initialize entity
  ent->Init(aFormatType);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepShape_RWValueFormatTypeQualifier::WriteStep (StepData_StepWriter& SW,
                                                        const Handle(StepShape_ValueFormatTypeQualifier) &ent) const
{
  SW.Send (ent->FormatType());
}

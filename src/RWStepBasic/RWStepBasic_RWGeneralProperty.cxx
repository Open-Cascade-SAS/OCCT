// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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
#include <RWStepBasic_RWGeneralProperty.hxx>
#include <StepBasic_GeneralProperty.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepBasic_RWGeneralProperty
//purpose  : 
//=======================================================================
RWStepBasic_RWGeneralProperty::RWStepBasic_RWGeneralProperty ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGeneralProperty::ReadStep (const Handle(StepData_StepReaderData)& data,
                                              const Standard_Integer num,
                                              Handle(Interface_Check)& ach,
                                              const Handle(StepBasic_GeneralProperty) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"general_property") ) return;

  // Own fields of GeneralProperty

  Handle(TCollection_HAsciiString) aId;
  data->ReadString (num, 1, "id", ach, aId);

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 2, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  // Initialize entity
  ent->Init(aId,
            aName,
            hasDescription,
            aDescription);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWGeneralProperty::WriteStep (StepData_StepWriter& SW,
                                               const Handle(StepBasic_GeneralProperty) &ent) const
{

  // Own fields of GeneralProperty

  SW.Send (ent->Id());

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWGeneralProperty::Share (const Handle(StepBasic_GeneralProperty) &,
                                           Interface_EntityIterator&) const
{
  // Own fields of GeneralProperty
}

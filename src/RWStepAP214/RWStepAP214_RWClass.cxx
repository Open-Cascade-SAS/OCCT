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
#include <RWStepAP214_RWClass.hxx>
#include <StepAP214_Class.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

//=======================================================================
//function : RWStepAP214_RWClass
//purpose  : 
//=======================================================================
RWStepAP214_RWClass::RWStepAP214_RWClass ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWClass::ReadStep (const Handle(StepData_StepReaderData)& data,
                                    const Standard_Integer num,
                                    Handle(Interface_Check)& ach,
                                    const Handle(StepAP214_Class) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"class") ) return;

  // Inherited fields of Group

  Handle(TCollection_HAsciiString) aGroup_Name;
  data->ReadString (num, 1, "group.name", ach, aGroup_Name);

  Handle(TCollection_HAsciiString) aGroup_Description;
  Standard_Boolean hasGroup_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "group.description", ach, aGroup_Description);
  }
  else {
    hasGroup_Description = Standard_False;
  }

  // Initialize entity
  ent->Init(aGroup_Name,
            hasGroup_Description,
            aGroup_Description);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepAP214_RWClass::WriteStep (StepData_StepWriter& SW,
                                     const Handle(StepAP214_Class) &ent) const
{

  // Inherited fields of Group

  SW.Send (ent->StepBasic_Group::Name());

  if ( ent->StepBasic_Group::HasDescription() ) {
    SW.Send (ent->StepBasic_Group::Description());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepAP214_RWClass::Share (const Handle(StepAP214_Class) &,
                                 Interface_EntityIterator&) const
{
  // Inherited fields of Group
}

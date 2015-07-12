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
#include <RWStepRepr_RWDataEnvironment.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_DataEnvironment.hxx>
#include <StepRepr_HArray1OfPropertyDefinitionRepresentation.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>

//=======================================================================
//function : RWStepRepr_RWDataEnvironment
//purpose  : 
//=======================================================================
RWStepRepr_RWDataEnvironment::RWStepRepr_RWDataEnvironment ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWDataEnvironment::ReadStep (const Handle(StepData_StepReaderData)& data,
                                             const Standard_Integer num,
                                             Handle(Interface_Check)& ach,
                                             const Handle(StepRepr_DataEnvironment) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"data_environment") ) return;

  // Own fields of DataEnvironment

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation) aElements;
  Standard_Integer sub3 = 0;
  if ( data->ReadSubList (num, 3, "elements", ach, sub3) ) {
    Standard_Integer nb0 = data->NbParams(sub3);
    aElements = new StepRepr_HArray1OfPropertyDefinitionRepresentation (1, nb0);
    Standard_Integer num2 = sub3;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepRepr_PropertyDefinitionRepresentation) anIt0;
      data->ReadEntity (num2, i0, "property_definition_representation", ach, STANDARD_TYPE(StepRepr_PropertyDefinitionRepresentation), anIt0);
      aElements->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aName,
            aDescription,
            aElements);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepRepr_RWDataEnvironment::WriteStep (StepData_StepWriter& SW,
                                              const Handle(StepRepr_DataEnvironment) &ent) const
{

  // Own fields of DataEnvironment

  SW.Send (ent->Name());

  SW.Send (ent->Description());

  SW.OpenSub();
  for (Standard_Integer i2=1; i2 <= ent->Elements()->Length(); i2++ ) {
    Handle(StepRepr_PropertyDefinitionRepresentation) Var0 = ent->Elements()->Value(i2);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepRepr_RWDataEnvironment::Share (const Handle(StepRepr_DataEnvironment) &ent,
                                          Interface_EntityIterator& iter) const
{

  // Own fields of DataEnvironment

  for (Standard_Integer i1=1; i1 <= ent->Elements()->Length(); i1++ ) {
    Handle(StepRepr_PropertyDefinitionRepresentation) Var0 = ent->Elements()->Value(i1);
    iter.AddItem (Var0);
  }
}

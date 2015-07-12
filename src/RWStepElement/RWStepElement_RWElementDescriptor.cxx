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
#include <RWStepElement_RWElementDescriptor.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_ElementDescriptor.hxx>

//=======================================================================
//function : RWStepElement_RWElementDescriptor
//purpose  : 
//=======================================================================
RWStepElement_RWElementDescriptor::RWStepElement_RWElementDescriptor ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepElement_RWElementDescriptor::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                  const Standard_Integer num,
                                                  Handle(Interface_Check)& ach,
                                                  const Handle(StepElement_ElementDescriptor) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"element_descriptor") ) return;

  // Own fields of ElementDescriptor

  StepElement_ElementOrder aTopologyOrder = StepElement_Linear;
  if (data->ParamType (num, 1) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 1);
    if      (strcmp(text, ".LINEAR.") == 0) aTopologyOrder = StepElement_Linear;
    else if (strcmp(text, ".QUADRATIC.") == 0) aTopologyOrder = StepElement_Quadratic;
    else if (strcmp(text, ".CUBIC.") == 0) aTopologyOrder = StepElement_Cubic;
    else ach->AddFail("Parameter #1 (topology_order) has not allowed value");
  }
  else ach->AddFail("Parameter #1 (topology_order) is not enumeration");

  Handle(TCollection_HAsciiString) aDescription;
  data->ReadString (num, 2, "description", ach, aDescription);

  // Initialize entity
  ent->Init(aTopologyOrder,
            aDescription);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepElement_RWElementDescriptor::WriteStep (StepData_StepWriter& SW,
                                                   const Handle(StepElement_ElementDescriptor) &ent) const
{

  // Own fields of ElementDescriptor

  switch (ent->TopologyOrder()) {
    case StepElement_Linear: SW.SendEnum (".LINEAR."); break;
    case StepElement_Quadratic: SW.SendEnum (".QUADRATIC."); break;
    case StepElement_Cubic: SW.SendEnum (".CUBIC."); break;
  }

  SW.Send (ent->Description());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepElement_RWElementDescriptor::Share (const Handle(StepElement_ElementDescriptor)&,
                                               Interface_EntityIterator&) const
{
  // Own fields of ElementDescriptor
}

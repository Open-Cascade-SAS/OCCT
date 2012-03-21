// Created on: 2002-12-12
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <RWStepElement_RWElementDescriptor.ixx>

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

  StepElement_ElementOrder aTopologyOrder;
  if (data->ParamType (num, 1) == Interface_ParamEnum) {
    Standard_CString text = data->ParamCValue(num, 1);
    if      (strcmp(text, ".LINEAR.")) aTopologyOrder = StepElement_Linear;
    else if (strcmp(text, ".QUADRATIC.")) aTopologyOrder = StepElement_Quadratic;
    else if (strcmp(text, ".CUBIC.")) aTopologyOrder = StepElement_Cubic;
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

void RWStepElement_RWElementDescriptor::Share (const Handle(StepElement_ElementDescriptor) &ent,
                                               Interface_EntityIterator& iter) const
{

  // Own fields of ElementDescriptor
}

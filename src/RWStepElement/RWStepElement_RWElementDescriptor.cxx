// File:	RWStepElement_RWElementDescriptor.cxx
// Created:	Thu Dec 12 17:29:01 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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

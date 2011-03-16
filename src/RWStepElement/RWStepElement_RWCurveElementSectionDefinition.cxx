// File:	RWStepElement_RWCurveElementSectionDefinition.cxx
// Created:	Thu Dec 12 17:29:00 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <RWStepElement_RWCurveElementSectionDefinition.ixx>

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

void RWStepElement_RWCurveElementSectionDefinition::Share (const Handle(StepElement_CurveElementSectionDefinition) &ent,
                                                           Interface_EntityIterator& iter) const
{

  // Own fields of CurveElementSectionDefinition
}

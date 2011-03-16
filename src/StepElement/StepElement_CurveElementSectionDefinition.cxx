// File:	StepElement_CurveElementSectionDefinition.cxx
// Created:	Thu Dec 12 17:29:00 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepElement_CurveElementSectionDefinition.ixx>

//=======================================================================
//function : StepElement_CurveElementSectionDefinition
//purpose  : 
//=======================================================================

StepElement_CurveElementSectionDefinition::StepElement_CurveElementSectionDefinition ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDefinition::Init (const Handle(TCollection_HAsciiString) &aDescription,
                                                      const Standard_Real aSectionAngle)
{

  theDescription = aDescription;

  theSectionAngle = aSectionAngle;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepElement_CurveElementSectionDefinition::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDefinition::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : SectionAngle
//purpose  : 
//=======================================================================

Standard_Real StepElement_CurveElementSectionDefinition::SectionAngle () const
{
  return theSectionAngle;
}

//=======================================================================
//function : SetSectionAngle
//purpose  : 
//=======================================================================

void StepElement_CurveElementSectionDefinition::SetSectionAngle (const Standard_Real aSectionAngle)
{
  theSectionAngle = aSectionAngle;
}

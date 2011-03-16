// File:	StepRepr_DataEnvironment.cxx
// Created:	Thu Dec 12 15:38:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepRepr_DataEnvironment.ixx>

//=======================================================================
//function : StepRepr_DataEnvironment
//purpose  : 
//=======================================================================

StepRepr_DataEnvironment::StepRepr_DataEnvironment ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_DataEnvironment::Init (const Handle(TCollection_HAsciiString) &aName,
                                     const Handle(TCollection_HAsciiString) &aDescription,
                                     const Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation) &aElements)
{

  theName = aName;

  theDescription = aDescription;

  theElements = aElements;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_DataEnvironment::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepRepr_DataEnvironment::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_DataEnvironment::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepRepr_DataEnvironment::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : Elements
//purpose  : 
//=======================================================================

Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation) StepRepr_DataEnvironment::Elements () const
{
  return theElements;
}

//=======================================================================
//function : SetElements
//purpose  : 
//=======================================================================

void StepRepr_DataEnvironment::SetElements (const Handle(StepRepr_HArray1OfPropertyDefinitionRepresentation) &aElements)
{
  theElements = aElements;
}

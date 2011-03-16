// File:	StepRepr_PropertyDefinition.cxx
// Created:	Mon Jul  3 16:29:03 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepRepr_PropertyDefinition.ixx>

//=======================================================================
//function : StepRepr_PropertyDefinition
//purpose  : 
//=======================================================================

StepRepr_PropertyDefinition::StepRepr_PropertyDefinition ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinition::Init (const Handle(TCollection_HAsciiString) &aName,
                                        const Standard_Boolean hasDescription,
                                        const Handle(TCollection_HAsciiString) &aDescription,
                                        const StepRepr_CharacterizedDefinition &aDefinition)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theDefinition = aDefinition;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_PropertyDefinition::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinition::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_PropertyDefinition::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinition::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepRepr_PropertyDefinition::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : Definition
//purpose  : 
//=======================================================================

StepRepr_CharacterizedDefinition StepRepr_PropertyDefinition::Definition () const
{
  return theDefinition;
}

//=======================================================================
//function : SetDefinition
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinition::SetDefinition (const StepRepr_CharacterizedDefinition &aDefinition)
{
  theDefinition = aDefinition;
}

// File:	StepBasic_CharacterizedObject.cxx
// Created:	Thu May 11 16:38:00 2000 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_CharacterizedObject.ixx>
#include <TCollection_HAsciiString.hxx>
//=======================================================================
//function : StepBasic_CharacterizedObject
//purpose  : 
//=======================================================================

StepBasic_CharacterizedObject::StepBasic_CharacterizedObject ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_CharacterizedObject::Init (const Handle(TCollection_HAsciiString) &aName,
                                          const Standard_Boolean hasDescription,
                                          const Handle(TCollection_HAsciiString) &aDescription)
{
  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_CharacterizedObject::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_CharacterizedObject::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_CharacterizedObject::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_CharacterizedObject::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_CharacterizedObject::HasDescription () const
{
  return defDescription;
}

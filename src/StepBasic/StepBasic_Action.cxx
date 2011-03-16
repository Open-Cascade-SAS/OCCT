// File:	StepBasic_Action.cxx
// Created:	Fri Nov 26 16:26:28 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_Action.ixx>

//=======================================================================
//function : StepBasic_Action
//purpose  : 
//=======================================================================

StepBasic_Action::StepBasic_Action ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_Action::Init (const Handle(TCollection_HAsciiString) &aName,
                             const Standard_Boolean hasDescription,
                             const Handle(TCollection_HAsciiString) &aDescription,
                             const Handle(StepBasic_ActionMethod) &aChosenMethod)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theChosenMethod = aChosenMethod;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Action::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_Action::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Action::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_Action::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_Action::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : ChosenMethod
//purpose  : 
//=======================================================================

Handle(StepBasic_ActionMethod) StepBasic_Action::ChosenMethod () const
{
  return theChosenMethod;
}

//=======================================================================
//function : SetChosenMethod
//purpose  : 
//=======================================================================

void StepBasic_Action::SetChosenMethod (const Handle(StepBasic_ActionMethod) &aChosenMethod)
{
  theChosenMethod = aChosenMethod;
}

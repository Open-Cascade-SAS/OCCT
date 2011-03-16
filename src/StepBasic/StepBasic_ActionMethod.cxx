// File:	StepBasic_ActionMethod.cxx
// Created:	Fri Nov 26 16:26:29 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

#include <StepBasic_ActionMethod.ixx>

//=======================================================================
//function : StepBasic_ActionMethod
//purpose  : 
//=======================================================================

StepBasic_ActionMethod::StepBasic_ActionMethod ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ActionMethod::Init (const Handle(TCollection_HAsciiString) &aName,
                                   const Standard_Boolean hasDescription,
                                   const Handle(TCollection_HAsciiString) &aDescription,
                                   const Handle(TCollection_HAsciiString) &aConsequence,
                                   const Handle(TCollection_HAsciiString) &aPurpose)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theConsequence = aConsequence;

  thePurpose = aPurpose;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ActionMethod::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_ActionMethod::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ActionMethod::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_ActionMethod::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_ActionMethod::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : Consequence
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ActionMethod::Consequence () const
{
  return theConsequence;
}

//=======================================================================
//function : SetConsequence
//purpose  : 
//=======================================================================

void StepBasic_ActionMethod::SetConsequence (const Handle(TCollection_HAsciiString) &aConsequence)
{
  theConsequence = aConsequence;
}

//=======================================================================
//function : Purpose
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ActionMethod::Purpose () const
{
  return thePurpose;
}

//=======================================================================
//function : SetPurpose
//purpose  : 
//=======================================================================

void StepBasic_ActionMethod::SetPurpose (const Handle(TCollection_HAsciiString) &aPurpose)
{
  thePurpose = aPurpose;
}

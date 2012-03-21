// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

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

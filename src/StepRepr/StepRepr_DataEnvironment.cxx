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

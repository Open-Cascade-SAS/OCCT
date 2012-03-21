// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepBasic_DocumentRepresentationType.ixx>

//=======================================================================
//function : StepBasic_DocumentRepresentationType
//purpose  : 
//=======================================================================

StepBasic_DocumentRepresentationType::StepBasic_DocumentRepresentationType ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_DocumentRepresentationType::Init (const Handle(TCollection_HAsciiString) &aName,
                                                 const Handle(StepBasic_Document) &aRepresentedDocument)
{

  theName = aName;

  theRepresentedDocument = aRepresentedDocument;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_DocumentRepresentationType::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_DocumentRepresentationType::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : RepresentedDocument
//purpose  : 
//=======================================================================

Handle(StepBasic_Document) StepBasic_DocumentRepresentationType::RepresentedDocument () const
{
  return theRepresentedDocument;
}

//=======================================================================
//function : SetRepresentedDocument
//purpose  : 
//=======================================================================

void StepBasic_DocumentRepresentationType::SetRepresentedDocument (const Handle(StepBasic_Document) &aRepresentedDocument)
{
  theRepresentedDocument = aRepresentedDocument;
}

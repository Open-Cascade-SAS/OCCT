// Created on: 2000-05-11
// Created by: data exchange team
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

#include <StepBasic_DocumentFile.ixx>
#include <StepBasic_CharacterizedObject.hxx>
//=======================================================================
//function : StepBasic_DocumentFile
//purpose  : 
//=======================================================================

StepBasic_DocumentFile::StepBasic_DocumentFile ()
{
  theCharacterizedObject = new StepBasic_CharacterizedObject;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_DocumentFile::Init (const Handle(TCollection_HAsciiString) &aDocument_Id,
                                   const Handle(TCollection_HAsciiString) &aDocument_Name,
                                   const Standard_Boolean hasDocument_Description,
                                   const Handle(TCollection_HAsciiString) &aDocument_Description,
                                   const Handle(StepBasic_DocumentType) &aDocument_Kind,
                                   const Handle(TCollection_HAsciiString) &aCharacterizedObject_Name,
                                   const Standard_Boolean hasCharacterizedObject_Description,
                                   const Handle(TCollection_HAsciiString) &aCharacterizedObject_Description)
{
  StepBasic_Document::Init(aDocument_Id,
                           aDocument_Name,
                           hasDocument_Description,
                           aDocument_Description,
                           aDocument_Kind);
    theCharacterizedObject->Init(aCharacterizedObject_Name,
				 hasCharacterizedObject_Description,
				 aCharacterizedObject_Description);
}

//=======================================================================
//function : CharacterizedObject
//purpose  : 
//=======================================================================

Handle(StepBasic_CharacterizedObject) StepBasic_DocumentFile::CharacterizedObject () const
{
  return theCharacterizedObject;
}

//=======================================================================
//function : SetCharacterizedObject
//purpose  : 
//=======================================================================

void StepBasic_DocumentFile::SetCharacterizedObject (const Handle(StepBasic_CharacterizedObject) &aCharacterizedObject)
{
  theCharacterizedObject = aCharacterizedObject;
}

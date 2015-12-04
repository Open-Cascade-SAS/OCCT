// Created on: 2000-05-11
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <Standard_Type.hxx>
#include <StepBasic_CharacterizedObject.hxx>
#include <StepBasic_DocumentFile.hxx>
#include <StepBasic_DocumentType.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DocumentFile,StepBasic_Document)

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

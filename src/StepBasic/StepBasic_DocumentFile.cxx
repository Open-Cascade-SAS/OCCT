// File:	StepBasic_DocumentFile.cxx
// Created:	Thu May 11 16:38:00 2000 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

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

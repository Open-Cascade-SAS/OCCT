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

#include <RWStepBasic_RWDocumentFile.ixx>
#include <StepBasic_CharacterizedObject.hxx>

//=======================================================================
//function : RWStepBasic_RWDocumentFile
//purpose  : 
//=======================================================================

RWStepBasic_RWDocumentFile::RWStepBasic_RWDocumentFile ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentFile::ReadStep (const Handle(StepData_StepReaderData)& data,
                                           const Standard_Integer num,
                                           Handle(Interface_Check)& ach,
                                           const Handle(StepBasic_DocumentFile) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,6,ach,"document_file") ) return;

  // Inherited fields of Document

  Handle(TCollection_HAsciiString) aDocument_Id;
  data->ReadString (num, 1, "document.id", ach, aDocument_Id);

  Handle(TCollection_HAsciiString) aDocument_Name;
  data->ReadString (num, 2, "document.name", ach, aDocument_Name);

  Handle(TCollection_HAsciiString) aDocument_Description;
  Standard_Boolean hasDocument_Description = Standard_True;
  if ( data->IsParamDefined (num,3) ) {
    data->ReadString (num, 3, "document.description", ach, aDocument_Description);
  }
  else {
    hasDocument_Description = Standard_False;
  }

  Handle(StepBasic_DocumentType) aDocument_Kind;
  data->ReadEntity (num, 4, "document.kind", ach, STANDARD_TYPE(StepBasic_DocumentType), aDocument_Kind);

  // Inherited fields of CharacterizedObject

  Handle(TCollection_HAsciiString) aCharacterizedObject_Name;
  data->ReadString (num, 5, "characterized_object.name", ach, aCharacterizedObject_Name);

  Handle(TCollection_HAsciiString) aCharacterizedObject_Description;
  Standard_Boolean hasCharacterizedObject_Description = Standard_True;
  if ( data->IsParamDefined (num,6) ) {
    data->ReadString (num, 6, "characterized_object.description", ach, aCharacterizedObject_Description);
  }
  else {
    hasCharacterizedObject_Description = Standard_False;
  }

  // Initialize entity
  ent->Init(aDocument_Id,
            aDocument_Name,
            hasDocument_Description,
            aDocument_Description,
            aDocument_Kind,
            aCharacterizedObject_Name,
            hasCharacterizedObject_Description,
            aCharacterizedObject_Description);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentFile::WriteStep (StepData_StepWriter& SW,
                                            const Handle(StepBasic_DocumentFile) &ent) const
{

  // Inherited fields of Document

  SW.Send (ent->StepBasic_Document::Id());

  SW.Send (ent->StepBasic_Document::Name());

  if ( ent->StepBasic_Document::HasDescription() ) {
    SW.Send (ent->StepBasic_Document::Description());
  }
  else SW.SendUndef();

  SW.Send (ent->StepBasic_Document::Kind());

  // Inherited fields of CharacterizedObject

  SW.Send (ent->CharacterizedObject()->Name());

  if ( ent->CharacterizedObject()->HasDescription() ) {
    SW.Send (ent->CharacterizedObject()->Description());
  }
  else SW.SendUndef();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentFile::Share (const Handle(StepBasic_DocumentFile) &ent,
                                        Interface_EntityIterator& iter) const
{

  // Inherited fields of Document

  iter.AddItem (ent->StepBasic_Document::Kind());

  // Inherited fields of CharacterizedObject
}

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

#include <RWStepBasic_RWDocumentRepresentationType.ixx>

//=======================================================================
//function : RWStepBasic_RWDocumentRepresentationType
//purpose  : 
//=======================================================================

RWStepBasic_RWDocumentRepresentationType::RWStepBasic_RWDocumentRepresentationType ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentRepresentationType::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepBasic_DocumentRepresentationType) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,2,ach,"document_representation_type") ) return;

  // Own fields of DocumentRepresentationType

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(StepBasic_Document) aRepresentedDocument;
  data->ReadEntity (num, 2, "represented_document", ach, STANDARD_TYPE(StepBasic_Document), aRepresentedDocument);

  // Initialize entity
  ent->Init(aName,
            aRepresentedDocument);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentRepresentationType::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepBasic_DocumentRepresentationType) &ent) const
{

  // Own fields of DocumentRepresentationType

  SW.Send (ent->Name());

  SW.Send (ent->RepresentedDocument());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentRepresentationType::Share (const Handle(StepBasic_DocumentRepresentationType) &ent,
                                                      Interface_EntityIterator& iter) const
{

  // Own fields of DocumentRepresentationType

  iter.AddItem (ent->RepresentedDocument());
}

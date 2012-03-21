// Created on: 2003-01-28
// Created by: data exchange team
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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

#include <RWStepBasic_RWDocumentProductAssociation.ixx>

//=======================================================================
//function : RWStepBasic_RWDocumentProductAssociation
//purpose  : 
//=======================================================================

RWStepBasic_RWDocumentProductAssociation::RWStepBasic_RWDocumentProductAssociation ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentProductAssociation::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepBasic_DocumentProductAssociation) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"document_product_association") ) return;

  // Own fields of DocumentProductAssociation

  Handle(TCollection_HAsciiString) aName;
  data->ReadString (num, 1, "name", ach, aName);

  Handle(TCollection_HAsciiString) aDescription;
  Standard_Boolean hasDescription = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "description", ach, aDescription);
  }
  else {
    hasDescription = Standard_False;
  }

  Handle(StepBasic_Document) aRelatingDocument;
  data->ReadEntity (num, 3, "relating_document", ach, STANDARD_TYPE(StepBasic_Document), aRelatingDocument);

  StepBasic_ProductOrFormationOrDefinition aRelatedProduct;
  data->ReadEntity (num, 4, "related_product", ach, aRelatedProduct);

  // Initialize entity
  ent->Init(aName,
            hasDescription,
            aDescription,
            aRelatingDocument,
            aRelatedProduct);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentProductAssociation::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepBasic_DocumentProductAssociation) &ent) const
{

  // Own fields of DocumentProductAssociation

  SW.Send (ent->Name());

  if ( ent->HasDescription() ) {
    SW.Send (ent->Description());
  }
  else SW.SendUndef();

  SW.Send (ent->RelatingDocument());

  SW.Send (ent->RelatedProduct().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentProductAssociation::Share (const Handle(StepBasic_DocumentProductAssociation) &ent,
                                                      Interface_EntityIterator& iter) const
{

  // Own fields of DocumentProductAssociation

  iter.AddItem (ent->RelatingDocument());

  iter.AddItem (ent->RelatedProduct().Value());
}

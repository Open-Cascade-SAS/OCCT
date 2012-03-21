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

#include <RWStepBasic_RWDocumentProductEquivalence.ixx>

//=======================================================================
//function : RWStepBasic_RWDocumentProductEquivalence
//purpose  : 
//=======================================================================

RWStepBasic_RWDocumentProductEquivalence::RWStepBasic_RWDocumentProductEquivalence ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentProductEquivalence::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                         const Standard_Integer num,
                                                         Handle(Interface_Check)& ach,
                                                         const Handle(StepBasic_DocumentProductEquivalence) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,4,ach,"document_product_equivalence") ) return;

  // Inherited fields of DocumentProductAssociation

  Handle(TCollection_HAsciiString) aDocumentProductAssociation_Name;
  data->ReadString (num, 1, "document_product_association.name", ach, aDocumentProductAssociation_Name);

  Handle(TCollection_HAsciiString) aDocumentProductAssociation_Description;
  Standard_Boolean hasDocumentProductAssociation_Description = Standard_True;
  if ( data->IsParamDefined (num,2) ) {
    data->ReadString (num, 2, "document_product_association.description", ach, aDocumentProductAssociation_Description);
  }
  else {
    hasDocumentProductAssociation_Description = Standard_False;
  }

  Handle(StepBasic_Document) aDocumentProductAssociation_RelatingDocument;
  data->ReadEntity (num, 3, "document_product_association.relating_document", ach, STANDARD_TYPE(StepBasic_Document), aDocumentProductAssociation_RelatingDocument);

  StepBasic_ProductOrFormationOrDefinition aDocumentProductAssociation_RelatedProduct;
  data->ReadEntity (num, 4, "document_product_association.related_product", ach, aDocumentProductAssociation_RelatedProduct);

  // Initialize entity
  ent->Init(aDocumentProductAssociation_Name,
            hasDocumentProductAssociation_Description,
            aDocumentProductAssociation_Description,
            aDocumentProductAssociation_RelatingDocument,
            aDocumentProductAssociation_RelatedProduct);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentProductEquivalence::WriteStep (StepData_StepWriter& SW,
                                                          const Handle(StepBasic_DocumentProductEquivalence) &ent) const
{

  // Inherited fields of DocumentProductAssociation

  SW.Send (ent->StepBasic_DocumentProductAssociation::Name());

  if ( ent->StepBasic_DocumentProductAssociation::HasDescription() ) {
    SW.Send (ent->StepBasic_DocumentProductAssociation::Description());
  }
  else SW.SendUndef();

  SW.Send (ent->StepBasic_DocumentProductAssociation::RelatingDocument());

  SW.Send (ent->StepBasic_DocumentProductAssociation::RelatedProduct().Value());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWDocumentProductEquivalence::Share (const Handle(StepBasic_DocumentProductEquivalence) &ent,
                                                      Interface_EntityIterator& iter) const
{

  // Inherited fields of DocumentProductAssociation

  iter.AddItem (ent->StepBasic_DocumentProductAssociation::RelatingDocument());

  iter.AddItem (ent->StepBasic_DocumentProductAssociation::RelatedProduct().Value());
}

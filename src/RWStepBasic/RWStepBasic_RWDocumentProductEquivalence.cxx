// Created on: 2003-01-28
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepBasic_RWDocumentProductEquivalence.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentProductEquivalence.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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

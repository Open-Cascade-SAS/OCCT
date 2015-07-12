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
#include <RWStepBasic_RWDocumentProductAssociation.hxx>
#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentProductAssociation.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>

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

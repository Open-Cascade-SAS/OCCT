// File:	RWStepBasic_RWDocumentProductAssociation.cxx
// Created:	Tue Jan 28 12:40:35 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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

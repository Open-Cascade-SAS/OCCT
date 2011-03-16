// File:	StepBasic_DocumentProductAssociation.cxx
// Created:	Tue Jan 28 12:40:35 2003 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepBasic_DocumentProductAssociation.ixx>

//=======================================================================
//function : StepBasic_DocumentProductAssociation
//purpose  : 
//=======================================================================

StepBasic_DocumentProductAssociation::StepBasic_DocumentProductAssociation ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_DocumentProductAssociation::Init (const Handle(TCollection_HAsciiString) &aName,
                                                 const Standard_Boolean hasDescription,
                                                 const Handle(TCollection_HAsciiString) &aDescription,
                                                 const Handle(StepBasic_Document) &aRelatingDocument,
                                                 const StepBasic_ProductOrFormationOrDefinition &aRelatedProduct)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theRelatingDocument = aRelatingDocument;

  theRelatedProduct = aRelatedProduct;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_DocumentProductAssociation::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_DocumentProductAssociation::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_DocumentProductAssociation::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_DocumentProductAssociation::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_DocumentProductAssociation::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : RelatingDocument
//purpose  : 
//=======================================================================

Handle(StepBasic_Document) StepBasic_DocumentProductAssociation::RelatingDocument () const
{
  return theRelatingDocument;
}

//=======================================================================
//function : SetRelatingDocument
//purpose  : 
//=======================================================================

void StepBasic_DocumentProductAssociation::SetRelatingDocument (const Handle(StepBasic_Document) &aRelatingDocument)
{
  theRelatingDocument = aRelatingDocument;
}

//=======================================================================
//function : RelatedProduct
//purpose  : 
//=======================================================================

StepBasic_ProductOrFormationOrDefinition StepBasic_DocumentProductAssociation::RelatedProduct () const
{
  return theRelatedProduct;
}

//=======================================================================
//function : SetRelatedProduct
//purpose  : 
//=======================================================================

void StepBasic_DocumentProductAssociation::SetRelatedProduct (const StepBasic_ProductOrFormationOrDefinition &aRelatedProduct)
{
  theRelatedProduct = aRelatedProduct;
}

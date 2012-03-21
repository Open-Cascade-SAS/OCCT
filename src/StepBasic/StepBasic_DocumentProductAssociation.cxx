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

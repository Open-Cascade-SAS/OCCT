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

#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentProductAssociation.hxx>
#include <StepBasic_ProductOrFormationOrDefinition.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DocumentProductAssociation, Standard_Transient)

//=================================================================================================

StepBasic_DocumentProductAssociation::StepBasic_DocumentProductAssociation()
{
  defDescription = Standard_False;
}

//=================================================================================================

void StepBasic_DocumentProductAssociation::Init(
  const Handle(TCollection_HAsciiString)&         aName,
  const Standard_Boolean                          hasDescription,
  const Handle(TCollection_HAsciiString)&         aDescription,
  const Handle(StepBasic_Document)&               aRelatingDocument,
  const StepBasic_ProductOrFormationOrDefinition& aRelatedProduct)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();

  theRelatingDocument = aRelatingDocument;

  theRelatedProduct = aRelatedProduct;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_DocumentProductAssociation::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_DocumentProductAssociation::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_DocumentProductAssociation::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_DocumentProductAssociation::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Standard_Boolean StepBasic_DocumentProductAssociation::HasDescription() const
{
  return defDescription;
}

//=================================================================================================

Handle(StepBasic_Document) StepBasic_DocumentProductAssociation::RelatingDocument() const
{
  return theRelatingDocument;
}

//=================================================================================================

void StepBasic_DocumentProductAssociation::SetRelatingDocument(
  const Handle(StepBasic_Document)& aRelatingDocument)
{
  theRelatingDocument = aRelatingDocument;
}

//=================================================================================================

StepBasic_ProductOrFormationOrDefinition StepBasic_DocumentProductAssociation::RelatedProduct()
  const
{
  return theRelatedProduct;
}

//=================================================================================================

void StepBasic_DocumentProductAssociation::SetRelatedProduct(
  const StepBasic_ProductOrFormationOrDefinition& aRelatedProduct)
{
  theRelatedProduct = aRelatedProduct;
}

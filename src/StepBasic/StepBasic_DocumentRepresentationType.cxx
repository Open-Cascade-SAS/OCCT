// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepBasic_Document.hxx>
#include <StepBasic_DocumentRepresentationType.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_DocumentRepresentationType, Standard_Transient)

//=================================================================================================

StepBasic_DocumentRepresentationType::StepBasic_DocumentRepresentationType() {}

//=================================================================================================

void StepBasic_DocumentRepresentationType::Init(
  const Handle(TCollection_HAsciiString)& aName,
  const Handle(StepBasic_Document)&       aRepresentedDocument)
{

  theName = aName;

  theRepresentedDocument = aRepresentedDocument;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_DocumentRepresentationType::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_DocumentRepresentationType::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(StepBasic_Document) StepBasic_DocumentRepresentationType::RepresentedDocument() const
{
  return theRepresentedDocument;
}

//=================================================================================================

void StepBasic_DocumentRepresentationType::SetRepresentedDocument(
  const Handle(StepBasic_Document)& aRepresentedDocument)
{
  theRepresentedDocument = aRepresentedDocument;
}

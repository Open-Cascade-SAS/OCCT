// File:	StepBasic_DocumentRepresentationType.cxx
// Created:	Wed May 10 15:09:06 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_DocumentRepresentationType.ixx>

//=======================================================================
//function : StepBasic_DocumentRepresentationType
//purpose  : 
//=======================================================================

StepBasic_DocumentRepresentationType::StepBasic_DocumentRepresentationType ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_DocumentRepresentationType::Init (const Handle(TCollection_HAsciiString) &aName,
                                                 const Handle(StepBasic_Document) &aRepresentedDocument)
{

  theName = aName;

  theRepresentedDocument = aRepresentedDocument;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_DocumentRepresentationType::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_DocumentRepresentationType::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : RepresentedDocument
//purpose  : 
//=======================================================================

Handle(StepBasic_Document) StepBasic_DocumentRepresentationType::RepresentedDocument () const
{
  return theRepresentedDocument;
}

//=======================================================================
//function : SetRepresentedDocument
//purpose  : 
//=======================================================================

void StepBasic_DocumentRepresentationType::SetRepresentedDocument (const Handle(StepBasic_Document) &aRepresentedDocument)
{
  theRepresentedDocument = aRepresentedDocument;
}

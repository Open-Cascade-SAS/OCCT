// File:	StepBasic_Document.cxx
// Created:	Thu May 11 16:38:00 2000 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_Document.ixx>

//=======================================================================
//function : StepBasic_Document
//purpose  : 
//=======================================================================

StepBasic_Document::StepBasic_Document ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_Document::Init (const Handle(TCollection_HAsciiString) &aId,
                               const Handle(TCollection_HAsciiString) &aName,
                               const Standard_Boolean hasDescription,
                               const Handle(TCollection_HAsciiString) &aDescription,
                               const Handle(StepBasic_DocumentType) &aKind)
{

  theId = aId;

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theKind = aKind;
}

//=======================================================================
//function : Id
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Document::Id () const
{
  return theId;
}

//=======================================================================
//function : SetId
//purpose  : 
//=======================================================================

void StepBasic_Document::SetId (const Handle(TCollection_HAsciiString) &aId)
{
  theId = aId;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Document::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_Document::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Document::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_Document::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_Document::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : Kind
//purpose  : 
//=======================================================================

Handle(StepBasic_DocumentType) StepBasic_Document::Kind () const
{
  return theKind;
}

//=======================================================================
//function : SetKind
//purpose  : 
//=======================================================================

void StepBasic_Document::SetKind (const Handle(StepBasic_DocumentType) &aKind)
{
  theKind = aKind;
}

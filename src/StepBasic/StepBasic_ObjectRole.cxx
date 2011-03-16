// File:	StepBasic_ObjectRole.cxx
// Created:	Wed May 10 15:09:08 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_ObjectRole.ixx>

//=======================================================================
//function : StepBasic_ObjectRole
//purpose  : 
//=======================================================================

StepBasic_ObjectRole::StepBasic_ObjectRole ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ObjectRole::Init (const Handle(TCollection_HAsciiString) &aName,
                                 const Standard_Boolean hasDescription,
                                 const Handle(TCollection_HAsciiString) &aDescription)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ObjectRole::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_ObjectRole::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ObjectRole::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_ObjectRole::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_ObjectRole::HasDescription () const
{
  return defDescription;
}

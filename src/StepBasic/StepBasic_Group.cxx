// File:	StepBasic_Group.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_Group.ixx>

//=======================================================================
//function : StepBasic_Group
//purpose  : 
//=======================================================================

StepBasic_Group::StepBasic_Group ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_Group::Init (const Handle(TCollection_HAsciiString) &aName,
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

Handle(TCollection_HAsciiString) StepBasic_Group::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_Group::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_Group::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_Group::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_Group::HasDescription () const
{
  return defDescription;
}

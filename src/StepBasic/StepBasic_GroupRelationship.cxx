// File:	StepBasic_GroupRelationship.cxx
// Created:	Wed May 10 15:09:07 2000 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1
// Copyright:	Matra Datavision 2000

#include <StepBasic_GroupRelationship.ixx>

//=======================================================================
//function : StepBasic_GroupRelationship
//purpose  : 
//=======================================================================

StepBasic_GroupRelationship::StepBasic_GroupRelationship ()
{
  defDescription = Standard_False;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_GroupRelationship::Init (const Handle(TCollection_HAsciiString) &aName,
                                        const Standard_Boolean hasDescription,
                                        const Handle(TCollection_HAsciiString) &aDescription,
                                        const Handle(StepBasic_Group) &aRelatingGroup,
                                        const Handle(StepBasic_Group) &aRelatedGroup)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription) {
    theDescription = aDescription;
  }
  else theDescription.Nullify();

  theRelatingGroup = aRelatingGroup;

  theRelatedGroup = aRelatedGroup;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_GroupRelationship::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_GroupRelationship::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_GroupRelationship::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_GroupRelationship::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : HasDescription
//purpose  : 
//=======================================================================

Standard_Boolean StepBasic_GroupRelationship::HasDescription () const
{
  return defDescription;
}

//=======================================================================
//function : RelatingGroup
//purpose  : 
//=======================================================================

Handle(StepBasic_Group) StepBasic_GroupRelationship::RelatingGroup () const
{
  return theRelatingGroup;
}

//=======================================================================
//function : SetRelatingGroup
//purpose  : 
//=======================================================================

void StepBasic_GroupRelationship::SetRelatingGroup (const Handle(StepBasic_Group) &aRelatingGroup)
{
  theRelatingGroup = aRelatingGroup;
}

//=======================================================================
//function : RelatedGroup
//purpose  : 
//=======================================================================

Handle(StepBasic_Group) StepBasic_GroupRelationship::RelatedGroup () const
{
  return theRelatedGroup;
}

//=======================================================================
//function : SetRelatedGroup
//purpose  : 
//=======================================================================

void StepBasic_GroupRelationship::SetRelatedGroup (const Handle(StepBasic_Group) &aRelatedGroup)
{
  theRelatedGroup = aRelatedGroup;
}

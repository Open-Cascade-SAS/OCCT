// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

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

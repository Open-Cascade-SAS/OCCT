// Created on: 2002-12-15
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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

#include <StepBasic_ProductDefinitionFormationRelationship.ixx>

//=======================================================================
//function : StepBasic_ProductDefinitionFormationRelationship
//purpose  : 
//=======================================================================

StepBasic_ProductDefinitionFormationRelationship::StepBasic_ProductDefinitionFormationRelationship ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ProductDefinitionFormationRelationship::Init (const Handle(TCollection_HAsciiString) &aId,
                                                             const Handle(TCollection_HAsciiString) &aName,
                                                             const Handle(TCollection_HAsciiString) &aDescription,
                                                             const Handle(StepBasic_ProductDefinitionFormation) &aRelatingProductDefinitionFormation,
                                                             const Handle(StepBasic_ProductDefinitionFormation) &aRelatedProductDefinitionFormation)
{

  theId = aId;

  theName = aName;

  theDescription = aDescription;

  theRelatingProductDefinitionFormation = aRelatingProductDefinitionFormation;

  theRelatedProductDefinitionFormation = aRelatedProductDefinitionFormation;
}

//=======================================================================
//function : Id
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionFormationRelationship::Id () const
{
  return theId;
}

//=======================================================================
//function : SetId
//purpose  : 
//=======================================================================

void StepBasic_ProductDefinitionFormationRelationship::SetId (const Handle(TCollection_HAsciiString) &aId)
{
  theId = aId;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionFormationRelationship::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepBasic_ProductDefinitionFormationRelationship::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ProductDefinitionFormationRelationship::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepBasic_ProductDefinitionFormationRelationship::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : RelatingProductDefinitionFormation
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionFormation) StepBasic_ProductDefinitionFormationRelationship::RelatingProductDefinitionFormation () const
{
  return theRelatingProductDefinitionFormation;
}

//=======================================================================
//function : SetRelatingProductDefinitionFormation
//purpose  : 
//=======================================================================

void StepBasic_ProductDefinitionFormationRelationship::SetRelatingProductDefinitionFormation (const Handle(StepBasic_ProductDefinitionFormation) &aRelatingProductDefinitionFormation)
{
  theRelatingProductDefinitionFormation = aRelatingProductDefinitionFormation;
}

//=======================================================================
//function : RelatedProductDefinitionFormation
//purpose  : 
//=======================================================================

Handle(StepBasic_ProductDefinitionFormation) StepBasic_ProductDefinitionFormationRelationship::RelatedProductDefinitionFormation () const
{
  return theRelatedProductDefinitionFormation;
}

//=======================================================================
//function : SetRelatedProductDefinitionFormation
//purpose  : 
//=======================================================================

void StepBasic_ProductDefinitionFormationRelationship::SetRelatedProductDefinitionFormation (const Handle(StepBasic_ProductDefinitionFormation) &aRelatedProductDefinitionFormation)
{
  theRelatedProductDefinitionFormation = aRelatedProductDefinitionFormation;
}

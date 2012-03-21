// Created on: 2002-12-12
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

#include <StepRepr_PropertyDefinitionRelationship.ixx>

//=======================================================================
//function : StepRepr_PropertyDefinitionRelationship
//purpose  : 
//=======================================================================

StepRepr_PropertyDefinitionRelationship::StepRepr_PropertyDefinitionRelationship ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRelationship::Init (const Handle(TCollection_HAsciiString) &aName,
                                                    const Handle(TCollection_HAsciiString) &aDescription,
                                                    const Handle(StepRepr_PropertyDefinition) &aRelatingPropertyDefinition,
                                                    const Handle(StepRepr_PropertyDefinition) &aRelatedPropertyDefinition)
{

  theName = aName;

  theDescription = aDescription;

  theRelatingPropertyDefinition = aRelatingPropertyDefinition;

  theRelatedPropertyDefinition = aRelatedPropertyDefinition;
}

//=======================================================================
//function : Name
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_PropertyDefinitionRelationship::Name () const
{
  return theName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRelationship::SetName (const Handle(TCollection_HAsciiString) &aName)
{
  theName = aName;
}

//=======================================================================
//function : Description
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepRepr_PropertyDefinitionRelationship::Description () const
{
  return theDescription;
}

//=======================================================================
//function : SetDescription
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRelationship::SetDescription (const Handle(TCollection_HAsciiString) &aDescription)
{
  theDescription = aDescription;
}

//=======================================================================
//function : RelatingPropertyDefinition
//purpose  : 
//=======================================================================

Handle(StepRepr_PropertyDefinition) StepRepr_PropertyDefinitionRelationship::RelatingPropertyDefinition () const
{
  return theRelatingPropertyDefinition;
}

//=======================================================================
//function : SetRelatingPropertyDefinition
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRelationship::SetRelatingPropertyDefinition (const Handle(StepRepr_PropertyDefinition) &aRelatingPropertyDefinition)
{
  theRelatingPropertyDefinition = aRelatingPropertyDefinition;
}

//=======================================================================
//function : RelatedPropertyDefinition
//purpose  : 
//=======================================================================

Handle(StepRepr_PropertyDefinition) StepRepr_PropertyDefinitionRelationship::RelatedPropertyDefinition () const
{
  return theRelatedPropertyDefinition;
}

//=======================================================================
//function : SetRelatedPropertyDefinition
//purpose  : 
//=======================================================================

void StepRepr_PropertyDefinitionRelationship::SetRelatedPropertyDefinition (const Handle(StepRepr_PropertyDefinition) &aRelatedPropertyDefinition)
{
  theRelatedPropertyDefinition = aRelatedPropertyDefinition;
}

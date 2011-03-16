// File:	StepRepr_PropertyDefinitionRelationship.cxx
// Created:	Thu Dec 12 16:04:59 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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

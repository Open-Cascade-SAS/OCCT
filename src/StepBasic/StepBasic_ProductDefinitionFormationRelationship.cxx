// File:	StepBasic_ProductDefinitionFormationRelationship.cxx
// Created:	Sun Dec 15 10:59:25 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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

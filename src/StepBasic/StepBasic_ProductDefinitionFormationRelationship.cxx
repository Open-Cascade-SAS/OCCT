// Created on: 2002-12-15
// Created by: data exchange team
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Standard_Type.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepBasic_ProductDefinitionFormationRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductDefinitionFormationRelationship,Standard_Transient)

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

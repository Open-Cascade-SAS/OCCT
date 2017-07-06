// Created on: 2002-12-12
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
#include <StepRepr_PropertyDefinition.hxx>
#include <StepRepr_PropertyDefinitionRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_PropertyDefinitionRelationship,Standard_Transient)

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

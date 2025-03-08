// Created on: 2000-05-10
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.1

#include <StepBasic_Group.hxx>
#include <StepBasic_GroupRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_GroupRelationship, Standard_Transient)

//=================================================================================================

StepBasic_GroupRelationship::StepBasic_GroupRelationship()
{
  defDescription = Standard_False;
}

//=================================================================================================

void StepBasic_GroupRelationship::Init(const Handle(TCollection_HAsciiString)& aName,
                                       const Standard_Boolean                  hasDescription,
                                       const Handle(TCollection_HAsciiString)& aDescription,
                                       const Handle(StepBasic_Group)&          aRelatingGroup,
                                       const Handle(StepBasic_Group)&          aRelatedGroup)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();

  theRelatingGroup = aRelatingGroup;

  theRelatedGroup = aRelatedGroup;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_GroupRelationship::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_GroupRelationship::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_GroupRelationship::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_GroupRelationship::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Standard_Boolean StepBasic_GroupRelationship::HasDescription() const
{
  return defDescription;
}

//=================================================================================================

Handle(StepBasic_Group) StepBasic_GroupRelationship::RelatingGroup() const
{
  return theRelatingGroup;
}

//=================================================================================================

void StepBasic_GroupRelationship::SetRelatingGroup(const Handle(StepBasic_Group)& aRelatingGroup)
{
  theRelatingGroup = aRelatingGroup;
}

//=================================================================================================

Handle(StepBasic_Group) StepBasic_GroupRelationship::RelatedGroup() const
{
  return theRelatedGroup;
}

//=================================================================================================

void StepBasic_GroupRelationship::SetRelatedGroup(const Handle(StepBasic_Group)& aRelatedGroup)
{
  theRelatedGroup = aRelatedGroup;
}

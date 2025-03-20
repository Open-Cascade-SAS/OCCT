// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <StepBasic_GeneralPropertyAssociation.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_GeneralPropertyAssociation, Standard_Transient)

//=================================================================================================

StepBasic_GeneralPropertyAssociation::StepBasic_GeneralPropertyAssociation() {}

//=================================================================================================

void StepBasic_GeneralPropertyAssociation::Init(
  const Handle(TCollection_HAsciiString)&    aName,
  const Handle(TCollection_HAsciiString)&    aDescription,
  const Handle(StepBasic_GeneralProperty)&   aGeneralProperty,
  const Handle(StepRepr_PropertyDefinition)& aPropertyDefinition)
{
  theName = aName;

  theDescription = aDescription;

  theGeneralProperty = aGeneralProperty;

  thePropertyDefinition = aPropertyDefinition;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_GeneralPropertyAssociation::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_GeneralPropertyAssociation::SetName(const Handle(TCollection_HAsciiString)& aName)
{
  theName = aName;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepBasic_GeneralPropertyAssociation::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_GeneralPropertyAssociation::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Handle(StepBasic_GeneralProperty) StepBasic_GeneralPropertyAssociation::GeneralProperty() const
{
  return theGeneralProperty;
}

//=================================================================================================

void StepBasic_GeneralPropertyAssociation::SetGeneralProperty(
  const Handle(StepBasic_GeneralProperty)& aGeneralProperty)
{
  theGeneralProperty = aGeneralProperty;
}

//=================================================================================================

Handle(StepRepr_PropertyDefinition) StepBasic_GeneralPropertyAssociation::PropertyDefinition() const
{
  return thePropertyDefinition;
}

//=================================================================================================

void StepBasic_GeneralPropertyAssociation::SetPropertyDefinition(
  const Handle(StepRepr_PropertyDefinition)& aPropertyDefinition)
{
  thePropertyDefinition = aPropertyDefinition;
}

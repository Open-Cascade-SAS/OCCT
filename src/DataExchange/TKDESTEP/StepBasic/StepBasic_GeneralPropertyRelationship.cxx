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

#include <StepBasic_GeneralProperty.hxx>
#include <StepBasic_GeneralPropertyRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_GeneralPropertyRelationship, Standard_Transient)

//=================================================================================================

StepBasic_GeneralPropertyRelationship::StepBasic_GeneralPropertyRelationship() {}

//=================================================================================================

void StepBasic_GeneralPropertyRelationship::Init(
  const occ::handle<TCollection_HAsciiString>&  aName,
  const bool                   hasDescription,
  const occ::handle<TCollection_HAsciiString>&  aDescription,
  const occ::handle<StepBasic_GeneralProperty>& aRelatingGeneralProperty,
  const occ::handle<StepBasic_GeneralProperty>& aRelatedGeneralProperty)
{
  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();

  theRelatingGeneralProperty = aRelatingGeneralProperty;

  theRelatedGeneralProperty = aRelatedGeneralProperty;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_GeneralPropertyRelationship::Name() const
{
  return theName;
}

//=================================================================================================

void StepBasic_GeneralPropertyRelationship::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  theName = aName;
}

//=================================================================================================

bool StepBasic_GeneralPropertyRelationship::HasDescription() const
{
  return defDescription;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepBasic_GeneralPropertyRelationship::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepBasic_GeneralPropertyRelationship::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  defDescription = true;
  if (aDescription.IsNull())
  {
    defDescription = false;
  }
  theDescription = aDescription;
}

//=================================================================================================

occ::handle<StepBasic_GeneralProperty> StepBasic_GeneralPropertyRelationship::RelatingGeneralProperty()
  const
{
  return theRelatingGeneralProperty;
}

//=================================================================================================

void StepBasic_GeneralPropertyRelationship::SetRelatingGeneralProperty(
  const occ::handle<StepBasic_GeneralProperty>& aRelatingGeneralProperty)
{
  theRelatingGeneralProperty = aRelatingGeneralProperty;
}

//=================================================================================================

occ::handle<StepBasic_GeneralProperty> StepBasic_GeneralPropertyRelationship::RelatedGeneralProperty()
  const
{
  return theRelatedGeneralProperty;
}

//=================================================================================================

void StepBasic_GeneralPropertyRelationship::SetRelatedGeneralProperty(
  const occ::handle<StepBasic_GeneralProperty>& aRelatedGeneralProperty)
{
  theRelatedGeneralProperty = aRelatedGeneralProperty;
}

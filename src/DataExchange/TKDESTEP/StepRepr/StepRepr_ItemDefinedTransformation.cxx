// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <StepRepr_ItemDefinedTransformation.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ItemDefinedTransformation, Standard_Transient)

StepRepr_ItemDefinedTransformation::StepRepr_ItemDefinedTransformation() {}

void StepRepr_ItemDefinedTransformation::Init(
  const occ::handle<TCollection_HAsciiString>&    aName,
  const occ::handle<TCollection_HAsciiString>&    aDescription,
  const occ::handle<StepRepr_RepresentationItem>& aTransformItem1,
  const occ::handle<StepRepr_RepresentationItem>& aTransformItem2)
{
  theName           = aName;
  theDescription    = aDescription;
  theTransformItem1 = aTransformItem1;
  theTransformItem2 = aTransformItem2;
}

void StepRepr_ItemDefinedTransformation::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  theName = aName;
}

occ::handle<TCollection_HAsciiString> StepRepr_ItemDefinedTransformation::Name() const
{
  return theName;
}

void StepRepr_ItemDefinedTransformation::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  theDescription = aDescription;
}

occ::handle<TCollection_HAsciiString> StepRepr_ItemDefinedTransformation::Description() const
{
  return theDescription;
}

void StepRepr_ItemDefinedTransformation::SetTransformItem1(
  const occ::handle<StepRepr_RepresentationItem>& aTransformItem1)
{
  theTransformItem1 = aTransformItem1;
}

occ::handle<StepRepr_RepresentationItem> StepRepr_ItemDefinedTransformation::TransformItem1() const
{
  return theTransformItem1;
}

void StepRepr_ItemDefinedTransformation::SetTransformItem2(
  const occ::handle<StepRepr_RepresentationItem>& aTransformItem2)
{
  theTransformItem2 = aTransformItem2;
}

occ::handle<StepRepr_RepresentationItem> StepRepr_ItemDefinedTransformation::TransformItem2() const
{
  return theTransformItem2;
}

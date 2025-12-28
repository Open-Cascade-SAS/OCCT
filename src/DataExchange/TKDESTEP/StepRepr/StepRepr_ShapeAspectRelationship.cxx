// Created on: 2000-04-18
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepRepr_ShapeAspect.hxx>
#include <StepRepr_ShapeAspectRelationship.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ShapeAspectRelationship, Standard_Transient)

//=================================================================================================

StepRepr_ShapeAspectRelationship::StepRepr_ShapeAspectRelationship()
{
  defDescription = false;
}

//=================================================================================================

void StepRepr_ShapeAspectRelationship::Init(
  const occ::handle<TCollection_HAsciiString>& aName,
  const bool                  hasDescription,
  const occ::handle<TCollection_HAsciiString>& aDescription,
  const occ::handle<StepRepr_ShapeAspect>&     aRelatingShapeAspect,
  const occ::handle<StepRepr_ShapeAspect>&     aRelatedShapeAspect)
{

  theName = aName;

  defDescription = hasDescription;
  if (defDescription)
  {
    theDescription = aDescription;
  }
  else
    theDescription.Nullify();

  theRelatingShapeAspect = aRelatingShapeAspect;

  theRelatedShapeAspect = aRelatedShapeAspect;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepRepr_ShapeAspectRelationship::Name() const
{
  return theName;
}

//=================================================================================================

void StepRepr_ShapeAspectRelationship::SetName(const occ::handle<TCollection_HAsciiString>& aName)
{
  theName = aName;
}

//=================================================================================================

occ::handle<TCollection_HAsciiString> StepRepr_ShapeAspectRelationship::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepRepr_ShapeAspectRelationship::SetDescription(
  const occ::handle<TCollection_HAsciiString>& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

bool StepRepr_ShapeAspectRelationship::HasDescription() const
{
  return defDescription;
}

//=================================================================================================

occ::handle<StepRepr_ShapeAspect> StepRepr_ShapeAspectRelationship::RelatingShapeAspect() const
{
  return theRelatingShapeAspect;
}

//=================================================================================================

void StepRepr_ShapeAspectRelationship::SetRelatingShapeAspect(
  const occ::handle<StepRepr_ShapeAspect>& aRelatingShapeAspect)
{
  theRelatingShapeAspect = aRelatingShapeAspect;
}

//=================================================================================================

occ::handle<StepRepr_ShapeAspect> StepRepr_ShapeAspectRelationship::RelatedShapeAspect() const
{
  return theRelatedShapeAspect;
}

//=================================================================================================

void StepRepr_ShapeAspectRelationship::SetRelatedShapeAspect(
  const occ::handle<StepRepr_ShapeAspect>& aRelatedShapeAspect)
{
  theRelatedShapeAspect = aRelatedShapeAspect;
}

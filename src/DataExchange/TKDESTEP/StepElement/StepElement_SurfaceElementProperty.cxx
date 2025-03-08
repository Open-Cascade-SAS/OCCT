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

#include <StepElement_SurfaceElementProperty.hxx>
#include <StepElement_SurfaceSectionField.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_SurfaceElementProperty, Standard_Transient)

//=================================================================================================

StepElement_SurfaceElementProperty::StepElement_SurfaceElementProperty() {}

//=================================================================================================

void StepElement_SurfaceElementProperty::Init(
  const Handle(TCollection_HAsciiString)&        aPropertyId,
  const Handle(TCollection_HAsciiString)&        aDescription,
  const Handle(StepElement_SurfaceSectionField)& aSection)
{

  thePropertyId = aPropertyId;

  theDescription = aDescription;

  theSection = aSection;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepElement_SurfaceElementProperty::PropertyId() const
{
  return thePropertyId;
}

//=================================================================================================

void StepElement_SurfaceElementProperty::SetPropertyId(
  const Handle(TCollection_HAsciiString)& aPropertyId)
{
  thePropertyId = aPropertyId;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepElement_SurfaceElementProperty::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepElement_SurfaceElementProperty::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Handle(StepElement_SurfaceSectionField) StepElement_SurfaceElementProperty::Section() const
{
  return theSection;
}

//=================================================================================================

void StepElement_SurfaceElementProperty::SetSection(
  const Handle(StepElement_SurfaceSectionField)& aSection)
{
  theSection = aSection;
}

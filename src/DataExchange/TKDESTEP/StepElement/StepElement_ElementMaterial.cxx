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

#include <StepElement_ElementMaterial.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepElement_ElementMaterial, Standard_Transient)

//=================================================================================================

StepElement_ElementMaterial::StepElement_ElementMaterial() {}

//=================================================================================================

void StepElement_ElementMaterial::Init(
  const Handle(TCollection_HAsciiString)&                         aMaterialId,
  const Handle(TCollection_HAsciiString)&                         aDescription,
  const Handle(StepRepr_HArray1OfMaterialPropertyRepresentation)& aProperties)
{

  theMaterialId = aMaterialId;

  theDescription = aDescription;

  theProperties = aProperties;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepElement_ElementMaterial::MaterialId() const
{
  return theMaterialId;
}

//=================================================================================================

void StepElement_ElementMaterial::SetMaterialId(const Handle(TCollection_HAsciiString)& aMaterialId)
{
  theMaterialId = aMaterialId;
}

//=================================================================================================

Handle(TCollection_HAsciiString) StepElement_ElementMaterial::Description() const
{
  return theDescription;
}

//=================================================================================================

void StepElement_ElementMaterial::SetDescription(
  const Handle(TCollection_HAsciiString)& aDescription)
{
  theDescription = aDescription;
}

//=================================================================================================

Handle(StepRepr_HArray1OfMaterialPropertyRepresentation) StepElement_ElementMaterial::Properties()
  const
{
  return theProperties;
}

//=================================================================================================

void StepElement_ElementMaterial::SetProperties(
  const Handle(StepRepr_HArray1OfMaterialPropertyRepresentation)& aProperties)
{
  theProperties = aProperties;
}

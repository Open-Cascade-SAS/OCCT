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
#include <StepElement_Volume3dElementDescriptor.hxx>
#include <StepFEA_FeaModel3d.hxx>
#include <StepFEA_Volume3dElementRepresentation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_Volume3dElementRepresentation, StepFEA_ElementRepresentation)

//=================================================================================================

StepFEA_Volume3dElementRepresentation::StepFEA_Volume3dElementRepresentation() {}

//=================================================================================================

void StepFEA_Volume3dElementRepresentation::Init(
  const Handle(TCollection_HAsciiString)&              aRepresentation_Name,
  const Handle(StepRepr_HArray1OfRepresentationItem)&  aRepresentation_Items,
  const Handle(StepRepr_RepresentationContext)&        aRepresentation_ContextOfItems,
  const Handle(StepFEA_HArray1OfNodeRepresentation)&   aElementRepresentation_NodeList,
  const Handle(StepFEA_FeaModel3d)&                    aModelRef,
  const Handle(StepElement_Volume3dElementDescriptor)& aElementDescriptor,
  const Handle(StepElement_ElementMaterial)&           aMaterial)
{
  StepFEA_ElementRepresentation::Init(aRepresentation_Name,
                                      aRepresentation_Items,
                                      aRepresentation_ContextOfItems,
                                      aElementRepresentation_NodeList);

  theModelRef = aModelRef;

  theElementDescriptor = aElementDescriptor;

  theMaterial = aMaterial;
}

//=================================================================================================

Handle(StepFEA_FeaModel3d) StepFEA_Volume3dElementRepresentation::ModelRef() const
{
  return theModelRef;
}

//=================================================================================================

void StepFEA_Volume3dElementRepresentation::SetModelRef(const Handle(StepFEA_FeaModel3d)& aModelRef)
{
  theModelRef = aModelRef;
}

//=================================================================================================

Handle(StepElement_Volume3dElementDescriptor) StepFEA_Volume3dElementRepresentation::
  ElementDescriptor() const
{
  return theElementDescriptor;
}

//=================================================================================================

void StepFEA_Volume3dElementRepresentation::SetElementDescriptor(
  const Handle(StepElement_Volume3dElementDescriptor)& aElementDescriptor)
{
  theElementDescriptor = aElementDescriptor;
}

//=================================================================================================

Handle(StepElement_ElementMaterial) StepFEA_Volume3dElementRepresentation::Material() const
{
  return theMaterial;
}

//=================================================================================================

void StepFEA_Volume3dElementRepresentation::SetMaterial(
  const Handle(StepElement_ElementMaterial)& aMaterial)
{
  theMaterial = aMaterial;
}

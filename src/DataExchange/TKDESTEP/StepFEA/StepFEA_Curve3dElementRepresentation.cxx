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

#include <StepElement_Curve3dElementDescriptor.hxx>
#include <StepElement_ElementMaterial.hxx>
#include <StepFEA_Curve3dElementProperty.hxx>
#include <StepFEA_Curve3dElementRepresentation.hxx>
#include <StepFEA_FeaModel3d.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_Curve3dElementRepresentation, StepFEA_ElementRepresentation)

//=================================================================================================

StepFEA_Curve3dElementRepresentation::StepFEA_Curve3dElementRepresentation() {}

//=================================================================================================

void StepFEA_Curve3dElementRepresentation::Init(
  const occ::handle<TCollection_HAsciiString>&             aRepresentation_Name,
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>& aRepresentation_Items,
  const occ::handle<StepRepr_RepresentationContext>&       aRepresentation_ContextOfItems,
  const occ::handle<NCollection_HArray1<occ::handle<StepFEA_NodeRepresentation>>>&  aElementRepresentation_NodeList,
  const occ::handle<StepFEA_FeaModel3d>&                   aModelRef,
  const occ::handle<StepElement_Curve3dElementDescriptor>& aElementDescriptor,
  const occ::handle<StepFEA_Curve3dElementProperty>&       aProperty,
  const occ::handle<StepElement_ElementMaterial>&          aMaterial)
{
  StepFEA_ElementRepresentation::Init(aRepresentation_Name,
                                      aRepresentation_Items,
                                      aRepresentation_ContextOfItems,
                                      aElementRepresentation_NodeList);

  theModelRef = aModelRef;

  theElementDescriptor = aElementDescriptor;

  theProperty = aProperty;

  theMaterial = aMaterial;
}

//=================================================================================================

occ::handle<StepFEA_FeaModel3d> StepFEA_Curve3dElementRepresentation::ModelRef() const
{
  return theModelRef;
}

//=================================================================================================

void StepFEA_Curve3dElementRepresentation::SetModelRef(const occ::handle<StepFEA_FeaModel3d>& aModelRef)
{
  theModelRef = aModelRef;
}

//=================================================================================================

occ::handle<StepElement_Curve3dElementDescriptor> StepFEA_Curve3dElementRepresentation::
  ElementDescriptor() const
{
  return theElementDescriptor;
}

//=================================================================================================

void StepFEA_Curve3dElementRepresentation::SetElementDescriptor(
  const occ::handle<StepElement_Curve3dElementDescriptor>& aElementDescriptor)
{
  theElementDescriptor = aElementDescriptor;
}

//=================================================================================================

occ::handle<StepFEA_Curve3dElementProperty> StepFEA_Curve3dElementRepresentation::Property() const
{
  return theProperty;
}

//=================================================================================================

void StepFEA_Curve3dElementRepresentation::SetProperty(
  const occ::handle<StepFEA_Curve3dElementProperty>& aProperty)
{
  theProperty = aProperty;
}

//=================================================================================================

occ::handle<StepElement_ElementMaterial> StepFEA_Curve3dElementRepresentation::Material() const
{
  return theMaterial;
}

//=================================================================================================

void StepFEA_Curve3dElementRepresentation::SetMaterial(
  const occ::handle<StepElement_ElementMaterial>& aMaterial)
{
  theMaterial = aMaterial;
}

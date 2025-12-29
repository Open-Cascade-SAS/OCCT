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
#include <StepElement_Surface3dElementDescriptor.hxx>
#include <StepElement_SurfaceElementProperty.hxx>
#include <StepFEA_FeaModel3d.hxx>
#include <StepFEA_Surface3dElementRepresentation.hxx>
#include <StepRepr_RepresentationContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepFEA_Surface3dElementRepresentation, StepFEA_ElementRepresentation)

//=================================================================================================

StepFEA_Surface3dElementRepresentation::StepFEA_Surface3dElementRepresentation() = default;

//=================================================================================================

void StepFEA_Surface3dElementRepresentation::Init(
  const occ::handle<TCollection_HAsciiString>& aRepresentation_Name,
  const occ::handle<NCollection_HArray1<occ::handle<StepRepr_RepresentationItem>>>&
                                                     aRepresentation_Items,
  const occ::handle<StepRepr_RepresentationContext>& aRepresentation_ContextOfItems,
  const occ::handle<NCollection_HArray1<occ::handle<StepFEA_NodeRepresentation>>>&
                                                             aElementRepresentation_NodeList,
  const occ::handle<StepFEA_FeaModel3d>&                     aModelRef,
  const occ::handle<StepElement_Surface3dElementDescriptor>& aElementDescriptor,
  const occ::handle<StepElement_SurfaceElementProperty>&     aProperty,
  const occ::handle<StepElement_ElementMaterial>&            aMaterial)
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

occ::handle<StepFEA_FeaModel3d> StepFEA_Surface3dElementRepresentation::ModelRef() const
{
  return theModelRef;
}

//=================================================================================================

void StepFEA_Surface3dElementRepresentation::SetModelRef(
  const occ::handle<StepFEA_FeaModel3d>& aModelRef)
{
  theModelRef = aModelRef;
}

//=================================================================================================

occ::handle<StepElement_Surface3dElementDescriptor> StepFEA_Surface3dElementRepresentation::
  ElementDescriptor() const
{
  return theElementDescriptor;
}

//=================================================================================================

void StepFEA_Surface3dElementRepresentation::SetElementDescriptor(
  const occ::handle<StepElement_Surface3dElementDescriptor>& aElementDescriptor)
{
  theElementDescriptor = aElementDescriptor;
}

//=================================================================================================

occ::handle<StepElement_SurfaceElementProperty> StepFEA_Surface3dElementRepresentation::Property()
  const
{
  return theProperty;
}

//=================================================================================================

void StepFEA_Surface3dElementRepresentation::SetProperty(
  const occ::handle<StepElement_SurfaceElementProperty>& aProperty)
{
  theProperty = aProperty;
}

//=================================================================================================

occ::handle<StepElement_ElementMaterial> StepFEA_Surface3dElementRepresentation::Material() const
{
  return theMaterial;
}

//=================================================================================================

void StepFEA_Surface3dElementRepresentation::SetMaterial(
  const occ::handle<StepElement_ElementMaterial>& aMaterial)
{
  theMaterial = aMaterial;
}

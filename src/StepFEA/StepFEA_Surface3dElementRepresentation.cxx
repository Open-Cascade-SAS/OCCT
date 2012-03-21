// Created on: 2002-12-12
// Created by: data exchange team
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <StepFEA_Surface3dElementRepresentation.ixx>

//=======================================================================
//function : StepFEA_Surface3dElementRepresentation
//purpose  : 
//=======================================================================

StepFEA_Surface3dElementRepresentation::StepFEA_Surface3dElementRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_Surface3dElementRepresentation::Init (const Handle(TCollection_HAsciiString) &aRepresentation_Name,
                                                   const Handle(StepRepr_HArray1OfRepresentationItem) &aRepresentation_Items,
                                                   const Handle(StepRepr_RepresentationContext) &aRepresentation_ContextOfItems,
                                                   const Handle(StepFEA_HArray1OfNodeRepresentation) &aElementRepresentation_NodeList,
                                                   const Handle(StepFEA_FeaModel3d) &aModelRef,
                                                   const Handle(StepElement_Surface3dElementDescriptor) &aElementDescriptor,
                                                   const Handle(StepElement_SurfaceElementProperty) &aProperty,
                                                   const Handle(StepElement_ElementMaterial) &aMaterial)
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

//=======================================================================
//function : ModelRef
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel3d) StepFEA_Surface3dElementRepresentation::ModelRef () const
{
  return theModelRef;
}

//=======================================================================
//function : SetModelRef
//purpose  : 
//=======================================================================

void StepFEA_Surface3dElementRepresentation::SetModelRef (const Handle(StepFEA_FeaModel3d) &aModelRef)
{
  theModelRef = aModelRef;
}

//=======================================================================
//function : ElementDescriptor
//purpose  : 
//=======================================================================

Handle(StepElement_Surface3dElementDescriptor) StepFEA_Surface3dElementRepresentation::ElementDescriptor () const
{
  return theElementDescriptor;
}

//=======================================================================
//function : SetElementDescriptor
//purpose  : 
//=======================================================================

void StepFEA_Surface3dElementRepresentation::SetElementDescriptor (const Handle(StepElement_Surface3dElementDescriptor) &aElementDescriptor)
{
  theElementDescriptor = aElementDescriptor;
}

//=======================================================================
//function : Property
//purpose  : 
//=======================================================================

Handle(StepElement_SurfaceElementProperty) StepFEA_Surface3dElementRepresentation::Property () const
{
  return theProperty;
}

//=======================================================================
//function : SetProperty
//purpose  : 
//=======================================================================

void StepFEA_Surface3dElementRepresentation::SetProperty (const Handle(StepElement_SurfaceElementProperty) &aProperty)
{
  theProperty = aProperty;
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================

Handle(StepElement_ElementMaterial) StepFEA_Surface3dElementRepresentation::Material () const
{
  return theMaterial;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void StepFEA_Surface3dElementRepresentation::SetMaterial (const Handle(StepElement_ElementMaterial) &aMaterial)
{
  theMaterial = aMaterial;
}

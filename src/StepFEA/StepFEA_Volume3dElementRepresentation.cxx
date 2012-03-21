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

#include <StepFEA_Volume3dElementRepresentation.ixx>

//=======================================================================
//function : StepFEA_Volume3dElementRepresentation
//purpose  : 
//=======================================================================

StepFEA_Volume3dElementRepresentation::StepFEA_Volume3dElementRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_Volume3dElementRepresentation::Init (const Handle(TCollection_HAsciiString) &aRepresentation_Name,
                                                  const Handle(StepRepr_HArray1OfRepresentationItem) &aRepresentation_Items,
                                                  const Handle(StepRepr_RepresentationContext) &aRepresentation_ContextOfItems,
                                                  const Handle(StepFEA_HArray1OfNodeRepresentation) &aElementRepresentation_NodeList,
                                                  const Handle(StepFEA_FeaModel3d) &aModelRef,
                                                  const Handle(StepElement_Volume3dElementDescriptor) &aElementDescriptor,
                                                  const Handle(StepElement_ElementMaterial) &aMaterial)
{
  StepFEA_ElementRepresentation::Init(aRepresentation_Name,
                                      aRepresentation_Items,
                                      aRepresentation_ContextOfItems,
                                      aElementRepresentation_NodeList);

  theModelRef = aModelRef;

  theElementDescriptor = aElementDescriptor;

  theMaterial = aMaterial;
}

//=======================================================================
//function : ModelRef
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel3d) StepFEA_Volume3dElementRepresentation::ModelRef () const
{
  return theModelRef;
}

//=======================================================================
//function : SetModelRef
//purpose  : 
//=======================================================================

void StepFEA_Volume3dElementRepresentation::SetModelRef (const Handle(StepFEA_FeaModel3d) &aModelRef)
{
  theModelRef = aModelRef;
}

//=======================================================================
//function : ElementDescriptor
//purpose  : 
//=======================================================================

Handle(StepElement_Volume3dElementDescriptor) StepFEA_Volume3dElementRepresentation::ElementDescriptor () const
{
  return theElementDescriptor;
}

//=======================================================================
//function : SetElementDescriptor
//purpose  : 
//=======================================================================

void StepFEA_Volume3dElementRepresentation::SetElementDescriptor (const Handle(StepElement_Volume3dElementDescriptor) &aElementDescriptor)
{
  theElementDescriptor = aElementDescriptor;
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================

Handle(StepElement_ElementMaterial) StepFEA_Volume3dElementRepresentation::Material () const
{
  return theMaterial;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void StepFEA_Volume3dElementRepresentation::SetMaterial (const Handle(StepElement_ElementMaterial) &aMaterial)
{
  theMaterial = aMaterial;
}

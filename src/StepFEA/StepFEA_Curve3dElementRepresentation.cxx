// File:	StepFEA_Curve3dElementRepresentation.cxx
// Created:	Thu Dec 12 17:51:03 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_Curve3dElementRepresentation.ixx>

//=======================================================================
//function : StepFEA_Curve3dElementRepresentation
//purpose  : 
//=======================================================================

StepFEA_Curve3dElementRepresentation::StepFEA_Curve3dElementRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementRepresentation::Init (const Handle(TCollection_HAsciiString) &aRepresentation_Name,
                                                 const Handle(StepRepr_HArray1OfRepresentationItem) &aRepresentation_Items,
                                                 const Handle(StepRepr_RepresentationContext) &aRepresentation_ContextOfItems,
                                                 const Handle(StepFEA_HArray1OfNodeRepresentation) &aElementRepresentation_NodeList,
                                                 const Handle(StepFEA_FeaModel3d) &aModelRef,
                                                 const Handle(StepElement_Curve3dElementDescriptor) &aElementDescriptor,
                                                 const Handle(StepFEA_Curve3dElementProperty) &aProperty,
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

Handle(StepFEA_FeaModel3d) StepFEA_Curve3dElementRepresentation::ModelRef () const
{
  return theModelRef;
}

//=======================================================================
//function : SetModelRef
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementRepresentation::SetModelRef (const Handle(StepFEA_FeaModel3d) &aModelRef)
{
  theModelRef = aModelRef;
}

//=======================================================================
//function : ElementDescriptor
//purpose  : 
//=======================================================================

Handle(StepElement_Curve3dElementDescriptor) StepFEA_Curve3dElementRepresentation::ElementDescriptor () const
{
  return theElementDescriptor;
}

//=======================================================================
//function : SetElementDescriptor
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementRepresentation::SetElementDescriptor (const Handle(StepElement_Curve3dElementDescriptor) &aElementDescriptor)
{
  theElementDescriptor = aElementDescriptor;
}

//=======================================================================
//function : Property
//purpose  : 
//=======================================================================

Handle(StepFEA_Curve3dElementProperty) StepFEA_Curve3dElementRepresentation::Property () const
{
  return theProperty;
}

//=======================================================================
//function : SetProperty
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementRepresentation::SetProperty (const Handle(StepFEA_Curve3dElementProperty) &aProperty)
{
  theProperty = aProperty;
}

//=======================================================================
//function : Material
//purpose  : 
//=======================================================================

Handle(StepElement_ElementMaterial) StepFEA_Curve3dElementRepresentation::Material () const
{
  return theMaterial;
}

//=======================================================================
//function : SetMaterial
//purpose  : 
//=======================================================================

void StepFEA_Curve3dElementRepresentation::SetMaterial (const Handle(StepElement_ElementMaterial) &aMaterial)
{
  theMaterial = aMaterial;
}

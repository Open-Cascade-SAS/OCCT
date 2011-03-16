// File:	StepFEA_Volume3dElementRepresentation.cxx
// Created:	Thu Dec 12 17:51:08 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

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

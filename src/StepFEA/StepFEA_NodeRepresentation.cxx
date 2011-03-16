// File:	StepFEA_NodeRepresentation.cxx
// Created:	Thu Dec 12 17:51:06 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_NodeRepresentation.ixx>

//=======================================================================
//function : StepFEA_NodeRepresentation
//purpose  : 
//=======================================================================

StepFEA_NodeRepresentation::StepFEA_NodeRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_NodeRepresentation::Init (const Handle(TCollection_HAsciiString) &aRepresentation_Name,
                                       const Handle(StepRepr_HArray1OfRepresentationItem) &aRepresentation_Items,
                                       const Handle(StepRepr_RepresentationContext) &aRepresentation_ContextOfItems,
                                       const Handle(StepFEA_FeaModel) &aModelRef)
{
  StepRepr_Representation::Init(aRepresentation_Name,
                                aRepresentation_Items,
                                aRepresentation_ContextOfItems);

  theModelRef = aModelRef;
}

//=======================================================================
//function : ModelRef
//purpose  : 
//=======================================================================

Handle(StepFEA_FeaModel) StepFEA_NodeRepresentation::ModelRef () const
{
  return theModelRef;
}

//=======================================================================
//function : SetModelRef
//purpose  : 
//=======================================================================

void StepFEA_NodeRepresentation::SetModelRef (const Handle(StepFEA_FeaModel) &aModelRef)
{
  theModelRef = aModelRef;
}

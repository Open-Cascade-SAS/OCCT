// File:	StepFEA_ElementRepresentation.cxx
// Created:	Thu Dec 12 17:51:05 2002 
// Author:	data exchange team
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2
// Copyright:	Open CASCADE 2002

#include <StepFEA_ElementRepresentation.ixx>

//=======================================================================
//function : StepFEA_ElementRepresentation
//purpose  : 
//=======================================================================

StepFEA_ElementRepresentation::StepFEA_ElementRepresentation ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ElementRepresentation::Init (const Handle(TCollection_HAsciiString) &aRepresentation_Name,
                                          const Handle(StepRepr_HArray1OfRepresentationItem) &aRepresentation_Items,
                                          const Handle(StepRepr_RepresentationContext) &aRepresentation_ContextOfItems,
                                          const Handle(StepFEA_HArray1OfNodeRepresentation) &aNodeList)
{
  StepRepr_Representation::Init(aRepresentation_Name,
                                aRepresentation_Items,
                                aRepresentation_ContextOfItems);

  theNodeList = aNodeList;
}

//=======================================================================
//function : NodeList
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfNodeRepresentation) StepFEA_ElementRepresentation::NodeList () const
{
  return theNodeList;
}

//=======================================================================
//function : SetNodeList
//purpose  : 
//=======================================================================

void StepFEA_ElementRepresentation::SetNodeList (const Handle(StepFEA_HArray1OfNodeRepresentation) &aNodeList)
{
  theNodeList = aNodeList;
}

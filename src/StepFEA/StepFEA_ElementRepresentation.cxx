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

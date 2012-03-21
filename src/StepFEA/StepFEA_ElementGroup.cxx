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

#include <StepFEA_ElementGroup.ixx>

//=======================================================================
//function : StepFEA_ElementGroup
//purpose  : 
//=======================================================================

StepFEA_ElementGroup::StepFEA_ElementGroup ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepFEA_ElementGroup::Init (const Handle(TCollection_HAsciiString) &aGroup_Name,
                                 const Handle(TCollection_HAsciiString) &aGroup_Description,
                                 const Handle(StepFEA_FeaModel) &aFeaGroup_ModelRef,
                                 const Handle(StepFEA_HArray1OfElementRepresentation) &aElements)
{
  StepFEA_FeaGroup::Init(aGroup_Name,
                         aGroup_Description,
                         aFeaGroup_ModelRef);

  theElements = aElements;
}

//=======================================================================
//function : Elements
//purpose  : 
//=======================================================================

Handle(StepFEA_HArray1OfElementRepresentation) StepFEA_ElementGroup::Elements () const
{
  return theElements;
}

//=======================================================================
//function : SetElements
//purpose  : 
//=======================================================================

void StepFEA_ElementGroup::SetElements (const Handle(StepFEA_HArray1OfElementRepresentation) &aElements)
{
  theElements = aElements;
}

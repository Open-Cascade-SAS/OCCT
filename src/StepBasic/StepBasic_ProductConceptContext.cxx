// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <StepBasic_ProductConceptContext.ixx>

//=======================================================================
//function : StepBasic_ProductConceptContext
//purpose  : 
//=======================================================================

StepBasic_ProductConceptContext::StepBasic_ProductConceptContext ()
{
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepBasic_ProductConceptContext::Init (const Handle(TCollection_HAsciiString) &aApplicationContextElement_Name,
                                            const Handle(StepBasic_ApplicationContext) &aApplicationContextElement_FrameOfReference,
                                            const Handle(TCollection_HAsciiString) &aMarketSegmentType)
{
  StepBasic_ApplicationContextElement::Init(aApplicationContextElement_Name,
                                            aApplicationContextElement_FrameOfReference);

  theMarketSegmentType = aMarketSegmentType;
}

//=======================================================================
//function : MarketSegmentType
//purpose  : 
//=======================================================================

Handle(TCollection_HAsciiString) StepBasic_ProductConceptContext::MarketSegmentType () const
{
  return theMarketSegmentType;
}

//=======================================================================
//function : SetMarketSegmentType
//purpose  : 
//=======================================================================

void StepBasic_ProductConceptContext::SetMarketSegmentType (const Handle(TCollection_HAsciiString) &aMarketSegmentType)
{
  theMarketSegmentType = aMarketSegmentType;
}

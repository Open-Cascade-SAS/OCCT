// File:	StepBasic_ProductConceptContext.cxx
// Created:	Fri Nov 26 16:26:39 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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

// Created on: 1999-11-26
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0

#include <Standard_Type.hxx>
#include <StepBasic_ApplicationContext.hxx>
#include <StepBasic_ProductConceptContext.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepBasic_ProductConceptContext,StepBasic_ApplicationContextElement)

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

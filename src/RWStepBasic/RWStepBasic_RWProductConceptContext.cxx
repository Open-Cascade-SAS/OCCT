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

#include <RWStepBasic_RWProductConceptContext.ixx>

//=======================================================================
//function : RWStepBasic_RWProductConceptContext
//purpose  : 
//=======================================================================

RWStepBasic_RWProductConceptContext::RWStepBasic_RWProductConceptContext ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductConceptContext::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                    const Standard_Integer num,
                                                    Handle(Interface_Check)& ach,
                                                    const Handle(StepBasic_ProductConceptContext) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"product_concept_context") ) return;

  // Inherited fields of ApplicationContextElement

  Handle(TCollection_HAsciiString) aApplicationContextElement_Name;
  data->ReadString (num, 1, "application_context_element.name", ach, aApplicationContextElement_Name);

  Handle(StepBasic_ApplicationContext) aApplicationContextElement_FrameOfReference;
  data->ReadEntity (num, 2, "application_context_element.frame_of_reference", ach, STANDARD_TYPE(StepBasic_ApplicationContext), aApplicationContextElement_FrameOfReference);

  // Own fields of ProductConceptContext

  Handle(TCollection_HAsciiString) aMarketSegmentType;
  data->ReadString (num, 3, "market_segment_type", ach, aMarketSegmentType);

  // Initialize entity
  ent->Init(aApplicationContextElement_Name,
            aApplicationContextElement_FrameOfReference,
            aMarketSegmentType);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductConceptContext::WriteStep (StepData_StepWriter& SW,
                                                     const Handle(StepBasic_ProductConceptContext) &ent) const
{

  // Inherited fields of ApplicationContextElement

  SW.Send (ent->StepBasic_ApplicationContextElement::Name());

  SW.Send (ent->StepBasic_ApplicationContextElement::FrameOfReference());

  // Own fields of ProductConceptContext

  SW.Send (ent->MarketSegmentType());
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepBasic_RWProductConceptContext::Share (const Handle(StepBasic_ProductConceptContext) &ent,
                                                 Interface_EntityIterator& iter) const
{

  // Inherited fields of ApplicationContextElement

  iter.AddItem (ent->StepBasic_ApplicationContextElement::FrameOfReference());

  // Own fields of ProductConceptContext
}

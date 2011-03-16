// File:	RWStepBasic_RWProductConceptContext.cxx
// Created:	Fri Nov 26 16:26:39 1999 
// Author:	Andrey BETENEV
// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.0
// Copyright:	Matra Datavision 1999

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

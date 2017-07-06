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


#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepVisual_RWContextDependentOverRidingStyledItem.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepVisual_ContextDependentOverRidingStyledItem.hxx>
#include <StepVisual_HArray1OfPresentationStyleAssignment.hxx>
#include <StepVisual_HArray1OfStyleContextSelect.hxx>
#include <StepVisual_PresentationStyleAssignment.hxx>
#include <StepVisual_StyleContextSelect.hxx>
#include <StepVisual_StyledItem.hxx>

RWStepVisual_RWContextDependentOverRidingStyledItem::RWStepVisual_RWContextDependentOverRidingStyledItem () {}

void RWStepVisual_RWContextDependentOverRidingStyledItem::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_ContextDependentOverRidingStyledItem)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,5,ach,"context_dependent_over_riding_styled_item")) return;

	// --- inherited field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- inherited field : styles ---

	Handle(StepVisual_HArray1OfPresentationStyleAssignment) aStyles;
	Handle(StepVisual_PresentationStyleAssignment) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"styles",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aStyles = new StepVisual_HArray1OfPresentationStyleAssignment (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"presentation_style_assignment", ach,
				  STANDARD_TYPE(StepVisual_PresentationStyleAssignment), anent2))
	      aStyles->SetValue(i2, anent2);
	  }
	}

	// --- inherited field : item ---

  Handle(Standard_Transient) aItem;
  data->ReadEntity(num, 3,"item", ach, STANDARD_TYPE(Standard_Transient), aItem);

	// --- inherited field : overRiddenStyle ---

	Handle(StepVisual_StyledItem) aOverRiddenStyle;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
	data->ReadEntity(num, 4,"over_ridden_style", ach, STANDARD_TYPE(StepVisual_StyledItem), aOverRiddenStyle);

	// --- own field : styleContext ---

	Handle(StepVisual_HArray1OfStyleContextSelect) aStyleContext;
	StepVisual_StyleContextSelect aStyleContextItem;
	Standard_Integer nsub5;
	if (data->ReadSubList (num,5,"style_context",ach,nsub5)) {
	  Standard_Integer nb5 = data->NbParams(nsub5);
	  aStyleContext = new StepVisual_HArray1OfStyleContextSelect (1, nb5);
	  for (Standard_Integer i5 = 1; i5 <= nb5; i5 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
	    if (data->ReadEntity (nsub5,i5,"style_context",ach,aStyleContextItem))
	      aStyleContext->SetValue(i5,aStyleContextItem);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aStyles, aItem, aOverRiddenStyle, aStyleContext);
}


void RWStepVisual_RWContextDependentOverRidingStyledItem::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_ContextDependentOverRidingStyledItem)& ent) const
{

	// --- inherited field name ---

	SW.Send(ent->Name());

	// --- inherited field styles ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbStyles();  i2 ++) {
	  SW.Send(ent->StylesValue(i2));
	}
	SW.CloseSub();

	// --- inherited field item ---

	SW.Send(ent->Item());

	// --- inherited field overRiddenStyle ---

	SW.Send(ent->OverRiddenStyle());

	// --- own field : styleContext ---

	SW.OpenSub();
	for (Standard_Integer i5 = 1;  i5 <= ent->NbStyleContext();  i5 ++) {
	  SW.Send(ent->StyleContextValue(i5).Value());
	}
	SW.CloseSub();
}


void RWStepVisual_RWContextDependentOverRidingStyledItem::Share(const Handle(StepVisual_ContextDependentOverRidingStyledItem)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbStyles();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->StylesValue(is1));
	}



	iter.GetOneItem(ent->Item());


	iter.GetOneItem(ent->OverRiddenStyle());


	Standard_Integer nbElem4 = ent->NbStyleContext();
	for (Standard_Integer is4=1; is4<=nbElem4; is4 ++) {
	  iter.GetOneItem(ent->StyleContextValue(is4).Value());
	}

}


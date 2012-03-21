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


#include <RWStepVisual_RWPresentationStyleByContext.ixx>
#include <StepVisual_StyleContextSelect.hxx>
#include <StepVisual_HArray1OfPresentationStyleSelect.hxx>
#include <StepVisual_PresentationStyleSelect.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_PresentationStyleByContext.hxx>


RWStepVisual_RWPresentationStyleByContext::RWStepVisual_RWPresentationStyleByContext () {}

void RWStepVisual_RWPresentationStyleByContext::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_PresentationStyleByContext)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"presentation_style_by_context")) return;

	// --- inherited field : styles ---

	Handle(StepVisual_HArray1OfPresentationStyleSelect) aStyles;
	StepVisual_PresentationStyleSelect aStylesItem;
	Standard_Integer nsub1;
	if (data->ReadSubList (num,1,"styles",ach,nsub1)) {
	  Standard_Integer nb1 = data->NbParams(nsub1);
	  aStyles = new StepVisual_HArray1OfPresentationStyleSelect (1, nb1);
	  for (Standard_Integer i1 = 1; i1 <= nb1; i1 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	    if (data->ReadEntity (nsub1,i1,"styles",ach,aStylesItem))
	      aStyles->SetValue(i1,aStylesItem);
	  }
	}

	// --- own field : styleContext ---

	StepVisual_StyleContextSelect aStyleContext;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	data->ReadEntity(num,2,"style_context",ach,aStyleContext);

	//--- Initialisation of the read entity ---


	ent->Init(aStyles, aStyleContext);
}


void RWStepVisual_RWPresentationStyleByContext::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_PresentationStyleByContext)& ent) const
{

	// --- inherited field styles ---

	SW.OpenSub();
	for (Standard_Integer i1 = 1;  i1 <= ent->NbStyles();  i1 ++) {
	  SW.Send(ent->StylesValue(i1).Value());
	}
	SW.CloseSub();

	// --- own field : styleContext ---

	SW.Send(ent->StyleContext().Value());
}


void RWStepVisual_RWPresentationStyleByContext::Share(const Handle(StepVisual_PresentationStyleByContext)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbStyles();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->StylesValue(is1).Value());
	}



	iter.GetOneItem(ent->StyleContext().Value());
}


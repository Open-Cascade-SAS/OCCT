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
#include <RWStepVisual_RWPresentationStyleByContext.hxx>
#include <StepData_SelectMember.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_HArray1OfPresentationStyleSelect.hxx>
#include <StepVisual_NullStyleMember.hxx>
#include <StepVisual_PresentationStyleByContext.hxx>
#include <StepVisual_PresentationStyleSelect.hxx>
#include <StepVisual_StyleContextSelect.hxx>

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
      Interface_ParamType aType = data->ParamType(nsub1, i1);
      if (aType == Interface_ParamIdent) {
        data->ReadEntity (nsub1,i1,"styles",ach,aStylesItem);
      }
      else {
        Handle(StepData_SelectMember) aMember;
        data->ReadMember(nsub1, i1, "null_style", ach, aMember);
        Standard_CString anEnumText = aMember->EnumText();
        Handle(StepVisual_NullStyleMember) aNullStyle = new StepVisual_NullStyleMember();
        aNullStyle->SetEnumText(0, anEnumText);
        aStylesItem.SetValue(aNullStyle);
      }
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
    StepVisual_PresentationStyleSelect aStyle = ent->StylesValue(i1);
    if (aStyle.Value()->IsKind(STANDARD_TYPE(StepVisual_NullStyleMember))) {
      SW.OpenTypedSub("NULL_STYLE");
      SW.SendEnum(".NULL.");
      SW.CloseSub();
    }
    else
      SW.Send(aStyle.Value());
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


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
#include <RWStepVisual_RWCurveStyleFont.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CurveStyleFont.hxx>
#include <StepVisual_CurveStyleFontPattern.hxx>
#include <StepVisual_HArray1OfCurveStyleFontPattern.hxx>

RWStepVisual_RWCurveStyleFont::RWStepVisual_RWCurveStyleFont () {}

void RWStepVisual_RWCurveStyleFont::ReadStep
	(const Handle(StepData_StepReaderData)& data,
	 const Standard_Integer num,
	 Handle(Interface_Check)& ach,
	 const Handle(StepVisual_CurveStyleFont)& ent) const
{


	// --- Number of Parameter Control ---

	if (!data->CheckNbParams(num,2,ach,"curve_style_font")) return;

	// --- own field : name ---

	Handle(TCollection_HAsciiString) aName;
	//szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
	data->ReadString (num,1,"name",ach,aName);

	// --- own field : patternList ---

	Handle(StepVisual_HArray1OfCurveStyleFontPattern) aPatternList;
	Handle(StepVisual_CurveStyleFontPattern) anent2;
	Standard_Integer nsub2;
	if (data->ReadSubList (num,2,"pattern_list",ach,nsub2)) {
	  Standard_Integer nb2 = data->NbParams(nsub2);
	  aPatternList = new StepVisual_HArray1OfCurveStyleFontPattern (1, nb2);
	  for (Standard_Integer i2 = 1; i2 <= nb2; i2 ++) {
	    //szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
	    if (data->ReadEntity (nsub2, i2,"curve_style_font_pattern", ach,
				  STANDARD_TYPE(StepVisual_CurveStyleFontPattern), anent2))
	      aPatternList->SetValue(i2, anent2);
	  }
	}

	//--- Initialisation of the read entity ---


	ent->Init(aName, aPatternList);
}


void RWStepVisual_RWCurveStyleFont::WriteStep
	(StepData_StepWriter& SW,
	 const Handle(StepVisual_CurveStyleFont)& ent) const
{

	// --- own field : name ---

	SW.Send(ent->Name());

	// --- own field : patternList ---

	SW.OpenSub();
	for (Standard_Integer i2 = 1;  i2 <= ent->NbPatternList();  i2 ++) {
	  SW.Send(ent->PatternListValue(i2));
	}
	SW.CloseSub();
}


void RWStepVisual_RWCurveStyleFont::Share(const Handle(StepVisual_CurveStyleFont)& ent, Interface_EntityIterator& iter) const
{

	Standard_Integer nbElem1 = ent->NbPatternList();
	for (Standard_Integer is1=1; is1<=nbElem1; is1 ++) {
	  iter.GetOneItem(ent->PatternListValue(is1));
	}

}



#include <RWStepVisual_RWCurveStyleFont.ixx>
#include <StepVisual_HArray1OfCurveStyleFontPattern.hxx>
#include <StepVisual_CurveStyleFontPattern.hxx>


#include <Interface_EntityIterator.hxx>


#include <StepVisual_CurveStyleFont.hxx>


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


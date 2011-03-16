// File:	StepRepr_MeasureRepresentationItem.cxx
// Created:	Wed Sep  8 11:16:06 1999
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

#include <StepRepr_MeasureRepresentationItem.ixx>

//=======================================================================
//function : StepRepr_MeasureRepresentationItem
//purpose  : 
//=======================================================================

StepRepr_MeasureRepresentationItem::StepRepr_MeasureRepresentationItem() 
{
  myMeasure = new StepBasic_MeasureWithUnit;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_MeasureRepresentationItem::Init (const Handle(TCollection_HAsciiString) &aName,
					       const Handle(StepBasic_MeasureValueMember) &aValueComponent,
					       const StepBasic_Unit &aUnitComponent)
{
  StepRepr_RepresentationItem::Init ( aName );
  myMeasure->Init ( aValueComponent, aUnitComponent );
}

//=======================================================================
//function : SetMeasureUnit
//purpose  : 
//=======================================================================

void StepRepr_MeasureRepresentationItem::SetMeasure (const Handle(StepBasic_MeasureWithUnit) &Measure)
{
  myMeasure = Measure;
}

//=======================================================================
//function : Measure
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureWithUnit) StepRepr_MeasureRepresentationItem::Measure () const
{
  return myMeasure;
}

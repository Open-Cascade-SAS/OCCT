#include <StepRepr_ReprItemAndLengthMeasureWithUnit.ixx>


//=======================================================================
//function : StepRepr_ReprItemAndLengthMeasureWithUnit
//purpose  : 
//=======================================================================

StepRepr_ReprItemAndLengthMeasureWithUnit::StepRepr_ReprItemAndLengthMeasureWithUnit()
{
  myLengthMeasureWithUnit = new StepBasic_LengthMeasureWithUnit();
  myMeasureWithUnit = new StepBasic_MeasureWithUnit();
  myMeasureRepresentationItem = new StepRepr_MeasureRepresentationItem();
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndLengthMeasureWithUnit::Init
  (const Handle(StepBasic_MeasureWithUnit)& aMWU,
   const Handle(StepRepr_RepresentationItem)& aRI) 
{
  myMeasureWithUnit = aMWU;
  SetName(aRI->Name());
}


//=======================================================================
//function : SetLengthMeasureWithUnit
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndLengthMeasureWithUnit::SetLengthMeasureWithUnit
  (const Handle(StepBasic_LengthMeasureWithUnit)& aLMWU) 
{
  myLengthMeasureWithUnit = aLMWU;
}


//=======================================================================
//function : GetLengthMeasureWithUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_LengthMeasureWithUnit) StepRepr_ReprItemAndLengthMeasureWithUnit::
       GetLengthMeasureWithUnit() const
{
  return myLengthMeasureWithUnit;
}


//=======================================================================
//function : GetMeasureRepresentationItem
//purpose  : 
//=======================================================================

Handle(StepRepr_MeasureRepresentationItem) StepRepr_ReprItemAndLengthMeasureWithUnit::
       GetMeasureRepresentationItem() const
{
  return myMeasureRepresentationItem;
}


//=======================================================================
//function : SetMeasureWithUnit
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndLengthMeasureWithUnit::SetMeasureWithUnit
  (const Handle(StepBasic_MeasureWithUnit)& aMWU) 
{
  myMeasureWithUnit = aMWU;
}


//=======================================================================
//function : GetMeasureWithUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureWithUnit) StepRepr_ReprItemAndLengthMeasureWithUnit::
       GetMeasureWithUnit() const
{
  return myMeasureWithUnit;
}


//=======================================================================
//function : GetRepresentationItem
//purpose  : 
//=======================================================================

Handle(StepRepr_RepresentationItem) StepRepr_ReprItemAndLengthMeasureWithUnit::
       GetRepresentationItem() const
{
  Handle(StepRepr_RepresentationItem) RI = new StepRepr_RepresentationItem();
  RI->Init(Name());
  return RI;
}



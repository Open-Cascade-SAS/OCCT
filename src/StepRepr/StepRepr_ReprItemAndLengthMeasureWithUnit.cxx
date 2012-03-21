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



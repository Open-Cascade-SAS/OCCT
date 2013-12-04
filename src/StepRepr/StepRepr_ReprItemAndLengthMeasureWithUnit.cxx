// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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



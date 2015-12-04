// Created on: 2015-07-22
// Created by: Irina KRYLOVA
// Copyright (c) 2015 OPEN CASCADE SAS
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


#include <Standard_Type.hxx>
#include <StepBasic_MeasureWithUnit.hxx>
#include <StepRepr_MeasureRepresentationItem.hxx>
#include <StepRepr_RepresentationItem.hxx>
#include <StepRepr_ReprItemAndMeasureWithUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ReprItemAndMeasureWithUnit,StepRepr_RepresentationItem)

//=======================================================================
//function : StepRepr_ReprItemAndMeasureWithUnit
//purpose  : 
//=======================================================================
StepRepr_ReprItemAndMeasureWithUnit::StepRepr_ReprItemAndMeasureWithUnit()
{
  myMeasureWithUnit = new StepBasic_MeasureWithUnit();
  myMeasureRepresentationItem = new StepRepr_MeasureRepresentationItem();
}


//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndMeasureWithUnit::Init
  (const Handle(StepBasic_MeasureWithUnit)& aMWU,
   const Handle(StepRepr_RepresentationItem)& aRI) 
{
  myMeasureWithUnit = aMWU;
  SetName(aRI->Name());
}

//=======================================================================
//function : GetMeasureRepresentationItem
//purpose  : 
//=======================================================================

Handle(StepRepr_MeasureRepresentationItem) StepRepr_ReprItemAndMeasureWithUnit::
       GetMeasureRepresentationItem() const
{
  return myMeasureRepresentationItem;
}


//=======================================================================
//function : SetMeasureWithUnit
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndMeasureWithUnit::SetMeasureWithUnit
  (const Handle(StepBasic_MeasureWithUnit)& aMWU) 
{
  myMeasureWithUnit = aMWU;
}


//=======================================================================
//function : GetMeasureWithUnit
//purpose  : 
//=======================================================================

Handle(StepBasic_MeasureWithUnit) StepRepr_ReprItemAndMeasureWithUnit::
       GetMeasureWithUnit() const
{
  return myMeasureWithUnit;
}


//=======================================================================
//function : GetRepresentationItem
//purpose  : 
//=======================================================================

Handle(StepRepr_RepresentationItem) StepRepr_ReprItemAndMeasureWithUnit::
       GetRepresentationItem() const
{
  Handle(StepRepr_RepresentationItem) RI = new StepRepr_RepresentationItem();
  RI->Init(Name());
  return RI;
}

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
#include <StepShape_QualifiedRepresentationItem.hxx>
#include <StepRepr_ReprItemAndMeasureWithUnitAndQRI.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StepRepr_ReprItemAndMeasureWithUnitAndQRI,StepRepr_ReprItemAndMeasureWithUnit)

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndMeasureWithUnitAndQRI::Init (const Handle(StepBasic_MeasureWithUnit)& aMWU, 
                                                      const Handle(StepRepr_RepresentationItem)& aRI, 
                                                      const Handle(StepShape_QualifiedRepresentationItem) aQRI)
{
  StepRepr_ReprItemAndMeasureWithUnit::Init(aMWU, aRI);
  myQualifiedRepresentationItem = aQRI;
}

//=======================================================================
//function : StepRepr_ReprItemAndMeasureWithUnitAndQRI
//purpose  : 
//=======================================================================
StepRepr_ReprItemAndMeasureWithUnitAndQRI::StepRepr_ReprItemAndMeasureWithUnitAndQRI() : StepRepr_ReprItemAndMeasureWithUnit()
{
  myQualifiedRepresentationItem = new StepShape_QualifiedRepresentationItem();
}

//=======================================================================
//function : SetQualifiedRepresentationItem
//purpose  : 
//=======================================================================

void StepRepr_ReprItemAndMeasureWithUnitAndQRI::SetQualifiedRepresentationItem
  (const Handle(StepShape_QualifiedRepresentationItem)& aQRI) 
{
  myQualifiedRepresentationItem = aQRI;
}


//=======================================================================
//function : GetPlaneAngleMeasureWithUnit
//purpose  : 
//=======================================================================

Handle(StepShape_QualifiedRepresentationItem) StepRepr_ReprItemAndMeasureWithUnitAndQRI::
       GetQualifiedRepresentationItem() const
{
  return myQualifiedRepresentationItem;
}

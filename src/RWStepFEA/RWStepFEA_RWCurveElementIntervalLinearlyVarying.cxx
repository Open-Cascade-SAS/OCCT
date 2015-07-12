// Created on: 2003-01-22
// Created by: data exchange team
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

// Generator:	ExpToCas (EXPRESS -> CASCADE/XSTEP Translator) V1.2

#include <Interface_Check.hxx>
#include <Interface_EntityIterator.hxx>
#include <RWStepFEA_RWCurveElementIntervalLinearlyVarying.hxx>
#include <StepBasic_EulerAngles.hxx>
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepElement_CurveElementSectionDefinition.hxx>
#include <StepElement_HArray1OfCurveElementSectionDefinition.hxx>
#include <StepFEA_CurveElementIntervalLinearlyVarying.hxx>
#include <StepFEA_CurveElementLocation.hxx>

//=======================================================================
//function : RWStepFEA_RWCurveElementIntervalLinearlyVarying
//purpose  : 
//=======================================================================
RWStepFEA_RWCurveElementIntervalLinearlyVarying::RWStepFEA_RWCurveElementIntervalLinearlyVarying ()
{
}

//=======================================================================
//function : ReadStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementIntervalLinearlyVarying::ReadStep (const Handle(StepData_StepReaderData)& data,
                                                                const Standard_Integer num,
                                                                Handle(Interface_Check)& ach,
                                                                const Handle(StepFEA_CurveElementIntervalLinearlyVarying) &ent) const
{
  // Check number of parameters
  if ( ! data->CheckNbParams(num,3,ach,"curve_element_interval_linearly_varying") ) return;

  // Inherited fields of CurveElementInterval

  Handle(StepFEA_CurveElementLocation) aCurveElementInterval_FinishPosition;
  data->ReadEntity (num, 1, "curve_element_interval.finish_position", ach, STANDARD_TYPE(StepFEA_CurveElementLocation), aCurveElementInterval_FinishPosition);

  Handle(StepBasic_EulerAngles) aCurveElementInterval_EuAngles;
  data->ReadEntity (num, 2, "curve_element_interval.eu_angles", ach, STANDARD_TYPE(StepBasic_EulerAngles), aCurveElementInterval_EuAngles);

  // Own fields of CurveElementIntervalLinearlyVarying

  Handle(StepElement_HArray1OfCurveElementSectionDefinition) aSections;
  Standard_Integer sub3 = 0;
  if ( data->ReadSubList (num, 3, "sections", ach, sub3) ) {
    Standard_Integer nb0 = data->NbParams(sub3);
    aSections = new StepElement_HArray1OfCurveElementSectionDefinition (1, nb0);
    Standard_Integer num2 = sub3;
    for ( Standard_Integer i0=1; i0 <= nb0; i0++ ) {
      Handle(StepElement_CurveElementSectionDefinition) anIt0;
      data->ReadEntity (num2, i0, "curve_element_section_definition", ach, STANDARD_TYPE(StepElement_CurveElementSectionDefinition), anIt0);
      aSections->SetValue(i0, anIt0);
    }
  }

  // Initialize entity
  ent->Init(aCurveElementInterval_FinishPosition,
            aCurveElementInterval_EuAngles,
            aSections);
}

//=======================================================================
//function : WriteStep
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementIntervalLinearlyVarying::WriteStep (StepData_StepWriter& SW,
                                                                 const Handle(StepFEA_CurveElementIntervalLinearlyVarying) &ent) const
{

  // Inherited fields of CurveElementInterval

  SW.Send (ent->StepFEA_CurveElementInterval::FinishPosition());

  SW.Send (ent->StepFEA_CurveElementInterval::EuAngles());

  // Own fields of CurveElementIntervalLinearlyVarying

  SW.OpenSub();
  for (Standard_Integer i2=1; i2 <= ent->Sections()->Length(); i2++ ) {
    Handle(StepElement_CurveElementSectionDefinition) Var0 = ent->Sections()->Value(i2);
    SW.Send (Var0);
  }
  SW.CloseSub();
}

//=======================================================================
//function : Share
//purpose  : 
//=======================================================================

void RWStepFEA_RWCurveElementIntervalLinearlyVarying::Share (const Handle(StepFEA_CurveElementIntervalLinearlyVarying) &ent,
                                                             Interface_EntityIterator& iter) const
{

  // Inherited fields of CurveElementInterval

  iter.AddItem (ent->StepFEA_CurveElementInterval::FinishPosition());

  iter.AddItem (ent->StepFEA_CurveElementInterval::EuAngles());

  // Own fields of CurveElementIntervalLinearlyVarying

  for (Standard_Integer i3=1; i3 <= ent->Sections()->Length(); i3++ ) {
    Handle(StepElement_CurveElementSectionDefinition) Var0 = ent->Sections()->Value(i3);
    iter.AddItem (Var0);
  }
}

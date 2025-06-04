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

#include <Interface_EntityIterator.hxx>
#include "RWStepGeom_RWReparametrisedCompositeCurveSegment.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_Curve.hxx>
#include <StepGeom_ReparametrisedCompositeCurveSegment.hxx>
#include <StepGeom_TransitionCode.hxx>

#include "RWStepGeom_RWTransitionCode.pxx"

RWStepGeom_RWReparametrisedCompositeCurveSegment::RWStepGeom_RWReparametrisedCompositeCurveSegment()
{
}

void RWStepGeom_RWReparametrisedCompositeCurveSegment::ReadStep(
  const Handle(StepData_StepReaderData)&                      data,
  const Standard_Integer                                      num,
  Handle(Interface_Check)&                                    ach,
  const Handle(StepGeom_ReparametrisedCompositeCurveSegment)& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 4, ach, "reparametrised_composite_curve_segment"))
    return;

  // --- inherited field : transition ---

  StepGeom_TransitionCode aTransition = StepGeom_tcDiscontinuous;
  if (data->ParamType(num, 1) == Interface_ParamEnum)
  {
    Standard_CString text = data->ParamCValue(num, 1);
    if (!RWStepGeom_RWTransitionCode::ConvertToEnum(text, aTransition))
    {
      ach->AddFail("Enumeration transition_code has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #1 (transition) is not an enumeration");

  // --- inherited field : sameSense ---

  Standard_Boolean aSameSense;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat2 =` not needed
  data->ReadBoolean(num, 2, "same_sense", ach, aSameSense);

  // --- inherited field : parentCurve ---

  Handle(StepGeom_Curve) aParentCurve;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat3 =` not needed
  data->ReadEntity(num, 3, "parent_curve", ach, STANDARD_TYPE(StepGeom_Curve), aParentCurve);

  // --- own field : paramLength ---

  Standard_Real aParamLength;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
  data->ReadReal(num, 4, "param_length", ach, aParamLength);

  //--- Initialisation of the read entity ---

  ent->Init(aTransition, aSameSense, aParentCurve, aParamLength);
}

void RWStepGeom_RWReparametrisedCompositeCurveSegment::WriteStep(
  StepData_StepWriter&                                        SW,
  const Handle(StepGeom_ReparametrisedCompositeCurveSegment)& ent) const
{

  // --- inherited field transition ---

  SW.SendEnum(RWStepGeom_RWTransitionCode::ConvertToString(ent->Transition()));

  // --- inherited field sameSense ---

  SW.SendBoolean(ent->SameSense());

  // --- inherited field parentCurve ---

  SW.Send(ent->ParentCurve());

  // --- own field : paramLength ---

  SW.Send(ent->ParamLength());
}

void RWStepGeom_RWReparametrisedCompositeCurveSegment::Share(
  const Handle(StepGeom_ReparametrisedCompositeCurveSegment)& ent,
  Interface_EntityIterator&                                   iter) const
{

  iter.GetOneItem(ent->ParentCurve());
}

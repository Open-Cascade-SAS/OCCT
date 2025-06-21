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
#include "RWStepGeom_RWSurfacePatch.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_BoundedSurface.hxx>
#include <StepGeom_SurfacePatch.hxx>
#include <StepGeom_TransitionCode.hxx>
#include <TCollection_AsciiString.hxx>

#include "RWStepGeom_RWTransitionCode.pxx"

RWStepGeom_RWSurfacePatch::RWStepGeom_RWSurfacePatch() {}

void RWStepGeom_RWSurfacePatch::ReadStep(const Handle(StepData_StepReaderData)& data,
                                         const Standard_Integer                 num,
                                         Handle(Interface_Check)&               ach,
                                         const Handle(StepGeom_SurfacePatch)&   ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 5, ach, "surface_patch"))
    return;

  // --- own field : parentSurface ---

  Handle(StepGeom_BoundedSurface) aParentSurface;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat1 =` not needed
  data->ReadEntity(num,
                   1,
                   "parent_surface",
                   ach,
                   STANDARD_TYPE(StepGeom_BoundedSurface),
                   aParentSurface);

  // --- own field : uTransition ---

  StepGeom_TransitionCode aUTransition = StepGeom_tcDiscontinuous;
  if (data->ParamType(num, 2) == Interface_ParamEnum)
  {
    Standard_CString text = data->ParamCValue(num, 2);
    if (!RWStepGeom_RWTransitionCode::ConvertToEnum(text, aUTransition))
    {
      ach->AddFail("Enumeration transition_code has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #2 (u_transition) is not an enumeration");

  // --- own field : vTransition ---

  StepGeom_TransitionCode aVTransition = StepGeom_tcDiscontinuous;
  if (data->ParamType(num, 3) == Interface_ParamEnum)
  {
    Standard_CString text = data->ParamCValue(num, 3);
    if (!RWStepGeom_RWTransitionCode::ConvertToEnum(text, aVTransition))
    {
      ach->AddFail("Enumeration transition_code has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #3 (v_transition) is not an enumeration");

  // --- own field : uSense ---

  Standard_Boolean aUSense;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat4 =` not needed
  data->ReadBoolean(num, 4, "u_sense", ach, aUSense);

  // --- own field : vSense ---

  Standard_Boolean aVSense;
  // szv#4:S4163:12Mar99 `Standard_Boolean stat5 =` not needed
  data->ReadBoolean(num, 5, "v_sense", ach, aVSense);

  //--- Initialisation of the read entity ---

  ent->Init(aParentSurface, aUTransition, aVTransition, aUSense, aVSense);
}

void RWStepGeom_RWSurfacePatch::WriteStep(StepData_StepWriter&                 SW,
                                          const Handle(StepGeom_SurfacePatch)& ent) const
{

  // --- own field : parentSurface ---

  SW.Send(ent->ParentSurface());

  // --- own field : uTransition ---

  SW.SendEnum(RWStepGeom_RWTransitionCode::ConvertToString(ent->UTransition()));

  // --- own field : vTransition ---

  SW.SendEnum(RWStepGeom_RWTransitionCode::ConvertToString(ent->VTransition()));

  // --- own field : uSense ---

  SW.SendBoolean(ent->USense());

  // --- own field : vSense ---

  SW.SendBoolean(ent->VSense());
}

void RWStepGeom_RWSurfacePatch::Share(const Handle(StepGeom_SurfacePatch)& ent,
                                      Interface_EntityIterator&            iter) const
{

  iter.GetOneItem(ent->ParentSurface());
}

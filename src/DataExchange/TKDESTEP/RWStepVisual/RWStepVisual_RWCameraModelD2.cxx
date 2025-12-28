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
#include "RWStepVisual_RWCameraModelD2.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepVisual_CameraModelD2.hxx>
#include <StepVisual_PlanarBox.hxx>

RWStepVisual_RWCameraModelD2::RWStepVisual_RWCameraModelD2() {}

void RWStepVisual_RWCameraModelD2::ReadStep(const occ::handle<StepData_StepReaderData>&  data,
                                            const int                                    num,
                                            occ::handle<Interface_Check>&                ach,
                                            const occ::handle<StepVisual_CameraModelD2>& ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "camera_model_d2"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : viewWindow ---

  occ::handle<StepVisual_PlanarBox> aViewWindow;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "view_window", ach, STANDARD_TYPE(StepVisual_PlanarBox), aViewWindow);

  // --- own field : viewWindowClipping ---

  bool aViewWindowClipping;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadBoolean(num, 3, "view_window_clipping", ach, aViewWindowClipping);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aViewWindow, aViewWindowClipping);
}

void RWStepVisual_RWCameraModelD2::WriteStep(StepData_StepWriter&                         SW,
                                             const occ::handle<StepVisual_CameraModelD2>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : viewWindow ---

  SW.Send(ent->ViewWindow());

  // --- own field : viewWindowClipping ---

  SW.SendBoolean(ent->ViewWindowClipping());
}

void RWStepVisual_RWCameraModelD2::Share(const occ::handle<StepVisual_CameraModelD2>& ent,
                                         Interface_EntityIterator&                    iter) const
{

  iter.GetOneItem(ent->ViewWindow());
}

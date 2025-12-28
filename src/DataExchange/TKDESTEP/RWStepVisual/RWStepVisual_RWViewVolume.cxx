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
#include "RWStepVisual_RWViewVolume.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepVisual_PlanarBox.hxx>
#include <StepVisual_ViewVolume.hxx>
#include <TCollection_AsciiString.hxx>

#include "RWStepVisual_RWCentralOrParallel.pxx"

RWStepVisual_RWViewVolume::RWStepVisual_RWViewVolume() {}

void RWStepVisual_RWViewVolume::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                         const int                                   num,
                                         occ::handle<Interface_Check>&               ach,
                                         const occ::handle<StepVisual_ViewVolume>&   ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 9, ach, "view_volume"))
    return;

  // --- own field : projectionType ---

  StepVisual_CentralOrParallel aProjectionType = StepVisual_copCentral;
  if (data->ParamType(num, 1) == Interface_ParamEnum)
  {
    const char* text = data->ParamCValue(num, 1);
    if (!RWStepVisual_RWCentralOrParallel::ConvertToEnum(text, aProjectionType))
    {
      ach->AddFail("Enumeration central_or_parallel has not an allowed value");
    }
  }
  else
    ach->AddFail("Parameter #1 (projection_type) is not an enumeration");

  // --- own field : projectionPoint ---

  occ::handle<StepGeom_CartesianPoint> aProjectionPoint;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num,
                   2,
                   "projection_point",
                   ach,
                   STANDARD_TYPE(StepGeom_CartesianPoint),
                   aProjectionPoint);

  // --- own field : viewPlaneDistance ---

  double aViewPlaneDistance;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadReal(num, 3, "view_plane_distance", ach, aViewPlaneDistance);

  // --- own field : frontPlaneDistance ---

  double aFrontPlaneDistance;
  // szv#4:S4163:12Mar99 `bool stat4 =` not needed
  data->ReadReal(num, 4, "front_plane_distance", ach, aFrontPlaneDistance);

  // --- own field : frontPlaneClipping ---

  bool aFrontPlaneClipping;
  // szv#4:S4163:12Mar99 `bool stat5 =` not needed
  data->ReadBoolean(num, 5, "front_plane_clipping", ach, aFrontPlaneClipping);

  // --- own field : backPlaneDistance ---

  double aBackPlaneDistance;
  // szv#4:S4163:12Mar99 `bool stat6 =` not needed
  data->ReadReal(num, 6, "back_plane_distance", ach, aBackPlaneDistance);

  // --- own field : backPlaneClipping ---

  bool aBackPlaneClipping;
  // szv#4:S4163:12Mar99 `bool stat7 =` not needed
  data->ReadBoolean(num, 7, "back_plane_clipping", ach, aBackPlaneClipping);

  // --- own field : viewVolumeSidesClipping ---

  bool aViewVolumeSidesClipping;
  // szv#4:S4163:12Mar99 `bool stat8 =` not needed
  data->ReadBoolean(num, 8, "view_volume_sides_clipping", ach, aViewVolumeSidesClipping);

  // --- own field : viewWindow ---

  occ::handle<StepVisual_PlanarBox> aViewWindow;
  // szv#4:S4163:12Mar99 `bool stat9 =` not needed
  data->ReadEntity(num, 9, "view_window", ach, STANDARD_TYPE(StepVisual_PlanarBox), aViewWindow);

  //--- Initialisation of the read entity ---

  ent->Init(aProjectionType,
            aProjectionPoint,
            aViewPlaneDistance,
            aFrontPlaneDistance,
            aFrontPlaneClipping,
            aBackPlaneDistance,
            aBackPlaneClipping,
            aViewVolumeSidesClipping,
            aViewWindow);
}

void RWStepVisual_RWViewVolume::WriteStep(StepData_StepWriter&                      SW,
                                          const occ::handle<StepVisual_ViewVolume>& ent) const
{

  // --- own field : projectionType ---

  SW.SendEnum(RWStepVisual_RWCentralOrParallel::ConvertToString(ent->ProjectionType()));

  // --- own field : projectionPoint ---

  SW.Send(ent->ProjectionPoint());

  // --- own field : viewPlaneDistance ---

  SW.Send(ent->ViewPlaneDistance());

  // --- own field : frontPlaneDistance ---

  SW.Send(ent->FrontPlaneDistance());

  // --- own field : frontPlaneClipping ---

  SW.SendBoolean(ent->FrontPlaneClipping());

  // --- own field : backPlaneDistance ---

  SW.Send(ent->BackPlaneDistance());

  // --- own field : backPlaneClipping ---

  SW.SendBoolean(ent->BackPlaneClipping());

  // --- own field : viewVolumeSidesClipping ---

  SW.SendBoolean(ent->ViewVolumeSidesClipping());

  // --- own field : viewWindow ---

  SW.Send(ent->ViewWindow());
}

void RWStepVisual_RWViewVolume::Share(const occ::handle<StepVisual_ViewVolume>& ent,
                                      Interface_EntityIterator&                 iter) const
{

  iter.GetOneItem(ent->ProjectionPoint());

  iter.GetOneItem(ent->ViewWindow());
}

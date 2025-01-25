// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Aspect_XRSession.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Aspect_XRSession, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Aspect_XRAction, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(Aspect_XRActionSet, Standard_Transient)

// =======================================================================
// function : Aspect_XRSession
// purpose  :
// =======================================================================
Aspect_XRSession::Aspect_XRSession()
    : myTrackOrigin(TrackingUniverseOrigin_Standing),
      myTrackedPoses(0, 0),
      myUnitFactor(1.0),
      myAspect(1.0),
      myFieldOfView(90.0),
      myIod(0.0),
      myDispFreq(0.0f)
{
  for (Standard_Integer aRoleIter = 0; aRoleIter < Aspect_XRTrackedDeviceRole_NB; ++aRoleIter)
  {
    myRoleActions[aRoleIter].Resize(0, Aspect_XRGenericAction_NB - 1, false);
  }
}

// =======================================================================
// function : AbortHapticVibrationAction
// purpose  :
// =======================================================================
void Aspect_XRSession::AbortHapticVibrationAction(const Handle(Aspect_XRAction)& theAction)
{
  triggerHapticVibrationAction(theAction, Aspect_XRHapticActionData());
}

// =======================================================================
// function : TriggerHapticVibrationAction
// purpose  :
// =======================================================================
void Aspect_XRSession::TriggerHapticVibrationAction(const Handle(Aspect_XRAction)&   theAction,
                                                    const Aspect_XRHapticActionData& theParams)
{
  if (!theParams.IsValid())
  {
    throw Standard_ProgramError(
      "Aspect_OpenVRSession::TriggerHapticVibrationAction() called for wrong action");
  }
  triggerHapticVibrationAction(theAction, theParams);
}

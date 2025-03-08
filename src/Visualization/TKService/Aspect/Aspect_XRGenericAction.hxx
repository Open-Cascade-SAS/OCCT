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

#ifndef _Aspect_XRGenericAction_HeaderFile
#define _Aspect_XRGenericAction_HeaderFile

//! Generic XR action.
enum Aspect_XRGenericAction
{
  Aspect_XRGenericAction_IsHeadsetOn,           //!< headset is on/off head
  Aspect_XRGenericAction_InputAppMenu,          //!< application menu button pressed/released
  Aspect_XRGenericAction_InputSysMenu,          //!< system menu button pressed/released
  Aspect_XRGenericAction_InputTriggerPull,      //!< trigger squeezing [0..1], 1 to click
  Aspect_XRGenericAction_InputTriggerClick,     //!< trigger clicked/released
  Aspect_XRGenericAction_InputGripClick,        //!< grip state on/off
  Aspect_XRGenericAction_InputTrackPadPosition, //!< trackpad 2D position [-1,+1] with X and Y axes
  Aspect_XRGenericAction_InputTrackPadTouch,    //!< trackpad touched/untouched
  Aspect_XRGenericAction_InputTrackPadClick,    //!< trackpad clicked/released
  Aspect_XRGenericAction_InputThumbstickPosition, //!< thumbstick 2D position [-1,+1] with X and Y
                                                  //!< axes
  Aspect_XRGenericAction_InputThumbstickTouch,    //!< thumbstick touched/untouched
  Aspect_XRGenericAction_InputThumbstickClick,    //!< thumbstick clicked/released
  Aspect_XRGenericAction_InputPoseBase,           //!< base position of hand
  Aspect_XRGenericAction_InputPoseFront,          //!< front position of hand
  Aspect_XRGenericAction_InputPoseHandGrip,       //!< position of main handgrip
  Aspect_XRGenericAction_InputPoseFingerTip,      //!< position of main fingertip
  Aspect_XRGenericAction_OutputHaptic             //!< haptic output (vibration)
};

enum
{
  Aspect_XRGenericAction_NB = Aspect_XRGenericAction_OutputHaptic + 1
};

#endif // _Aspect_XRGenericAction_HeaderFile

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

#ifndef _Aspect_XRActionType_HeaderFile
#define _Aspect_XRActionType_HeaderFile

//! XR action type.
enum Aspect_XRActionType
{
  Aspect_XRActionType_InputDigital,  //!< boolean input (like button)
  Aspect_XRActionType_InputAnalog,   //!< analog input (1/2/3 axes)
  Aspect_XRActionType_InputPose,     //!< positional input
  Aspect_XRActionType_InputSkeletal, //!< skeletal input
  Aspect_XRActionType_OutputHaptic   //!< haptic output (vibration)
};

#endif // _Aspect_XRActionType_HeaderFile

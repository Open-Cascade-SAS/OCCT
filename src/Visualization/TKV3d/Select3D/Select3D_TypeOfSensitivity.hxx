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

#ifndef _Select3D_TypeOfSensitivity_HeaderFile
#define _Select3D_TypeOfSensitivity_HeaderFile

//! Provides values for type of sensitivity in 3D.
//! These are used to specify whether it is the interior,
//! the boundary, or the exterior of a 3D sensitive entity which is sensitive.
enum Select3D_TypeOfSensitivity
{
  Select3D_TOS_INTERIOR,
  Select3D_TOS_BOUNDARY
};

#endif // _Select3D_TypeOfSensitivity_HeaderFile

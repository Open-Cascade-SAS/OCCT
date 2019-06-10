// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _AIS_NavigationMode_HeaderFile
#define _AIS_NavigationMode_HeaderFile

//! Camera navigation mode.
enum AIS_NavigationMode
{
  AIS_NavigationMode_Orbit,             //!< orbit rotation
  AIS_NavigationMode_FirstPersonFlight, //!< flight rotation (first person)
  AIS_NavigationMode_FirstPersonWalk,   //!< walking mode (first person)
};

enum
{
  AIS_NavigationMode_LOWER = 0,
  AIS_NavigationMode_UPPER = AIS_NavigationMode_FirstPersonWalk
};

#endif // _V3d_NavigationMode_HeaderFile

// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _AIS_TrihedronSelectionMode_HeaderFile
#define _AIS_TrihedronSelectionMode_HeaderFile

//! Enumeration defining selection modes supported by AIS_Trihedron.
enum AIS_TrihedronSelectionMode
{
  AIS_TrihedronSelectionMode_EntireObject = 0, //!< select trihedron as whole
  AIS_TrihedronSelectionMode_Origin       = 1, //!< origin
  AIS_TrihedronSelectionMode_Axes         = 2, //!< axes
  AIS_TrihedronSelectionMode_MainPlanes   = 3  //!< main planes
};

#endif // _AIS_TrihedronSelectionMode_HeaderFile

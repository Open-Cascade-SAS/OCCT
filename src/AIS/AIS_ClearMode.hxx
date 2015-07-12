// Created on: 1996-12-11
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _AIS_ClearMode_HeaderFile
#define _AIS_ClearMode_HeaderFile

//! Declares which entities in an opened local context
//! are to be cleared of mode settings. Temporary
//! graphic presentations such as those for sub-shapes,
//! for example, are only created for the selection
//! process. By means of these enumerations, they can
//! be cleared from local context.
enum AIS_ClearMode
{
AIS_CM_All,
AIS_CM_Interactive,
AIS_CM_Filters,
AIS_CM_StandardModes,
AIS_CM_TemporaryShapePrs
};

#endif // _AIS_ClearMode_HeaderFile

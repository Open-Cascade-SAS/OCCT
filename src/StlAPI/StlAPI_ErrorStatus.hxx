// Created on: 1997-05-13
// Created by: Fabien REUTER
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _StlAPI_ErrorStatus_HeaderFile
#define _StlAPI_ErrorStatus_HeaderFile

//! Set of statuses to indicate a type of the error
//! occurred during data retrieving and writing operations.
enum StlAPI_ErrorStatus
{
StlAPI_StatusOK,
StlAPI_MeshIsEmpty,
StlAPI_CannotOpenFile
};

#endif // _StlAPI_ErrorStatus_HeaderFile

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

#ifndef _AIS_ConnectStatus_HeaderFile
#define _AIS_ConnectStatus_HeaderFile

//! Gives the status of connection of an Interactive
//! Object. This will be one of the following:
//! -   No connection
//! -   Connection
//! -   Transformation
//! -   Both connection and transformation
//! This enumeration is used in
//! AIS_ConnectedInteractive. Transform indicates
//! that the Interactive Object reference geometry has
//! changed location relative to the reference geometry.
enum AIS_ConnectStatus
{
AIS_CS_None,
AIS_CS_Connection,
AIS_CS_Transform,
AIS_CS_Both
};

#endif // _AIS_ConnectStatus_HeaderFile

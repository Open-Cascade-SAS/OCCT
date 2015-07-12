// Created on: 1992-08-26
// Created by: Jean Louis FRENKEL
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _StdPrs_Volume_HeaderFile
#define _StdPrs_Volume_HeaderFile

//! defines the way how to interpret input shapes
//! Volume_Autodetection to perform Autodetection (would split input shape into two groups)
//! Volume_Closed as Closed volumes (to activate back-face culling and capping plane algorithms)
//! Volume_Opened as Open volumes (shells or solids with holes)
enum StdPrs_Volume
{
StdPrs_Volume_Autodetection,
StdPrs_Volume_Closed,
StdPrs_Volume_Opened
};

#endif // _StdPrs_Volume_HeaderFile

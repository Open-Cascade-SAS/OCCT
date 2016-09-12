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

#ifndef _OSD_WhoAmI_HeaderFile
#define _OSD_WhoAmI_HeaderFile

//! Allows great accuracy for error management.
//! This is private.
enum OSD_WhoAmI
{
OSD_WDirectory,
OSD_WDirectoryIterator,
OSD_WEnvironment,
OSD_WFile,
OSD_WFileNode,
OSD_WFileIterator,
OSD_WPath,
OSD_WProcess,
OSD_WProtection,
OSD_WHost,
OSD_WDisk,
OSD_WChronometer,
OSD_WTimer,
OSD_WPackage,
OSD_WEnvironmentIterator
};

#endif // _OSD_WhoAmI_HeaderFile

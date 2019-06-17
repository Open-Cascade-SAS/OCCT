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

#ifndef _OSD_SignalMode_HeaderFile
#define _OSD_SignalMode_HeaderFile

//! Mode of operation for OSD::SetSignal() function
enum OSD_SignalMode
{
OSD_SignalMode_AsIs,         //!< Do not set or remove signal handlers
OSD_SignalMode_Set,          //!< Set OCCT signal handlers
OSD_SignalMode_SetUnhandled, //!< Set OCCT signal handler but only if no handler is set, for each particular signal type
OSD_SignalMode_Unset         //!< Unset signal handler to system default
};

#endif // _OSD_SignalMode_HeaderFile

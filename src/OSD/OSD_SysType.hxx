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

#ifndef _OSD_SysType_HeaderFile
#define _OSD_SysType_HeaderFile

//! Thisd is a set of possible system types.
//! 'Default' means SysType of machine operating this process.
//! This can be used with the Path class.
//! All UNIX-like are grouped under "UnixBSD" or "UnixSystemV".
//! Such systems are Solaris, NexTOS ...
//! A category of systems accept MSDOS-like path such as
//! WindowsNT and OS2.
enum OSD_SysType
{
  OSD_Unknown,
  OSD_Default,
  OSD_UnixBSD,
  OSD_UnixSystemV,
  OSD_VMS,
  OSD_OS2,
  OSD_OSF,
  OSD_MacOs,
  OSD_Taligent,
  OSD_WindowsNT,
  OSD_LinuxREDHAT,
  OSD_Aix
};

#endif // _OSD_SysType_HeaderFile

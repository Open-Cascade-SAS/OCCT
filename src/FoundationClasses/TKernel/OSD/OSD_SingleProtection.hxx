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

#ifndef _OSD_SingleProtection_HeaderFile
#define _OSD_SingleProtection_HeaderFile

//! Access rights for files.
//! R means Read, W means Write, X means eXecute and D means Delete.
//! On UNIX, the right to Delete is combined with Write access.
//! So if "W"rite is not set and "D"elete is, "W"rite will be set
//! and if "W" is set, "D" will be too.
enum OSD_SingleProtection
{
  OSD_None,
  OSD_R,
  OSD_W,
  OSD_RW,
  OSD_X,
  OSD_RX,
  OSD_WX,
  OSD_RWX,
  OSD_D,
  OSD_RD,
  OSD_WD,
  OSD_RWD,
  OSD_XD,
  OSD_RXD,
  OSD_WXD,
  OSD_RWXD
};

#endif // _OSD_SingleProtection_HeaderFile

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

#ifndef _OSD_SIGHUP_HeaderFile
#define _OSD_SIGHUP_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_SStream.hxx>
#include <OSD_Signal.hxx>

class OSD_SIGHUP;
DEFINE_STANDARD_HANDLE(OSD_SIGHUP, OSD_Signal)

#if !defined No_Exception && !defined No_OSD_SIGHUP
  #define OSD_SIGHUP_Raise_if(CONDITION, MESSAGE)                                                  \
    if (CONDITION)                                                                                 \
      throw OSD_SIGHUP(MESSAGE);
#else
  #define OSD_SIGHUP_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(OSD_SIGHUP, OSD_Signal)

#endif // _OSD_SIGHUP_HeaderFile

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

#ifndef _PrsMgr_DisplayStatus_HeaderFile
#define _PrsMgr_DisplayStatus_HeaderFile

//! To give the display status of an Interactive Object.
enum PrsMgr_DisplayStatus
{
  PrsMgr_DisplayStatus_Displayed, //!< the Interactive Object is displayed in the main viewer
  PrsMgr_DisplayStatus_Erased,    //!< the Interactive Object is hidden in main viewer
  PrsMgr_DisplayStatus_None,      //!< the Interactive Object is nowhere displayed
  // old aliases
  AIS_DS_Displayed = PrsMgr_DisplayStatus_Displayed,
  AIS_DS_Erased    = PrsMgr_DisplayStatus_Erased,
  AIS_DS_None      = PrsMgr_DisplayStatus_None
};

#endif // _PrsMgr_DisplayStatus_HeaderFile

// Created on: 2002-07-09
// Created by: Andrey BETENEV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

/*======================================================================
// 
// Purpose:   Defines macros identifying current version of Open CASCADE
//
//            OCC_VERSION_MAJOR       : (integer) number identifying major version 
//            OCC_VERSION_MINOR       : (integer) number identifying minor version 
//            OCC_VERSION_MAINTENANCE : (integer) number identifying maintenance version 
//            OCC_VERSION_DEVELOPMENT : (string)  if defined, indicates development or modified version
//            OCC_VERSION             : (real)    complete number (major.minor)
//            OCC_VERSION_STRING      : (string)  short version number ("major.minor")
//            OCC_VERSION_COMPLETE    : (string)  complete version number ("major.minor.maintenance")
//            OCC_VERSION_STRING_EXT  : (string)  extended version ("major.minor.maintenance.development")
//            OCC_VERSION_HEX         : (hex)     complete number as hex, two positions per each of major, minor, and patch number 
//            
//======================================================================*/

#ifndef _Standard_Version_HeaderFile
#define _Standard_Version_HeaderFile

// Primary definitions
#define OCC_VERSION_MAJOR         7
#define OCC_VERSION_MINOR         6
#define OCC_VERSION_MAINTENANCE   0

//! This macro must be commented in official release, and set to non-empty 
//! string in other situations, to identify specifics of the version, e.g.:
//! - "dev" for development version between releases
//! - "beta..." or "rc..." for beta releases or release candidates
//! - "project..." for version containing project-specific fixes
//#define OCC_VERSION_DEVELOPMENT   "dev"

// Derived (manually): version as real and string (major.minor)
#define OCC_VERSION               7.6
#define OCC_VERSION_STRING       "7.6"
#define OCC_VERSION_COMPLETE     "7.6.0"

//! Derived: extended version as string ("major.minor.maintenance.dev")
#ifdef OCC_VERSION_DEVELOPMENT
#define OCC_VERSION_STRING_EXT OCC_VERSION_COMPLETE "." OCC_VERSION_DEVELOPMENT
#else
#define OCC_VERSION_STRING_EXT OCC_VERSION_COMPLETE
#endif

// Derived: complete version as hex (0x0'major'0'minor'0'maintenance')
#define OCC_VERSION_HEX    (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE)

#endif  /* _Standard_Version_HeaderFile */

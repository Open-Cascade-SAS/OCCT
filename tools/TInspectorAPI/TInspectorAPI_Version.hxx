// Copyright (c) 2018 OPEN CASCADE SAS
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
// Purpose:   Defines macros identifying current version of Inspector
// The version is equal to current OCCT version and used to support compile compatibility
// to previous OCCT versions interface.
// To compile Inspector with earlier version of OCCT, change version variable here to needed OCCT version.
//
//            TINSPECTORAPI_VERSION_MAJOR       : (integer) number identifying major version 
//            TINSPECTORAPI_VERSION_MINOR       : (integer) number identifying minor version 
//            TINSPECTORAPI_VERSION_MAINTENANCE : (integer) number identifying maintenance version 
//            TINSPECTORAPI_VERSION_HEX         : (hex)     complete number as hex, two positions per each of major, minor, and patch number 
//            
//======================================================================*/

#ifndef _TInspectorAPI_Version_HeaderFile
#define _TInspectorAPI_Version_HeaderFile

#include <Standard_Version.hxx>

// Primary definitions
#define TINSPECTORAPI_VERSION_MAJOR         OCC_VERSION_MAJOR
#define TINSPECTORAPI_VERSION_MINOR         OCC_VERSION_MINOR
#define TINSPECTORAPI_VERSION_MAINTENANCE   0//OCC_VERSION_MAINTENANCE

// Derived: complete version as hex (0x0'major'0'minor'0'maintenance')
#define TINSPECTORAPI_VERSION_HEX    (TINSPECTORAPI_VERSION_MAJOR << 16 | TINSPECTORAPI_VERSION_MINOR << 8 | TINSPECTORAPI_VERSION_MAINTENANCE)

#endif  /* _TInspectorAPI_Version_HeaderFile */

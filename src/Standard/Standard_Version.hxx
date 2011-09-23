/*======================================================================
// 
// File:      Standard_Version.hxx
// Created:   Tue Jul  9 17:42:58 2002
// Author:    Andrey BETENEV
// Copyright: Open CASCADE S.A. 2002
//
// Purpose:   Defines macros identifying current version of Open CASCADE
//
//            OCC_VERSION_MAJOR       : (integer) number identifying major version 
//            OCC_VERSION_MINOR       : (integer) number identifying minor version 
//            OCC_VERSION_MAINTENANCE : (integer) number identifying maintenance version 
//            OCC_VERSION_DEVELOPMENT : (no value) if defined, identify development version.
//            OCC_VERSION             : (real)    complete number (major.minor)
//            OCC_VERSION_STRING      : (string)  complete number ("major.minor")
//            OCC_VERSION_HEX         : (hex)     complete number as hex, two positions per each of major, minor, and patch number 
//            
//======================================================================*/

#ifndef _Standard_Version_HeaderFile
#define _Standard_Version_HeaderFile

// Primary definitions
#define OCC_VERSION_MAJOR         6
#define OCC_VERSION_MINOR         5
#define OCC_VERSION_MAINTENANCE   2
//#define OCC_VERSION_DEVELOPMENT // This line must be commented in any final version (release)

// Derived: version as real and string (major.minor)
#define OCC_VERSION         6.5
#define OCC_VERSION_STRING "6.5"

// Derived: version as hex (0x0'major'0'minor'0'maintenance')
#define OCC_VERSION_HEX    (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE)

#endif  /* _Standard_Version_HeaderFile */

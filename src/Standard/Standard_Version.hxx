// Created on: 2002-07-09
// Created by: Andrey BETENEV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

/*======================================================================
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
#define OCC_VERSION_DEVELOPMENT // This line must be commented in any final version (release)

// Derived: version as real and string (major.minor)
#define OCC_VERSION         6.5
#define OCC_VERSION_STRING "6.5"

// Derived: version as hex (0x0'major'0'minor'0'maintenance')
#define OCC_VERSION_HEX    (OCC_VERSION_MAJOR << 16 | OCC_VERSION_MINOR << 8 | OCC_VERSION_MAINTENANCE)

#endif  /* _Standard_Version_HeaderFile */

// Created on: 2005-08-24
// Created by: ABV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

// Purpose:     Defines some portability macros

#ifndef NCollection_TypeDef_HeaderFile
#define NCollection_TypeDef_HeaderFile

// Macro TYPENAME - either C++ keyword typename, or empty on
// platforms that do not support it
#if defined(__SUNPRO_CC) && (__SUNPRO_CC <= 0x530)
// work-around against obsolete SUN WorkShop 5.3 compiler
#define TYPENAME
#else
#define TYPENAME typename
#endif

#endif

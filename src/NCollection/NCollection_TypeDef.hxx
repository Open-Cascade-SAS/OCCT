// File:        NCollection_TypeDef.hxx
// Created:     Wed Aug 24 09:22:55 2005
// Author:      ABV
//
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

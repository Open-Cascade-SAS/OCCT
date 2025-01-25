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

#ifndef _Font_FontAspect_HeaderFile
#define _Font_FontAspect_HeaderFile

//! Specifies aspect of system font.
enum Font_FontAspect
{
  Font_FontAspect_UNDEFINED = -1, //!< special value reserved for undefined aspect
  Font_FontAspect_Regular   = 0,  //!< normal (regular) aspect
  Font_FontAspect_Bold,           //!< bold aspect
  Font_FontAspect_Italic,         //!< italic aspect
  Font_FontAspect_BoldItalic,     //!< bold+italic aspect

  // old aliases
  Font_FA_Undefined  = Font_FontAspect_UNDEFINED,
  Font_FA_Regular    = Font_FontAspect_Regular,
  Font_FA_Bold       = Font_FontAspect_Bold,
  Font_FA_Italic     = Font_FontAspect_Italic,
  Font_FA_BoldItalic = Font_FontAspect_BoldItalic
};

enum
{
  Font_FontAspect_NB = Font_FontAspect_BoldItalic + 1
};

#endif // _Font_FontAspect_HeaderFile

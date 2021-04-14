// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Font_Hinting_HeaderFile
#define _Font_Hinting_HeaderFile

//! Enumeration defining font hinting options.
enum Font_Hinting
{
  // hinting style
  Font_Hinting_Off           = 0x00, //!< no      hinting (FT_LOAD_NO_HINTING)
  Font_Hinting_Normal        = 0x01, //!< default hinting (FT_LOAD_TARGET_NORMAL)
  Font_Hinting_Light         = 0x02, //!< light   hinting (FT_LOAD_TARGET_LIGHT)
  // native/autohinting flags
  Font_Hinting_ForceAutohint = 0x10, //!< prefer autohinting over native hinting (FT_LOAD_FORCE_AUTOHINT)
  Font_Hinting_NoAutohint    = 0x20, //!< disallow autohinting (FT_LOAD_NO_AUTOHINT)
};

#endif // _Font_Hinting_HeaderFile

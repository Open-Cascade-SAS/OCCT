// Copyright (c) 2019 OPEN CASCADE SAS
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

#ifndef _Font_StrictLevel_HeaderFile
#define _Font_StrictLevel_HeaderFile

//! Enumeration defining font search restrictions.
enum Font_StrictLevel
{
  Font_StrictLevel_Strict,  //!< search only for exact font
  Font_StrictLevel_Aliases, //!< search for exact font match and for aliases (ignore global
                            //!< fallback)
  Font_StrictLevel_Any,     //!< search for any font, including global fallback
};

#endif // _Font_StrictLevel_HeaderFile

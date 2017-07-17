// Created on: 2013-01-28
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Font_FTLibrary.hxx>

#include <ft2build.h>
#include FT_FREETYPE_H

IMPLEMENT_STANDARD_RTTIEXT(Font_FTLibrary,Standard_Transient)

// =======================================================================
// function : Font_FTLibrary
// purpose  :
// =======================================================================
Font_FTLibrary::Font_FTLibrary()
: myFTLib (NULL)
{
  if (FT_Init_FreeType (&myFTLib) != 0)
  {
    myFTLib = NULL;
  }
}

// =======================================================================
// function : ~Font_FTLibrary
// purpose  :
// =======================================================================
Font_FTLibrary::~Font_FTLibrary()
{
  if (IsValid())
  {
    FT_Done_FreeType (myFTLib);
  }
}

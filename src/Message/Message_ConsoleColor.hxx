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

#ifndef _Message_ConsoleColor_HeaderFile
#define _Message_ConsoleColor_HeaderFile

//! Color definition for console/terminal output (limited palette).
enum Message_ConsoleColor
{
  Message_ConsoleColor_Default, //!< default (white) color
  Message_ConsoleColor_Black,   //!< black   color
  Message_ConsoleColor_White,   //!< white   color
  Message_ConsoleColor_Red,     //!< red     color
  Message_ConsoleColor_Blue,    //!< blue    color
  Message_ConsoleColor_Green,   //!< green   color
  Message_ConsoleColor_Yellow,  //!< yellow  color
  Message_ConsoleColor_Cyan,    //!< cyan    color
  Message_ConsoleColor_Magenta, //!< magenta color
};

#endif // _Message_ConsoleColor_HeaderFile

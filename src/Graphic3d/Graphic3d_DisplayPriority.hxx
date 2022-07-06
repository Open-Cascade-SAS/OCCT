// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Graphic3d_DisplayPriority_HeaderFile
#define _Graphic3d_DisplayPriority_HeaderFile

//! Structure priority - range (do not change this range!).
//! Values are between 0 and 10, with 5 used by default.
//! A structure of priority 10 is displayed the last and appears over the others (considering depth test).
enum Graphic3d_DisplayPriority
{
  Graphic3d_DisplayPriority_INVALID      = -1,
  Graphic3d_DisplayPriority_Bottom       =  0,
  Graphic3d_DisplayPriority_AlmostBottom =  1,
  Graphic3d_DisplayPriority_Below2       =  2,
  Graphic3d_DisplayPriority_Below1       =  3,
  Graphic3d_DisplayPriority_Below        =  4,
  Graphic3d_DisplayPriority_Normal       =  5,
  Graphic3d_DisplayPriority_Above        =  6,
  Graphic3d_DisplayPriority_Above1       =  7,
  Graphic3d_DisplayPriority_Above2       =  8,
  Graphic3d_DisplayPriority_Highlight    =  9,
  Graphic3d_DisplayPriority_Topmost      = 10,
};
enum { Graphic3d_DisplayPriority_NB = Graphic3d_DisplayPriority_Topmost - Graphic3d_DisplayPriority_Bottom + 1 };

#endif // _Graphic3d_DisplayPriority_HeaderFile

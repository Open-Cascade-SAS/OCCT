// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Aspect_GradientFillMethod_HeaderFile
#define _Aspect_GradientFillMethod_HeaderFile

//! Defines the fill methods to write gradient background in a window.
enum Aspect_GradientFillMethod
{
  Aspect_GradientFillMethod_None,        //!< fill method not specified
  Aspect_GradientFillMethod_Horizontal,  //!< gradient directed from left (Color1) to right (Color2)
  Aspect_GradientFillMethod_Vertical,    //!< gradient directed from top (Color1) to bottom (Color2)
  Aspect_GradientFillMethod_Diagonal1,   //!< gradient directed from upper left corner (Color1) to lower right (Color2)
  Aspect_GradientFillMethod_Diagonal2,   //!< gradient directed from upper right corner (Color1) to lower left (Color2)
  Aspect_GradientFillMethod_Corner1,     //!< highlights upper left corner with Color1
  Aspect_GradientFillMethod_Corner2,     //!< highlights upper right corner with Color1
  Aspect_GradientFillMethod_Corner3,     //!< highlights lower right corner with Color1
  Aspect_GradientFillMethod_Corner4,     //!< highlights lower left corner with Color1
  Aspect_GradientFillMethod_Elliptical,  //!< gradient directed from center (Color1) in all directions forming an elliptic shape (Color2)

  // obsolete aliases
  Aspect_GFM_NONE    = Aspect_GradientFillMethod_None,
  Aspect_GFM_HOR     = Aspect_GradientFillMethod_Horizontal,
  Aspect_GFM_VER     = Aspect_GradientFillMethod_Vertical,
  Aspect_GFM_DIAG1   = Aspect_GradientFillMethod_Diagonal1,
  Aspect_GFM_DIAG2   = Aspect_GradientFillMethod_Diagonal2,
  Aspect_GFM_CORNER1 = Aspect_GradientFillMethod_Corner1,
  Aspect_GFM_CORNER2 = Aspect_GradientFillMethod_Corner2,
  Aspect_GFM_CORNER3 = Aspect_GradientFillMethod_Corner3,
  Aspect_GFM_CORNER4 = Aspect_GradientFillMethod_Corner4
};

#endif // _Aspect_GradientFillMethod_HeaderFile

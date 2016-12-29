// Created on: 1993-09-14
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Prs3d_TextAspect.hxx>

#include <Font_NameOfFont.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_TextAspect, Prs3d_BasicAspect)

// =======================================================================
// function : Prs3d_TextAspect
// purpose  :
// =======================================================================
Prs3d_TextAspect::Prs3d_TextAspect()
: myTextAspect (new Graphic3d_AspectText3d (Quantity_Color (Quantity_NOC_YELLOW), Font_NOF_ASCII_TRIPLEX, 1.0, 0.0)),
  myAngle (0.0),
  myHeight(16.0),
  myHorizontalJustification (Graphic3d_HTA_LEFT),
  myVerticalJustification (Graphic3d_VTA_BOTTOM),
  myOrientation (Graphic3d_TP_RIGHT)
{
  //
}

// =======================================================================
// function : Prs3d_TextAspect
// purpose  :
// =======================================================================
Prs3d_TextAspect::Prs3d_TextAspect (const Handle(Graphic3d_AspectText3d)& theAspect)
: myTextAspect (theAspect),
  myAngle (0.0),
  myHeight(16.0),
  myHorizontalJustification (Graphic3d_HTA_LEFT),
  myVerticalJustification (Graphic3d_VTA_BOTTOM),
  myOrientation (Graphic3d_TP_RIGHT)
{
  //
}

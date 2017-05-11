// Created on: 2000-08-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <XCAFPrs_Style.hxx>

//=======================================================================
//function : XCAFPrs_Style
//purpose  :
//=======================================================================
XCAFPrs_Style::XCAFPrs_Style()
: myHasColorSurf(Standard_False),
  myHasColorCurv(Standard_False),
  myIsVisible   (Standard_True)
{
  //
}

//=======================================================================
//function : SetColorSurf
//purpose  :
//=======================================================================
void XCAFPrs_Style::SetColorSurf (const Quantity_ColorRGBA& theColor)
{
  myColorSurf    = theColor;
  myHasColorSurf = Standard_True;
}

//=======================================================================
//function : UnSetColorSurf
//purpose  :
//=======================================================================
void XCAFPrs_Style::UnSetColorSurf()
{
  myHasColorSurf = Standard_False;
  myColorSurf.ChangeRGB().SetValues (Quantity_NOC_YELLOW);
  myColorSurf.SetAlpha (1.0f);
}

//=======================================================================
//function : SetColorCurv
//purpose  :
//=======================================================================
void XCAFPrs_Style::SetColorCurv (const Quantity_Color& theColor)
{
  myColorCurv    = theColor;
  myHasColorCurv = Standard_True;
}

//=======================================================================
//function : UnSetColorCurv
//purpose  :
//=======================================================================
void XCAFPrs_Style::UnSetColorCurv()
{
  myHasColorCurv = Standard_False;
  myColorCurv.SetValues (Quantity_NOC_YELLOW);
}

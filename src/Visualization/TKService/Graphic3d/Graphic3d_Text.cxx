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

#include <Graphic3d_Text.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Graphic3d_Text, Standard_Transient)

//=================================================================================================

Graphic3d_Text::Graphic3d_Text(const float theHeight)
    : myHeight(theHeight),
      myHAlign(Graphic3d_HTA_LEFT),
      myVAlign(Graphic3d_VTA_BOTTOM),
      myHasPlane(false),
      myHasOwnAnchor(true)
{
}

//=================================================================================================

void Graphic3d_Text::SetOrientation(const gp_Ax2& theOrientation)
{
  myOrientation = theOrientation;
  myHasPlane    = true;
}

//=================================================================================================

void Graphic3d_Text::ResetOrientation()
{
  myOrientation = gp_Ax2();
  myHasPlane    = false;
}

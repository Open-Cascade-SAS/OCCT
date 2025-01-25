// Created on: 1992-04-30
// Created by: Remi LEQUETTE
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

#ifndef _Draw_Circle3D_HeaderFile
#define _Draw_Circle3D_HeaderFile

#include <Standard.hxx>

#include <gp_Circ.hxx>
#include <Standard_Real.hxx>
#include <Draw_Color.hxx>
#include <Draw_Drawable3D.hxx>
class Draw_Display;

class Draw_Circle3D;
DEFINE_STANDARD_HANDLE(Draw_Circle3D, Draw_Drawable3D)

class Draw_Circle3D : public Draw_Drawable3D
{

public:
  Standard_EXPORT Draw_Circle3D(const gp_Circ&      C,
                                const Standard_Real A1,
                                const Standard_Real A2,
                                const Draw_Color&   col);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Draw_Circle3D, Draw_Drawable3D)

protected:
private:
  gp_Circ       myCirc;
  Standard_Real myA1;
  Standard_Real myA2;
  Draw_Color    myColor;
};

#endif // _Draw_Circle3D_HeaderFile

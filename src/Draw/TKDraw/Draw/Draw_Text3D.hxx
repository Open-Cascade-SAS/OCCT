// Created on: 1992-04-29
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

#ifndef _Draw_Text3D_HeaderFile
#define _Draw_Text3D_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt.hxx>
#include <Draw_Color.hxx>
#include <TCollection_AsciiString.hxx>
#include <Draw_Drawable3D.hxx>
class Draw_Display;

class Draw_Text3D : public Draw_Drawable3D
{

public:
  Standard_EXPORT Draw_Text3D(const gp_Pnt& p, const char* const T, const Draw_Color& col);

  Standard_EXPORT Draw_Text3D(const gp_Pnt&     p,
                              const char* const T,
                              const Draw_Color& col,
                              const double      moveX,
                              const double      moveY);

  Standard_EXPORT void SetPnt(const gp_Pnt& p);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const override;

  DEFINE_STANDARD_RTTIEXT(Draw_Text3D, Draw_Drawable3D)

private:
  gp_Pnt                  myPoint;
  Draw_Color              myColor;
  TCollection_AsciiString myText;
  double                  mymoveX;
  double                  mymoveY;
};

#endif // _Draw_Text3D_HeaderFile

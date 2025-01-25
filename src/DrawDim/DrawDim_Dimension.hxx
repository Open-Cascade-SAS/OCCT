// Created on: 1997-04-21
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _DrawDim_Dimension_HeaderFile
#define _DrawDim_Dimension_HeaderFile

#include <Standard.hxx>

#include <Draw_Color.hxx>
#include <Draw_Drawable3D.hxx>
class gp_Pnt;
class Draw_Display;

class DrawDim_Dimension;
DEFINE_STANDARD_HANDLE(DrawDim_Dimension, Draw_Drawable3D)

//! Dimension between planes and cylinder
class DrawDim_Dimension : public Draw_Drawable3D
{

public:
  Standard_EXPORT void SetValue(const Standard_Real avalue);

  Standard_EXPORT Standard_Real GetValue() const;

  Standard_EXPORT Standard_Boolean IsValued() const;

  Standard_EXPORT void TextColor(const Draw_Color& C);

  Standard_EXPORT Draw_Color TextColor() const;

  Standard_EXPORT void DrawText(const gp_Pnt& Pos, Draw_Display& D) const;

  DEFINE_STANDARD_RTTIEXT(DrawDim_Dimension, Draw_Drawable3D)

protected:
  Standard_EXPORT DrawDim_Dimension();

  Standard_Boolean is_valued;
  Standard_Real    myValue;
  Draw_Color       myTextColor;

private:
};

#endif // _DrawDim_Dimension_HeaderFile

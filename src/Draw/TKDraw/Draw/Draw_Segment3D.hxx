// Created on: 1991-04-25
// Created by: Arnaud BOUZY
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

#ifndef _Draw_Segment3D_HeaderFile
#define _Draw_Segment3D_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt.hxx>
#include <Draw_Color.hxx>
#include <Draw_Drawable3D.hxx>
class Draw_Display;

class Draw_Segment3D;
DEFINE_STANDARD_HANDLE(Draw_Segment3D, Draw_Drawable3D)

class Draw_Segment3D : public Draw_Drawable3D
{

public:
  Standard_EXPORT Draw_Segment3D(const gp_Pnt& p1, const gp_Pnt& p2, const Draw_Color& col);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  Standard_EXPORT const gp_Pnt& First() const;

  Standard_EXPORT void First(const gp_Pnt& P);

  Standard_EXPORT const gp_Pnt& Last() const;

  Standard_EXPORT void Last(const gp_Pnt& P);

  DEFINE_STANDARD_RTTIEXT(Draw_Segment3D, Draw_Drawable3D)

protected:
private:
  gp_Pnt     myFirst;
  gp_Pnt     myLast;
  Draw_Color myColor;
};

#endif // _Draw_Segment3D_HeaderFile

// Created on: 1994-04-18
// Created by: Modelistation
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Draw_Text2D_HeaderFile
#define _Draw_Text2D_HeaderFile

#include <Standard.hxx>

#include <gp_Pnt2d.hxx>
#include <Draw_Color.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Draw_Drawable2D.hxx>
class Draw_Display;

class Draw_Text2D;
DEFINE_STANDARD_HANDLE(Draw_Text2D, Draw_Drawable2D)

class Draw_Text2D : public Draw_Drawable2D
{

public:
  Standard_EXPORT Draw_Text2D(const gp_Pnt2d& p, const Standard_CString T, const Draw_Color& col);

  Standard_EXPORT Draw_Text2D(const gp_Pnt2d&        p,
                              const Standard_CString T,
                              const Draw_Color&      col,
                              const Standard_Integer moveX,
                              const Standard_Integer moveY);

  Standard_EXPORT void SetPnt2d(const gp_Pnt2d& p);

  Standard_EXPORT void DrawOn(Draw_Display& dis) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Draw_Text2D, Draw_Drawable2D)

protected:
private:
  gp_Pnt2d                myPoint;
  Draw_Color              myColor;
  TCollection_AsciiString myText;
  Standard_Integer        mymoveX;
  Standard_Integer        mymoveY;
};

#endif // _Draw_Text2D_HeaderFile

// Created on: 1994-10-24
// Created by: Jean Yves LEBEY
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

#ifndef _TestTopOpeDraw_DrawableP3D_HeaderFile
#define _TestTopOpeDraw_DrawableP3D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <gp_Pnt.hxx>
#include <Standard_CString.hxx>
#include <Draw_Color.hxx>
#include <Standard_Real.hxx>
#include <Draw_Marker3D.hxx>
#include <Draw_MarkerShape.hxx>
#include <Standard_Integer.hxx>
class Draw_Text3D;
class gp_Pnt;
class Draw_Color;
class Draw_Display;


class TestTopOpeDraw_DrawableP3D;
DEFINE_STANDARD_HANDLE(TestTopOpeDraw_DrawableP3D, Draw_Marker3D)


class TestTopOpeDraw_DrawableP3D : public Draw_Marker3D
{

public:

  
  Standard_EXPORT TestTopOpeDraw_DrawableP3D(const gp_Pnt& P, const Draw_Color& PColor, const Standard_Real moveX = 0.0, const Standard_Real moveY = 0.0);
  
  Standard_EXPORT TestTopOpeDraw_DrawableP3D(const gp_Pnt& P, const Draw_Color& PColor, const Standard_CString Text, const Draw_Color& TextColor, const Standard_Real moveX = 0.0, const Standard_Real moveY = 0.0);
  
  Standard_EXPORT TestTopOpeDraw_DrawableP3D(const gp_Pnt& P, const Draw_MarkerShape T, const Draw_Color& PColor, const Standard_CString Text, const Draw_Color& TextColor, const Standard_Integer Size = 2, const Standard_Real moveX = 0.0, const Standard_Real moveY = 0.0);
  
  Standard_EXPORT TestTopOpeDraw_DrawableP3D(const gp_Pnt& P, const Draw_MarkerShape T, const Draw_Color& PColor, const Standard_CString Text, const Draw_Color& TextColor, const Standard_Real Tol, const Standard_Real moveX = 0.0, const Standard_Real moveY = 0.0);
  
  Standard_EXPORT void ChangePnt (const gp_Pnt& P);
  
  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(TestTopOpeDraw_DrawableP3D,Draw_Marker3D)

protected:




private:


  gp_Pnt myPnt;
  Standard_CString myText;
  Draw_Color myTextColor;
  Standard_Real myMoveX;
  Standard_Real myMoveY;
  Handle(Draw_Text3D) myText3D;


};







#endif // _TestTopOpeDraw_DrawableP3D_HeaderFile

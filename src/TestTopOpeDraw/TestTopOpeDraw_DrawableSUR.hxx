// Created on: 1996-09-10
// Created by: Jean Yves LEBEY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _TestTopOpeDraw_DrawableSUR_HeaderFile
#define _TestTopOpeDraw_DrawableSUR_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Draw_Color.hxx>
#include <DrawTrSurf_Surface.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class Draw_Text3D;
class Geom_Surface;
class Draw_Color;
class gp_Pnt;
class gp_Pnt2d;
class Draw_Display;


class TestTopOpeDraw_DrawableSUR;
DEFINE_STANDARD_HANDLE(TestTopOpeDraw_DrawableSUR, DrawTrSurf_Surface)


class TestTopOpeDraw_DrawableSUR : public DrawTrSurf_Surface
{

public:

  
  Standard_EXPORT TestTopOpeDraw_DrawableSUR(const Handle(Geom_Surface)& S, const Draw_Color& IsoColor);
  
  Standard_EXPORT TestTopOpeDraw_DrawableSUR(const Handle(Geom_Surface)& S, const Draw_Color& IsoColor, const Standard_CString Text, const Draw_Color& TextColor);
  
  Standard_EXPORT TestTopOpeDraw_DrawableSUR(const Handle(Geom_Surface)& S, const Draw_Color& IsoColor, const Draw_Color& BoundColor, const Draw_Color& NormalColor, const Standard_CString Text, const Draw_Color& TextColor, const Standard_Integer Nu, const Standard_Integer Nv, const Standard_Integer Discret, const Standard_Real Deflection, const Standard_Integer DrawMode, const Standard_Boolean DispOrigin = Standard_True);
  
  Standard_EXPORT gp_Pnt Pnt() const;
  
  Standard_EXPORT gp_Pnt2d Pnt2d() const;
  
  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;
  
  Standard_EXPORT void NormalColor (const Draw_Color& NormalColor);
  
  Standard_EXPORT void DrawNormale (Draw_Display& dis) const;




  DEFINE_STANDARD_RTTI(TestTopOpeDraw_DrawableSUR,DrawTrSurf_Surface)

protected:




private:


  Handle(Draw_Text3D) myText;
  Draw_Color myNormalColor;


};







#endif // _TestTopOpeDraw_DrawableSUR_HeaderFile

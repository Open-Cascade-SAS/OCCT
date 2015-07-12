// Created on: 1995-12-01
// Created by: Jean Yves LEBEY
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _TestTopOpeDraw_DrawableC2D_HeaderFile
#define _TestTopOpeDraw_DrawableC2D_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_CString.hxx>
#include <Draw_Color.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Draw_Interpretor.hxx>
class Draw_Text2D;
class Geom2d_Curve;
class Draw_Color;
class gp_Pnt2d;
class Draw_Display;


class TestTopOpeDraw_DrawableC2D;
DEFINE_STANDARD_HANDLE(TestTopOpeDraw_DrawableC2D, DrawTrSurf_Curve2d)


class TestTopOpeDraw_DrawableC2D : public DrawTrSurf_Curve2d
{

public:

  
  Standard_EXPORT TestTopOpeDraw_DrawableC2D(const Handle(Geom2d_Curve)& C, const Draw_Color& CColor);
  
  Standard_EXPORT TestTopOpeDraw_DrawableC2D(const Handle(Geom2d_Curve)& C, const Draw_Color& CColor, const Standard_CString Text, const Draw_Color& TextColor);
  
  Standard_EXPORT TestTopOpeDraw_DrawableC2D(const Handle(Geom2d_Curve)& C, const Draw_Color& CColor, const Standard_CString Text, const Draw_Color& TextColor, const Standard_Integer Discret, const Standard_Boolean DispOrigin = Standard_True, const Standard_Boolean DispCurvRadius = Standard_False, const Standard_Real RadiusMax = 1.0e3, const Standard_Real RatioOfRadius = 0.1);
  
  Standard_EXPORT virtual gp_Pnt2d Pnt2d() const;
  
  Standard_EXPORT void ChangePnt2d (const gp_Pnt2d& P);
  
  Standard_EXPORT void ChangeCurve (const Handle(Geom2d_Curve)& C);
  
  Standard_EXPORT void ChangeText (const Standard_CString T);
  
  Standard_EXPORT virtual void Name (const Standard_CString N) Standard_OVERRIDE;
  
  //! For variable whatis command.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(TestTopOpeDraw_DrawableC2D,DrawTrSurf_Curve2d)

protected:


  Handle(Draw_Text2D) myText2D;
  Standard_CString myText;


private:


  Draw_Color myTextColor;


};







#endif // _TestTopOpeDraw_DrawableC2D_HeaderFile

// Created on: 1997-03-19
// Created by: Prestataire Mary FABIEN
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

#ifndef _TestTopOpeDraw_DrawableMesure_HeaderFile
#define _TestTopOpeDraw_DrawableMesure_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <Draw_Color.hxx>
#include <TestTopOpeDraw_HArray1OfDrawableP3D.hxx>
#include <Standard_Real.hxx>
#include <TestTopOpeDraw_DrawableC3D.hxx>
#include <Draw_Interpretor.hxx>
class TestTopOpeDraw_DrawableC3D;
class TestTopOpeTools_Mesure;
class Draw_Color;
class TCollection_AsciiString;
class gp_Pnt;
class Draw_Display;


class TestTopOpeDraw_DrawableMesure;
DEFINE_STANDARD_HANDLE(TestTopOpeDraw_DrawableMesure, TestTopOpeDraw_DrawableC3D)


class TestTopOpeDraw_DrawableMesure : public TestTopOpeDraw_DrawableC3D
{

public:

  
  Standard_EXPORT TestTopOpeDraw_DrawableMesure(const TestTopOpeTools_Mesure& M, const Draw_Color& CurveColor, const Draw_Color& TextColor, const Standard_Real ScaleX = 1.0, const Standard_Real ScaleY = 1.0);
  
  Standard_EXPORT void SetScale (const Standard_Real ScaleX, const Standard_Real ScaleY);
  
  Standard_EXPORT void SetScaleX (const Standard_Real ScaleX);
  
  Standard_EXPORT void SetScaleY (const Standard_Real ScaleY);
  
  Standard_EXPORT void SetName (const TCollection_AsciiString& Name);
  
  Standard_EXPORT virtual gp_Pnt Pnt() const Standard_OVERRIDE;
  
  Standard_EXPORT const Handle(TColgp_HArray1OfPnt)& Pnts() const;
  
  Standard_EXPORT void Clear();
  
  //! For variable whatis command.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void DrawOn (Draw_Display& dis) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTI(TestTopOpeDraw_DrawableMesure,TestTopOpeDraw_DrawableC3D)

protected:




private:


  Handle(TColgp_HArray1OfPnt) myP;
  Draw_Color myCurveColor;
  Handle(TestTopOpeDraw_DrawableC3D) myAXE1;
  Handle(TestTopOpeDraw_DrawableC3D) myAXE2;
  Handle(TestTopOpeDraw_HArray1OfDrawableP3D) myHDP;
  Handle(TestTopOpeDraw_HArray1OfDrawableP3D) myHADP1;
  Handle(TestTopOpeDraw_HArray1OfDrawableP3D) myHADP2;
  Standard_Real myScaleX;
  Standard_Real myScaleY;


};







#endif // _TestTopOpeDraw_DrawableMesure_HeaderFile
